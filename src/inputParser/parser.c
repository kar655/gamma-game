

#define _XOPEN_SOURCE 700

#include "parser.h"
#include "../gameMode/batchMode.h"
#include "../gameMode/interactiveMode.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>

int lineNumber = 1;

inline void nextLine() {
    lineNumber++;
}

void textMessage(char *str) {
    if (str == NULL) {
        errorMessage();
        return;
    }

    printf("%s", str);
    free(str);
    lineNumber++;
    fflush(stdout);
}

void lineMessage(int value) {
    printf("%d\n", value);
    lineNumber++;
    fflush(stdout);
}

inline void okMessage() {
    printf("OK %d\n", lineNumber++);
    fflush(stdout);
}

inline void errorMessage() {
    fprintf(stderr, "ERROR %d\n", lineNumber++);
}

static inline bool correctNumber(uint64_t num) {
//    num > 0 &&
    return num <= UINT32_MAX;
}

int readNumbers(uint32_t values[], char *str, int expectingValues) {
//    uint32_t values[4];
    uint64_t result;
    int num = 0;
    char *endPtr;

    for (num = 0; num < expectingValues; num++) {
//        if (strtok(str, " \t\v\f\r\n") == NULL)
//            return -1;

        result = strtoul(str, &endPtr, 10);

        if (!correctNumber(result) || str == endPtr)
            return -1;

        str = endPtr;
        values[num] = result;
    }

    // anything different than white spaces on the end of the line
    if (strtok(str, " \t\v\f\r\n") != NULL)
        return -1;

    return num;
}

//B width height players areas
//I width height players areas

// todo tutaj lineNumber++;
// bylo void
int giveLine(char **str) {
    size_t inputSize = 0;
    char *instructions = NULL;
    uint32_t values[4];

    ssize_t read = getline(&instructions, &inputSize, stdin);

    if (read == -1) {
        free(instructions);
        //exit(2);
        return -1;
    }

    if (instructions == NULL) {
        free(instructions);
//        exit(1);
        assert(false);
    }


    *str = instructions;

    return 0;
}

int getMode() {

//    size_t inputSize = 0;
//    char *instructions = NULL;
//    uint32_t values[4];
//
//    ssize_t read = getline(&instructions, &inputSize, stdin);
//
//    if (instructions == NULL)
//        exit(1);
//
//    if (read == -1) {
//        free(instructions);
//        exit(2);
//    }
//

    char *instructions;
    if (giveLine(&instructions) == -1)
        return -1;
    uint32_t values[4];
//    giveValues(values, instructions + 1);
    if (instructions[0] == '#' || instructions[0] == '\n') {
        lineNumber++;
        free(instructions);
        return 0;
    }

    if (instructions[0] != 'B' && instructions[0] != 'I') {
        errorMessage();
        free(instructions);
        return 0;
    }

    // proper input and correct number of values
    if (readNumbers(values, instructions + 1, 4) == -1) {
        errorMessage();
        free(instructions);
        return 0;
    }

    if (instructions[0] == 'B') {
        // batch mode with values
        if (initializeBatch(values)) {
            free(instructions);
            return 1; // game completed
        }
    } else {
        // interactive mode with values
        if (initializeInteractive(values)) {
            free(instructions);
            return 1; // game completed
        }
    }

    // wrong numbers to make new gamma game or not enough memory
    free(instructions);
    errorMessage();
    // not enough memory to allocate certain board
    return 0;
}
