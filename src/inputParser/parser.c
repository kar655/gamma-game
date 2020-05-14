
#define _XOPEN_SOURCE 700

#include "parser.h"
#include "../gameMode/batchMode.h"
#include "../gameMode/interactiveMode.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>


int lineNumber = 0;

void textMessage(char *str) {
    if (str == NULL) {
        errorMessage();
        return;
    }

    printf("%s", str);
    free(str);
}

inline void okMessage() {
    printf("OK %d\n", lineNumber);
    fflush(stdout);
}

inline void errorMessage() {
    fprintf(stderr, "ERROR %d\n", lineNumber);
}

static inline bool correctNumber(uint64_t num) {
    return num <= UINT32_MAX;
}

bool readNumbers(uint32_t values[], char *str, int expectingValues) {
    uint64_t result;
    char *endPtr;

    for (int num = 0; num < expectingValues; num++) {

        result = strtoul(str, &endPtr, 10);

        if (!correctNumber(result) || str == endPtr)
            return false;

        str = endPtr;
        values[num] = result;
    }

    // anything different than white spaces on the end of the line
    if (strtok(str, " \t\v\f\r\n") != NULL)
        return false;

    return true;
}

bool giveLine(char **str) {
    lineNumber++;
    size_t inputSize = 0;
    char *instructions = NULL;

    ssize_t read = getline(&instructions, &inputSize, stdin);

    if (read == -1) {
        free(instructions);
        return false;
    }

    *str = instructions;
    return true;
}

static bool gameSuccess() {

    char *instructions;
    uint32_t values[4];

    if (!giveLine(&instructions))
        return true;

    // comment or new line
    if (instructions[0] == '#' || instructions[0] == '\n') {
        free(instructions);
        return false;
    }

    if ((instructions[0] == 'B' || instructions[0] == 'I')
        && readNumbers(values, instructions + 1, 4)) {
        if (instructions[0] == 'B') {
            // batch mode with values
            if (initializeBatch(values)) {
                free(instructions);
                return true; // game completed
            }
        }
        else {
            // interactive mode with values
            if (initializeInteractive(values)) {
                free(instructions);
                return true; // game completed
            }
        }
    }

    free(instructions);
    errorMessage();
    return false;
}

void playGame() {
    while (!gameSuccess()) {
        fflush(stdout);
    }
}
