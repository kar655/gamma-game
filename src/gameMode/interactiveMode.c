//
// Created by karol on 06/05/2020.
//

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <termio.h>
#include <assert.h>

#include "interactiveMode.h"
#include "../gamma.h"
#include "../inputParser/parser.h"

//#define BACKGROUND_WHITE "\x1b[7m"
//#define COLOR_RESET "\x1b[0m"

static inline uint32_t min(uint32_t a, uint32_t b) {
    return a <= b ? a : b;
}

static inline uint32_t max(uint32_t a, uint32_t b) {
    return a >= b ? a : b;
}


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
uint32_t posX;
uint32_t posY;


//static char *paintBoard(char *board) {
//    board = (char *) realloc(board, strlen(board) + 14);
//    int width = 1; // strlen gracza o najwiekszym id
//}

static void move(int num) {
    if (num == 0) // up
        posY = (posY + 1) == getHeight(game) ? posY : posY + 1;
    else if (num == 1) // down
        posY = posY == 0 ? 0 : posY - 1;
    else if (num == 2) // right
        posX = (posX + 1) == getWidth(game) ? posX : posX + 1;
    else if (num == 3) // left
        posX = posX == 0 ? 0 : posX - 1;
    else // todo
        assert(false);
}

// left - 27 91 68
// down - 27 91 66
// right - 27 91 quit (67)
// up - 27 91 65
static bool isArrowKey(char ch) {
    if ((unsigned int) ch != 27 || (unsigned int) getch() != 91)
        return false;

    return true;
}

static bool keepPlaying(int ch) {
    return ch != 'c' && ch != 'C'; // && ktokolwiek moze sie ruszyc
}

static void gameLoop();

bool initializeInteractive(uint32_t values[]) {
    game = gamma_new(values[0], values[1], values[2], values[3]);

    if (game == NULL)
        return false;

    posX = values[0] / 2;
    posY = values[1] / 2;

    okMessage();
    gameLoop();
    printf("QUIT\n");
}

//        clear
//        printf("\e[1;1H\e[2J");

static void gameLoop() {
    int id = 0;
    int ch = 1;
    char *line;
    char *playerInfo = nextPlayerInfo(game, &id);
    char *board;
    uint32_t values[4];

//    gamma_board(game);
//
//    printf("%s\n", nextPlayerInfo(game, &id));
    clear();
    printf("\e[?25l");  // no cursor

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
        board = paintBoard(game, posX, posY);
        if (board == NULL)
            return;
        printf("%s\n", board);
        free(board);

//        playerInfo = nextPlayerInfo(game, &id);
//        if (playerInfo == NULL)
//            return;
        printf("%s\n", playerInfo);


        printf("wczytalem znak %i\n", ch);
        printf("id = %d \t\t posX = %u \t\t posY %u\n", id, posX, posY);
        ch = getch();
//        ch = getchar();
        if (isArrowKey(ch)) {
            ch = getch();
            move((unsigned int) ch - 65);
            ch = 0;
        }
        else if (ch == 32) {
            if (gamma_move(game, id, posX, posY)) { // flip y
                free(playerInfo);
                playerInfo = nextPlayerInfo(game, &id);
            }
        }
        else if (ch == 71 || ch == 103) {
            if (gamma_golden_move(game, id, posX, posY)) {
                free(playerInfo);
                playerInfo =  nextPlayerInfo(game, &id);
            }
        }
        if (playerInfo == NULL) // no one can't move
            break;

        clear();
    } while (keepPlaying(ch));

    // enable cursor
    printf("\e[?25h");
    free(playerInfo);
//    while (true) {
//        giveLine(&line);
//
//        return;
//        switch (line[0]) {}
//
//    }
}