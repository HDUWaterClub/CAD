#ifndef MISC_H_
#define MISC_H_

#include <stdbool.h>

#ifndef bool
#define bool _Bool
#endif

#ifndef min
#define min(x, y) (((x) < (y)) ? (x) : (y))
#endif

#ifndef max
#define max(x, y) (((x) > (y)) ? (x) : (y))
#endif

long long int quickPow(long long int a, long long int n);

#endif
