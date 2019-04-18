#include "plg_common.h"

CALC_FN(invaperc) {
  return 1.0 / (1.0 - x / 100.0);
}
