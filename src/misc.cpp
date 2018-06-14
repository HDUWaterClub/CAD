#include "misc.h"

long long int quickPow(long long int a, long long int n) {
    long long int ans = 1;
    while (n > 0) {
        if (n & 1)
            ans = ans * a;
        a = a * a;
        n >>= 1;
    }
    return ans;
}
