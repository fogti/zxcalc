#include "plg_common.h"
#include <math.h>

CALC_FN(fact) {
  typedef unsigned long long num_t;
  const num_t cnt = floor(fabs(x));
  if(cnt == 0) return 1;
  for(num_t tmp = cnt - 1; tmp; --tmp)
    x *= tmp;
  return x;
}

// gamma(x + 1) = fact(x); but scales better on non-int values
CALC_FN(gamma) {
  const double x2 = tgamma(fabs(x));
  return (x < 0) ? (-x2) : (x2);
}
