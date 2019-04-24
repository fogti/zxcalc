#include "plg_common.h"

CALC_FN(gamma) { return zx_sgnprop(tgamma(fabs(x)), x); }
CALC_FN(fact)  { return gamma_calc(x + 1); }
