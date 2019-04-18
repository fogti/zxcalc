#include "plg_common.h"
#include <math.h>

CALC_FN(fact) {
  const int cnt = floor(fabs(x));
  if(cnt == 0) {
    x = 1;
  } else {
    int x2 = 1;
    for(int tmp = cnt; tmp; --tmp) x2 *= tmp;
    x *= x2 / ((double)cnt);
  }
  return x;
}
