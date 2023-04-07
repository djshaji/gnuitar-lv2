#include "lv2/core/lv2.h"
#include <stdio.h>
#include "rcfilter.c"

#define URI "http://shaji.in/plugins/gnuitar-distort"

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

