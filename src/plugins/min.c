#include "plg_common.h"
#include <math.h>

// just for convenience
CALC_FN(min) {
  return x;
}

CALCINV_FN(min) {
  x /= 60.0;
  return (x - trunc(x)) * 60.0;
}
