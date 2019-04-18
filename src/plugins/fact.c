#include "plg_common.h"
#include <math.h>

CALC_FN(fact) {
  typedef unsigned long long num_t;
  const num_t cnt = floor(fabs(x));
  if(cnt == 0) {
    x = 1;
  } else {
    num_t x2 = 1;
    for(num_t tmp = cnt; tmp; --tmp) x2 *= tmp;
    x *= x2 / ((double)cnt);
  }
  return x;
}
