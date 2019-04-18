#include "plg_common.h"
#include <math.h>

// Dezibel
CALC_FN(dB)    { return log10(x)    * 10.0 ; }
CALCINV_FN(dB) { return pow(10.0, x / 10.0); }
