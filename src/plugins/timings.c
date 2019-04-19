#include "plg_common.h"
#include "zxmath.h"
#include <math.h>

CALC_FN(day)    { return x * 1440; }
CALCINV_FN(day) { return x / 1440; }

CALC_FN(hour)    { return x * 60; }
CALCINV_FN(hour) { return zx_modulo(x / 60, 24); }

CALC_FN(min)    { return x; }
CALCINV_FN(min) { return zx_modulo(x, 60); }

CALC_FN(sec)    { return              x / 60; }
CALCINV_FN(sec) { return (x - trunc(x)) * 60; }
