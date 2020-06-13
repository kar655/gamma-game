/** @file
 * Implementation of input parser
 *
 * @author Karol Zagródka <karol.zagrodka@gmail.com>
 */

/** Enables getline */
#define _XOPEN_SOURCE 700

#include "parser.h"
#include "../gameMode/batchMode.h"
#include "../gameMode/interactiveMode.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <ctype.h>

/**
 * Line number counter.
 */
int lineNumber = 0;

/** @brief Check if number is correct.
 * Check if @p num fits in uint32_t
 * @param num - checked number
 * @return true if number fits else false
 */
static bool correctNumber(uint64_t num);

/** @brief Tries to chose and launch game mode.
 * Read single line by giveLine and tries to play game.
 * @return true if game was played or EOF else false
 */
static bool gameSuccess();

static inline bool correctNumber(uint64_t num) {
    return num <= UINT32_MAX;
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

// todo no error message when number with + i.e. B +3 5 2 1
bool readNumbers(uint32_t values[], char *str, int expectingValues) {
    uint64_t result;
    char *endPtr;

    if (strlen(str) == 0 || isspace(str[0]) == 0) // no free space
        return false;


    for (int num = 0; num < expectingValues; num++) {

        uint64_t shift = 0;
        // not a number
        while((str[shift] < '0' || str[shift] > '9')) {
            // not a whitespace
            if (isspace(str[shift]) == 0) {
                return false;
            }
            shift++;
        }
        str += shift;

        result = strtoul(str, &endPtr, 10);

        if (!correctNumber(result) || str == endPtr)
            return false;

        str = endPtr;
        values[num] = result;
    }

    // anything different than whitespaces on the end of the line
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

void playGame() {
    while (!gameSuccess()) {
        fflush(stdout);
    }
}
