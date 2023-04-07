#include "lv2/core/lv2.h"
#include <stdio.h>
#include "rcfilter.c"

#define URI "http://shaji.in/plugins/gnuitar-autowah"

typedef enum {
    INPUT = 0,
    OUTPUT = 1,
    FREQ_LOW = 2,
    FREQ_HIGH = 3,
    COUNT = 4,
    RESONANCE = 5,
    DELTA = 6,
    MIX = 7
} PortIndex;

typedef struct {
    const float * input; 
	float * output ;
	float * freq_low ;
    float * freq_high ;
    float * count ;
    float * resonance ;
    float * delta ;
    float * mix ;
    int samplerate ;
    struct filter_data *fd;
} Autowah;

static LV2_Handle
instantiate(const LV2_Descriptor*     descriptor,
            double                    rate,
            const char*               bundle_path,
            const LV2_Feature* const* features)
{
    Autowah * plugin = (Autowah*)calloc(1, sizeof(Autowah));
    plugin -> samplerate = rate ;
}

static void
connect_port(LV2_Handle instance, uint32_t port, void* data)
{
  Autowah* plugin = (Autowah*)instance;

  switch ((PortIndex)port) {
  case INPUT:
    plugin->input = (float*)data;
    break;
  case OUTPUT:
    plugin->output = (float*)data;
    break;
  case FREQ_LOW:
    plugin->freq_low = (float*)data;
    break;
  case FREQ_HIGH:
    plugin->freq_high = (float*)data;
    break;
  case COUNT:
    plugin->count = (float*)data;
    break;
  case RESONANCE:
    plugin->resonance = (float*)data;
    break;
  case DELTA:
    plugin->delta = (float*)data;
    break;
  case MIX:
    plugin->mix = (float*)data;
    break;
  }
}

static void
activate(LV2_Handle instance)
{
	Autowah* plugin = (Autowah*)instance;
    plugin->fd = (struct filter_data *) malloc(sizeof(struct filter_data));    	
}

static void
run(LV2_Handle instance, uint32_t n_samples)
{
    Autowah * plugin = (Autowah *) instance ;
	float freq_low = * plugin -> freq_low;
    float freq_high = * plugin -> freq_high ;
    float count = * plugin -> count ;
    float resonance = * plugin -> resonance ;
    float delta = * plugin -> delta ;
    float mix = * plugin -> mix ;
    int buffer_size = 128 ; // aaaaaaaargh

    resonance = 	freq_high - freq_low * 1000.0 * buffer_size / (plugin -> samplerate *
						     1 * resonance ;
}
