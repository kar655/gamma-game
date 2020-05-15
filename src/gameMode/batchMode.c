/** @file
 * Batch mode implementation
 *
 * @author Karol Zagródka <karol.zagrodka@gmail.com>
 */

#include "batchMode.h"
#include "../gamma.h"
#include "../inputParser/parser.h"

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

/**
 * Current gamma game
 */
gamma_t *game;

/** @brief Plays game in Batch Mode
 * Reads lines and prints results.
 */
static void gameLoop();

bool initializeBatch(uint32_t values[]) {
    game = gamma_new(values[0], values[1], values[2], values[3]);

    if (game == NULL)
        return false;

    okMessage();
    gameLoop();

    gamma_delete(game);
    return true;
}

static void gameLoop() {
    char *line;
    uint32_t values[4];

    while (giveLine(&line)) {

        // comment or new line
        if (line[0] == '#' || line[0] == '\n') {
            free(line);
            continue;
        }

        // no whitespace char after command
        if (strlen(line) > 1 && isspace(line[1]) == 0) {
            free(line);
            errorMessage();
            continue;
        }

        if (line[0] == 'm') {
            if (readNumbers(values, line + 1, 3))
                printf("%d\n",
                       gamma_move(game, values[0], values[1], values[2]));
            else
                errorMessage();
        }
        else if (line[0] == 'g') {
            if (readNumbers(values, line + 1, 3))
                printf("%d\n",
                       gamma_golden_move(game, values[0], values[1], values[2]));
            else
                errorMessage();
        }
        else if (line[0] == 'b') {
            if (readNumbers(values, line + 1, 1))
                printf("%lu\n",
                       gamma_busy_fields(game, values[0]));
            else
                errorMessage();
        }
        else if (line[0] == 'f') {
            if (readNumbers(values, line + 1, 1))
                printf("%lu\n",
                       gamma_free_fields(game, values[0]));
            else
                errorMessage();
        }
        else if (line[0] == 'q') {
            if (readNumbers(values, line + 1, 1))
                printf("%d\n",
                       gamma_golden_possible(game, values[0]));
            else
                errorMessage();
        }
        else if (line[0] == 'p') {
            if (readNumbers(values, line + 1, 0))
                textMessage(gamma_board(game));
            else
                errorMessage();
        }
        else
            errorMessage();


        free(line);
        fflush(stdout);
    }
}
