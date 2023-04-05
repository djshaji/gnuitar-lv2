#include "lv2/core/lv2.h"

/** Include standard C headers */
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include "definitions.h"
#include "lv2/log/log.h"
#include "lv2/log/logger.h"

#define URI "http://shaji.in/plugins/gnuitar-delay"
#define MAX_STEP 65000
#define MAX_COUNT 1025
#define MAX_SIZE (MAX_STEP*MAX_COUNT)

/**
   In code, ports are referred to by index.  An enumeration of port indices
   should be defined for readability.
*/
typedef enum {
    INPUT = 0,
    OUTPUT = 1,
    TIME = 2,
    DECAY = 3,
    REPEAT = 4
} PortIndex;
/**
   Every plugin defines a private structure for the plugin instance.  All data
   associated with a plugin instance is stored here, and is available to
   every instance method.  In this simple plugin, only port buffers need to be
   stored, since there is no additional instance data.
*/
typedef struct {

    float             *delay_size,	/* length of history */
                    *delay_decay,	/* volume of processed signal */
                    *delay_count;
    int                delay_start,
                    delay_step,
                   *idelay;
  const float* input;
  float * history ;
  float*       output;
  int index ;
} Delay;

/**
   The `instantiate()` function is called by the host to create a new plugin
   instance.  The host passes the plugin descriptor, sample rate, and bundle
   path for plugins that need to load additional resources (e.g. waveforms).
   The features parameter contains host-provided features defined in LV2
   extensions, but this simple plugin does not use any.

   This function is in the ``instantiation'' threading class, so no other
   methods on this instance will be called concurrently with it.
*/
static LV2_Handle
instantiate(const LV2_Descriptor*     descriptor,
            double                    rate,
            const char*               bundle_path,
            const LV2_Feature* const* features)
{
  Delay* delay = (Delay*)calloc(1, sizeof(Delay));
    delay -> history = (float *) malloc(MAX_SIZE * sizeof(float));
    delay ->idelay = (int *) malloc(MAX_COUNT * sizeof(int));
    /*
    delay ->delay_size = (int *) malloc(sizeof(int));
    delay ->delay_decay = (int *) malloc(sizeof(int));
    delay ->delay_count = (int *) malloc(sizeof(int));
    */

    //~ new_time = (int) adj->value * sample_rate * nchannels / 1000;

    //~ delay ->delay_size = MAX_SIZE;
    //*delay->delay_size = 480 * 48000 /*sample rate */  * 2 / 1000;
    //*delay->delay_decay = 550;
    delay->delay_start = 11300;
    delay->delay_step = 11300;
    //*delay->delay_count = 8;
    delay -> index = 0 ;

    memset(delay->history, 0, MAX_SIZE * sizeof(float));
    memset(delay->idelay, 0, MAX_COUNT * sizeof(int));
  

  return (LV2_Handle)delay;
}

/**
   The `connect_port()` method is called by the host to connect a particular
   port to a buffer.  The plugin must store the data location, but data may not
   be accessed except in run().

   This method is in the ``audio'' threading class, and is called in the same
   context as run().
*/
static vodi
connect_port(LV2_Handle instance, uint32_t port, void* data)
{
  if (port != 0 && port != 1)
    fprintf (stderr, "port: %d\t\tdata: %f\n", port, data) ;
  Delay* delay = (Delay*)instance;
  switch ((PortIndex)port) {
  case TIME:
    //~ delay->delay_size = (const float*)data;
    delay->delay_size = (const float *) data ;
    break;
  case REPEAT:
    delay->delay_count = (const float *)data;
    break;
  case DECAY:
    delay->delay_decay = (const float*)data;
    break;
  case INPUT:
    delay->input = (const float*)data;
    break;
  case OUTPUT:
    delay->output = (float*)data;
    break;
  }
}

/**
   The `activate()` method is called by the host to initialise and prepare the
   plugin instance for running.  The plugin must reset all internal state
   except for buffer locations set by `connect_port()`.  Since this plugin has
   no other internal state, this method does nothing.

   This method is in the ``instantiation'' threading class, so no other
   methods on this instance will be called concurrently with it.
*/
static void
activate(LV2_Handle instance)
{
    /*
    Delay *delay = (Delay *) instance;
    delay->delay_size = 480 * 48000   * 2 / 1000;
    delay->delay_decay = 550;
    delay->delay_start = 11300;
    delay->delay_step = 11300;
    delay->delay_count = 8;
    delay -> index = 0 ;

    memset(delay->history, 0, MAX_SIZE * sizeof(float));
    memset(delay->idelay, 0, MAX_COUNT * sizeof(int));
  
  */
}

/**
   The `run()` method is the main process function of the plugin.  It processes
   a block of audio in the audio context.  Since this plugin is
   `lv2:hardRTCapable`, `run()` must be real-time safe, so blocking (e.g. with
   a mutex) or memory allocation are not allowed.
*/
static void
run(LV2_Handle instance, uint32_t n_samples)
{
  Delay* dp = (const Delay*)instance;
  float delay = *(dp->delay_size),
  repeat = *(dp->delay_count),
  decay = *(dp->delay_decay);
  
  if (delay != 46080)
   fprintf (stderr, "size: %d\tcount: %d\tdecay: %d\n", delay, repeat, decay) ;
  /*
    fprintf (stderr,
        "size: %d\t\tcount: %d\n",
        dp -> delay_size, 
        dp -> delay_count
    );
    */


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
      if (dp->index == dp->delay_size)
          dp->index = 0;

      current_decay = dp->delay_decay;
      for (i = 0; i < dp->delay_count; i++) {
          if (dp->index >= dp->idelay[i]) {
            //~ fprintf (stderr, "dp->index >= dp->idelay[i]");
            if (dp->index - dp->idelay[i] == dp->delay_start + i * dp->delay_step) {
                  dp->idelay[i]++;
                  //~ fprintf (stderr, "dp->index - dp->idelay[i] == dp->delay_start + i * dp->delay_step");
              }
          } else if (dp->delay_size + dp->index - dp->idelay[i] == dp->delay_start + i * dp->delay_step) {
                  dp->idelay[i]++;
                  //~ fprintf (stderr,"dp->delay_size + dp->index - dp->idelay[i] == dp->delay_start + i * dp->delay_step");
          }
          
          if (dp->idelay[i] == dp->delay_size)
            dp->idelay[i] = 0;

          //~ fprintf (stderr, "%d: %d\n", i, dp->idelay[i]);
          *s += dp->history[dp->idelay[i]] * current_decay / 1000;
          current_decay = current_decay * (int)dp->delay_decay / 1000;
      }
      dp -> output [pos] = *s ;
      pos ++ ;
      s ++ ;
      count--;
    }

}




/**
   The `deactivate()` method is the counterpart to `activate()`, and is called
   by the host after running the plugin.  It indicates that the host will not
   call `run()` again until another call to `activate()` and is mainly useful
   for more advanced plugins with ``live'' characteristics such as those with
   auxiliary processing threads.  As with `activate()`, this plugin has no use
   for this information so this method does nothing.

   This method is in the ``instantiation'' threading class, so no other
   methods on this instance will be called concurrently with it.
*/
static void
deactivate(LV2_Handle instance)
{}

/**
   Destroy a plugin instance (counterpart to `instantiate()`).

   This method is in the ``instantiation'' threading class, so no other
   methods on this instance will be called concurrently with it.
*/
static void
cleanup(LV2_Handle instance)
{
  free(instance);
}

/**
   The `extension_data()` function returns any extension data supported by the
   plugin.  Note that this is not an instance method, but a function on the
   plugin descriptor.  It is usually used by plugins to implement additional
   interfaces.  This plugin does not have any extension data, so this function
   returns NULL.

   This method is in the ``discovery'' threading class, so no other functions
   or methods in this plugin library will be called concurrently with it.
*/
static const void*
extension_data(const char* uri)
{
  return NULL;
}

/**
   Every plugin must define an `LV2_Descriptor`.  It is best to define
   descriptors statically to avoid leaking memory and non-portable shared
   library constructors and destructors to clean up properly.
*/
static const LV2_Descriptor descriptor = {URI,
                                          instantiate,
                                          connect_port,
                                          activate,
                                          run,
                                          deactivate,
                                          cleanup,
                                          extension_data};

/**
   The `lv2_descriptor()` function is the entry point to the plugin library. The
   host will load the library and call this function repeatedly with increasing
   indices to find all the plugins defined in the library.  The index is not an
   identifier, the URI of the returned descriptor is used to determine the
   identify of the plugin.

   This method is in the ``discovery'' threading class, so no other functions
   or methods in this plugin library will be called concurrently with it.
*/
LV2_SYMBOL_EXPORT
const LV2_Descriptor*
lv2_descriptor(uint32_t index)
{
  return index == 0 ? &descriptor : NULL;
}

