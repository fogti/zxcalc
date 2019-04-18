#include "plg_common.h"
#include <math.h>

CALC_FN(base2)    { return pow(2, x); }
CALCINV_FN(base2) { return   log2(x); }
