//
// Created by karol on 06/05/2020.
//

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <termio.h>

#include "interactiveMode.h"
#include "../gamma.h"
#include "../inputParser/parser.h"


char getch() {
    char buf = 0;
    struct termios old = {0};
    if (tcgetattr(0, &old) < 0)
        perror("tcsetattr()");
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if (tcsetattr(0, TCSANOW, &old) < 0)
        perror("tcsetattr ICANON");
    if (read(0, &buf, 1) < 0)
        perror("read()");
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if (tcsetattr(0, TCSADRAIN, &old) < 0)
        perror("tcsetattr ~ICANON");
    return (buf);
}

static inline void clear() {
    printf("\e[1;1H\e[2J");
}


gamma_t *game;


static bool keepPlaying(int ch) {
    return ch != 'c' && ch != 'C'; // && ktokolwiek moze sie ruszyc
}

static void gameLoop();

bool initializeInteractive(uint32_t values[]) {
    game = gamma_new(values[0], values[1], values[2], values[3]);

    if (game == NULL)
        return false;

    okMessage();
    gameLoop();
    printf("QUIT\n");
}

//        clear
//        printf("\e[1;1H\e[2J");

static void gameLoop() {
    char *line;
    char *board;
    uint32_t values[4];
    int id = 0;
    int ch = 1;

//    gamma_board(game);
//
//    printf("%s\n", nextPlayerInfo(game, &id));
    clear();

//    while (ch) {
//        board = gamma_board(game);
//        if (board == NULL)
//            return;
//        printf("%s\n", board);
//        free(board);
//        printf("%s\n", nextPlayerInfo(game, &id));
//        printf("wczytalem znak %i\n", ch);
//        ch = getch();
//        clear();
//    }

    do {
        board = gamma_board(game);
        if (board == NULL)
            return;
        printf("%s\n", board);
        free(board);
        printf("%s\n", nextPlayerInfo(game, &id));
        printf("wczytalem znak %i\n", ch);
//        ch = getch();
        ch = getchar();
        clear();
    } while (keepPlaying(ch));

//    while (true) {
//        giveLine(&line);
//
//        return;
//        switch (line[0]) {}
//
//    }
}