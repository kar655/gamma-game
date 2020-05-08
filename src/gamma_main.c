
#include "inputParser/parser.h"

#include <stdio.h>
#include <stdbool.h>

int main() {

    // 0 - keep reading, 1 - batch, 2 - interactive
    int mode;

    while (true) {
        mode = getMode();

        if (mode != 0)
            break;

        fflush(stdout);
    }

//    playMode(mode);
//
//    clearMemory();

    return 0;
}