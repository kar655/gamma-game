#include "gamma.h"
#include <stdio.h>
#include <assert.h>

int main() {
    printf("%s", "Testing...\n");
    gamma_t *g = gamma_new(3, 3, 2, 1);
    assert(gamma_move(g, 1, 0, 0));
    assert(gamma_move(g, 1, 0, 1));
    assert(gamma_move(g, 1, 0, 2));
    assert(gamma_move(g, 2, 1, 0));
    assert(gamma_move(g, 2, 1, 1));
    assert(gamma_move(g, 2, 1, 2));
    assert(gamma_move(g, 2, 2, 0));
    assert(gamma_move(g, 2, 2, 2));

    assert(!gamma_golden_possible(g, 1));

    return 0;
}
