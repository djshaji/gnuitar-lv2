#include "lv2/core/lv2.h"
#include <stdio.h>
#include "rcfilter.c"
#include "chebyshev.c"
#include "biquad.c"

#define URI "http://shaji.in/plugins/gnuitar-distort2"
#define UPSAMPLE	4

#define DIST2_DOWNSCALE	16.0 / MAX_SAMPLE
#define DIST2_UPSCALE	MAX_SAMPLE * 0.2

typedef enum {
    INPUT = 0,
    OUTPUT = 1,
    DRIVE = 2,
    TREBLE = 3,
    NOISEGATE = 4
} PortIndex;

typedef struct {
    const float * input; 
	float * output ;
	float * drive ;
    float * noisegate ;
    float * treble ;
    int samplerate ;
    struct filter_data noise;
    double	c0,d1,lyf;
    double 	last[MAX_CHANNELS];
    double	lastupsample;
    struct Biquad cheb, cheb1;  /* chebyshev filters */    
} Distort2;

static LV2_Handle
instantiate(const LV2_Descriptor*     descriptor,
            double                    rate,
            const char*               bundle_path,
            const LV2_Feature* const* features)
{
    Distort2 * distort = (Distort2*)calloc(1, sizeof(Distort));
    distort -> samplerate = rate ;
    distort -> noisegate = 3500 ;
    int         i;
    double	Ts, Ts1;
    double	RC=(0.047*1e-6)*(4.7*1e+3);
    Ts = 1.0/distort->samplerate;
    Ts1 = Ts / UPSAMPLE;  /* Ts1 is Ts for upsampled processing  */

    /* Init stuff
     * This is the rc filter tied to ground. */
    distort->c0 = Ts1 / (Ts1 + RC);
    distort->d1 = RC / (Ts1 + RC);
    distort->lyf = 0;

    distort->last[0] = 0;
    distort->lastupsample = 0;
    distort->cheb.mem = (double*) malloc ( 1 * sizeof (double) * 4 );
    memset ( (void*) distort->cheb.mem, 0 , 1 * sizeof (double) * 4 );
    distort->cheb1.mem = (double*) malloc ( 1 * sizeof (double) * 4 );
    memset ( (void*) distort->cheb1.mem, 0 , 1 * sizeof (double) * 4 );

    /* 2 lowPass Chebyshev filters used in downsampling */
    CalcChebyshev2(samplerate * UPSAMPLE, 12000, 1, 1, &distort->cheb);
    CalcChebyshev2(samplerate * UPSAMPLE, 5500, 1, 1, &distort->cheb1);

    return (LV2_Handle)distort;
}

static void
connect_port(LV2_Handle instance, uint32_t port, void* data)
{
  Distort2* plugin = (Distort2*)instance;

  switch ((PortIndex)port) {
  case INPUT:
    plugin->input = (float*)data;
    break;
  case OUTPUT:
    plugin->output = (float*)data;
    break;
  case TREBLE:
    plugin->treble = (float*)data;
    break;
  case DRIVE:
    plugin->drive = (float*)data;
    break;
  case NOISEGATE:
    plugin->noisegate = (float*)data;
    RC_set_freq(plugin->noisegate, &(plugin->noise));

    break;
  }
}

static void
activate(LV2_Handle instance)
{
    Distort2* plugin = (Distort2*)instance;
    RC_setup(10, 1, &(distort->noise));
    RC_set_freq(distort->noisegate, &(distort->noise));

}

static void
run(LV2_Handle instance, uint32_t n_samples)
{
	Distort2 * dp = (Distort2 *) instance ;
    float drive =  0 ;
    drive = (int) * dp -> drive * 5000;
    drive -= drive % 10;
    drive += 50;
    
    float treble = * dp -> treble ;
    float noisegate = * dp -> noisegate ;
    
#define mUt				  30*1e+2
#define Is				 10*1e-12

    int			i,count;
    static int		curr_channel = 0;
    float 	       *s;
    static double	x,y,x1,f,df,dx,e1,e2;
    static double upsample [UPSAMPLE];
#define DRIVE (drive)
    count = n_samples;
    s = dp -> input;

    /* no input, no output :-) to avoid extra calc. Optimized for noise gate,
     * when all input is zero.
     * This is the heuristics - since there is no the standard function
     * in the ANSI C library that reliably compares the memory region
     * with the given byte, we compare just a few excerpts from an array.
     * If everything is zero, we have a large chances that all array is zero. */
    if(s[0]==0 && s[1]==0 && s[16]==0 && s[17]==0 &&
          s[24]==0 && s[25]==0 && s[32]==0 && s[33]==0 &&
	  s[buffer_size-1]==0) {
	dp->last[0]=dp->last[1]=dp->lastupsample=0;
        return;
    }

    /*
     * process signal; x - input, in the range -1, 1
     */
    while (count) {

	/* scale down to -1..1 range */
	x = *s ;
	x *= DIST2_DOWNSCALE ;
	
	/* first we prepare the lineary interpoled upsamples */
	y = 0;
	upsample[0] = dp->lastupsample;
	y = 1.0 / UPSAMPLE;  /* temporary usage of y */
	for (i=1; i< UPSAMPLE; i++)
	{
	    upsample[i] = dp->lastupsample + ( x - dp->lastupsample) *y;
	    y += 1.0 / UPSAMPLE;
	}
	dp->lastupsample = x;
	/* Now the actual upsampled processing */
	for (i=0; i<UPSAMPLE; i++)
	{
	    x = upsample[i]; /*get one of the upsamples */

	    /* first compute the linear rc filter current output */
	    y = dp->c0*x + dp->d1 * dp->lyf;
	    dp->lyf = y;
	    x1 = (x-y) / 4700.0;

	    /* start searching from time previous point , improves speed */
	    y = dp->last[curr_channel];
	    do {
		/* f(y) = 0 , y= ? */
		e1 = exp ( (x-y) / mUt );  e2 = 1.0 / e1;
    	
		/* f=x1+(x-y)/DRIVE+Is*(exp((x-y)/mUt)-exp((y-x)/mUt));  optimized makes : */
		f = x1 + (x-y)/ DRIVE + Is * (e1 - e2);
	
		/* df/dy */
		/*df=-1.0/DRIVE-Is/mUt*(exp((x-y)/mUt)+exp((y-x)/mUt)); optimized makes : */
		df = -1.0 / DRIVE - Is / mUt * (e1 + e2);
	
		/* This is the newton's algo, it searches a root of a function,
		 * f here, which must equal 0, using it's derivate. */
		dx=f/df;
		y-=dx;
	    }
	    while ((dx>0.01)||(dx<-0.01));
	    /* when dx gets very small, we found a solution. */

	    dp->last[curr_channel] = y;
	    y = doBiquad( y, &dp->cheb, curr_channel);
	    y = doBiquad( y, &dp->cheb1, curr_channel);
	}

	/* scale up from -1..1 range */
	*s = y * DIST2_UPSCALE;

	/*if(*s > MAX_SAMPLE)
	    *s=MAX_SAMPLE;
	else if(*s < -MAX_SAMPLE)
	    *s=-MAX_SAMPLE;*/

	s++;
	count--;

	if (nchannels > 1)
	    curr_channel = !curr_channel;

    }
    if(dp->treble)
	RC_lowpass(db->data, db->len, &(dp->noise));
#undef DRIVE
    
    
}
