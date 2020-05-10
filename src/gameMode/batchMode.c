//
// Created by karol on 05/05/2020.
//

#include "batchMode.h"
#include "../gamma.h"
#include "../inputParser/parser.h"

#include <stdlib.h>
#include <stdio.h>

gamma_t *game;

static void gameLoop();

bool initializeBatch(uint32_t values[]) {
    game = gamma_new(values[0], values[1], values[2], values[3]);

    if (game == NULL)
        return false;
//    return game != NULL;

    okMessage();
    gameLoop();

    gamma_delete(game);
    return true;
}


//m player x y – wywołuje funkcję gamma_move,
//g player x y – wywołuje funkcję gamma_golden_move,
//b player – wywołuje funkcję gamma_busy_fields,
//f player – wywołuje funkcję gamma_free_fields,
//q player – wywołuje funkcję gamma_golden_possible,
//p – wywołuje funkcję gamma_board.


static void gameLoop() {
    char *line;
    uint32_t values[4];
//    int num;

    while (giveLine(&line) != -1) {
//        giveLine(&line);
//        num = readNumbers(values, line + 1, 3);

        switch (line[0]) {
            case 'm':
                if (readNumbers(values, line + 1, 3) == 3) {
                    lineMessage(
                            gamma_move(game, values[0], values[1], values[2])
                    );
                }
                else {
                    errorMessage();
                }
                free(line);
                break;

            case 'g':
                if (readNumbers(values, line + 1, 3) == 3) {
                    lineMessage(
                            gamma_golden_move(game, values[0], values[1],
                                              values[2])
                    );
                }
                else {
                    errorMessage();
                }
                free(line);
                break;

            case 'b':
                if (readNumbers(values, line + 1, 1) == 1) {
                    lineMessage(
                            gamma_busy_fields(game, values[0])
                    );
                }
                else {
                    errorMessage();
                }
                free(line);
                break;

            case 'f':
                if (readNumbers(values, line + 1, 1) == 1) {
                    lineMessage(
                            gamma_free_fields(game, values[0])
                    );
                }
                else {
                    errorMessage();
                }
                free(line);
                break;

            case 'q':
                if (readNumbers(values, line + 1, 1) == 1) {
                    lineMessage(
                            gamma_golden_possible(game, values[0])
                    );
                }
                else {
                    errorMessage();
                }
                free(line);
                break;

            case 'p':
                if (readNumbers(values, line + 1, 0) == 0) {
//                    printf("%s",gamma_board(game));
//                    fflush(stdout);
                    textMessage(gamma_board(game));
                }
                else {
                    errorMessage();
                }
                free(line);
                break;

            case '/':
                exit(2137);

            case '#':
            case '\n':
                free(line);
                nextLine();
                break;

            case 4:
                break;

            default:
                free(line);
                errorMessage();
        }
    }

}