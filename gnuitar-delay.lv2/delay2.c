#include "lv2/core/lv2.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define URI "http://shaji.in/plugins/gnuitar-delay"
#define MAX_STEP 65000
#define MAX_COUNT 1025
#define MAX_SIZE (MAX_STEP*MAX_COUNT)

typedef enum {
    INPUT = 0,
    OUTPUT = 1,
    TIME = 2,
    DECAY = 3,
    REPEAT = 4
} PortIndex;

typedef struct {
    float * delay_size;
    float * delay_decay ;
    float * delay_count;
    int delay_start ;
    int delay_step  ;
    int * idelay;
    const float * input;
    float * history ;
    float * output;
    int index ;
    double sample_rate ;
} Delay;

static LV2_Handle
instantiate(const LV2_Descriptor*     descriptor,
            double                    rate,
            const char*               bundle_path,
            const LV2_Feature* const* features)
{
    Delay * delay = (Delay*)calloc(1, sizeof(Delay));
    delay -> sample_rate = rate ;
    delay -> delay_start = delay -> delay_step = 11300 ;
    delay -> index = 0 ;
    delay -> delay_count = delay -> delay_decay = delay -> delay_size = NULL ;
    delay -> history = (float *) malloc(MAX_SIZE * sizeof(float));
    delay ->idelay = (int *) malloc(MAX_COUNT * sizeof(int));
    memset(delay->history, 0, MAX_SIZE * sizeof(float));
    memset(delay->idelay, 0, MAX_COUNT * sizeof(int));
    
    return (LV2_Handle)delay;
}

static void
connect_port(LV2_Handle instance, uint32_t port, void* data)
{
  Delay* delay = (Delay*)instance;

  switch ((PortIndex)port) {
  case DECAY:
    delay->delay_decay = (float*)data;
    break;
  case REPEAT:
    delay->delay_count = (float*)data;
    break;
  case TIME:
    delay->delay_size = (float*)data;
    break;
  case INPUT:
    delay->input = (float*)data;
    break;
  case OUTPUT:
    delay->output = (float*)data;
    break;
  }
}

static void
activate(LV2_Handle instance)
{}

static void
run(LV2_Handle instance, uint32_t n_samples)
{
    Delay* dp = (const Delay*)instance;
    float delay = *(dp->delay_size);
    fprintf (stderr, "%f", delay) ;
    float repeat = *(dp->delay_count);
    float decay = *(dp->delay_decay);
  
    if (delay != 46080)
        fprintf (stderr, "size: %f\tcount: %f\tdecay: %f\n", delay, repeat, decay) ;

    int             i, count, pos = 0 ;
    int                current_decay;
    float     *s = dp -> input ;

    count = n_samples;

    while (count) {
        //~ dp -> output [pos] = dp -> input [pos];
      /*
       * add sample to history 
       */
      dp->history[dp->index++] = *s;
      /*
       * wrap around 
       */
      if (dp->index == delay)
          dp->index = 0;

      current_decay = decay;
      for (i = 0; i < repeat; i++) {
          if (dp->index >= dp->idelay[i]) {
            if (dp->index - dp->idelay[i] == dp->delay_start + i * dp->delay_step) {
                  dp->idelay[i]++;
              }
          } else if (delay + dp->index - dp->idelay[i] == dp->delay_start + i * dp->delay_step) {
                  dp->idelay[i]++;
          }
          
          if (dp->idelay[i] == delay)
            dp->idelay[i] = 0;

          *s += dp->history[dp->idelay[i]] * current_decay / 1000;
          current_decay = current_decay * decay / 1000;
      }
      
      dp -> output [pos] = *s ;
      pos ++ ;
      s ++ ;
      count--;
    }

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
