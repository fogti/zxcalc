#include "plg_common.h"
#include <math.h>

CALC_FN(base2)    { return pow(2, x); }
CALCINV_FN(base2) { return   log2(x); }

CALC_FN(base10)    { return pow(10, x); }
CALCINV_FN(base10) { return   log10(x); }

// Dezibel
CALC_FN(dB)    { return log10(x)    * 10.0 ; }
CALCINV_FN(dB) { return pow(10.0, x / 10.0); }
