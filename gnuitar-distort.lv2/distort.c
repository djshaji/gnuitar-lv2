#include "lv2/core/lv2.h"
#include <stdio.h>
#include "rcfilter.c"

#define URI "http://shaji.in/plugins/gnuitar-distort"

typedef enum {
    INPUT = 0,
    OUTPUT = 1,
    DRIVE = 2,
    SAT = 3,
    LEVEL = 4,
    LOW_PASS = 5
} PortIndex;

typedef struct {
	const float * input; 
	float * output ;
	float * drive ;
	float * saturation ;
	float * level ;
	float *low_pass ;
	int noisegate ;
	int samplerate ;
	struct filter_data fd;
    struct filter_data noise;
	int lastval[MAX_CHANNELS] ;

} Distort ;

static LV2_Handle
instantiate(const LV2_Descriptor*     descriptor,
            double                    rate,
            const char*               bundle_path,
            const LV2_Feature* const* features)
{
    Distort * distort = (Distort*)calloc(1, sizeof(Distort));
    distort -> samplerate = rate ;
    distort -> noisegate = 3000 ; 
    RC_setup(10, 1.5, &(distort->fd));
    RC_set_freq(350, &(distort->fd));

    RC_setup(10, 1, &(distort->noise));
    RC_set_freq(distort->noisegate, &(distort->noise));
    
    distort -> lastval [0] = 1 ;
    return (LV2_Handle)distort;
}

static void
connect_port(LV2_Handle instance, uint32_t port, void* data)
{
  Distort* plugin = (Distort*)instance;

  switch ((PortIndex)port) {
  case INPUT:
    plugin->input = (float*)data;
    break;
  case OUTPUT:
    plugin->output = (float*)data;
    break;
  case LEVEL:
    plugin->level = (float*)data;
    break;
  case SAT:
    plugin->saturation = (float*)data;
    break;
  case DRIVE:
    plugin->drive = (float*)data;
    break;
  case LOW_PASS:
    plugin->low_pass = (float*)data;
    //fprintf (stderr, "%f\n", (float *)data) ;
    //RC_set_freq(*plugin->low_pass, &(plugin->fd));
    break;
  }
}

static void
activate(LV2_Handle instance)
{
	Distort* plugin = (Distort*)instance;
	
}

static void
run(LV2_Handle instance, uint32_t n_samples)
{
	Distort * dp = (Distort *) instance ;
	float drive = * dp -> drive * 10;
	float saturation = * dp -> saturation * 300 ;
	float level = * dp -> level * 2.56;
	float low_pass = * dp -> low_pass ;
	int             count,
                    currchannel = 0;
    float     *s;
    /*
     * sat clips derivative by limiting difference between samples. Use lastval 
     * member to store last sample for seamlessness between chunks. 
     */
    count = n_samples;
    //~ for (int i = 0 ; i < count ; i ++)
	//~ dp -> output [i] = dp -> input [i] ;
    s = dp -> input;
    //~ return ;
    //~ fprintf (stderr, "%f\t%f\t%f\n", drive, saturation, level);

    int i = 0 ;
    //~ RC_highpass(s, n_samples, &(dp->fd));

    while (count) {
		/*
		 * apply drive  
		 */
		*s *= drive;
		*s /= 16;

		/*
		 * apply sat 
		 */
		if ((*s - dp->lastval[currchannel]) > saturation)
			*s = dp->lastval[currchannel] + saturation;
		else if ((dp->lastval[currchannel] - *s) > saturation)
			*s = dp->lastval[currchannel] - saturation;

		dp->lastval[currchannel] = *s;
		if (nchannels > 1)
			currchannel = !currchannel;

		*s *= level;
		*s /= 256;
		dp -> output [i] = *s ;
		i ++ ;
		s++;
		count--;
    }

    LC_filter(s, n_samples, LOWPASS, low_pass, &(dp->noise));
}

static void
deactivate(LV2_Handle instance)
{}

static void
cleanup(LV2_Handle instance)
{
  free(instance);
}

static const void*
extension_data(const char* uri)
{
  return NULL;
}

static const LV2_Descriptor descriptor = {URI,
                                          instantiate,
                                          connect_port,
                                          activate,
                                          run,
                                          deactivate,
                                          cleanup,
                                          extension_data};


LV2_SYMBOL_EXPORT
const LV2_Descriptor*
lv2_descriptor(uint32_t index)
{
  return index == 0 ? &descriptor : NULL;
}
