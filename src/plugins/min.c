#include "plg_common.h"
#include <stdbool.h>
#include <math.h>

// just for convenience
CALC_FN(min) {
  return x;
}

CALCINV_FN(min) {
  const double u_bound = 60.0;
  const bool is_neg = (x < 0.0);
  x = fabs(x)        / u_bound;
  x = (x - floor(x)) * u_bound;
  return is_neg ? (-x) : x;
}
