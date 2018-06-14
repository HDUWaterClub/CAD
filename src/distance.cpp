#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include <assert.h>

#include "distance.h"

int getManhattanDistance(const struct Vertex * a, const struct Vertex * b) {
    assert(a != NULL && b != NULL);
    return abs(a -> x - b -> x) + abs(a -> y - b -> y);
}

long long int getSqrEuclideanDistance(const struct Vertex * a, const struct Vertex * b) {
    assert(a != NULL && b != NULL);
    return quickPow(a -> x - b -> x, 2) + quickPow(a -> y - b -> y, 2);
}
