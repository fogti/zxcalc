#include "plg_common.h"
#include <math.h>

CALC_FN(sec)    { return              x / 60.0; }
CALCINV_FN(sec) { return (x - trunc(x)) * 60.0; }
