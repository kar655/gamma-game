/** @file
 * Interactive mode implementation
 *
 * @author Karol Zagródka <karol.zagrodka@gmail.com>
 */

#include <stddef.h>
#include <stdio.h>

#include <unistd.h>
#include <termio.h>

#include "interactiveMode.h"
#include "../gamma.h"
#include "../inputParser/parser.h"

/**
 * Clears console.
 */
static void clear();

/** @brief Moves special field.
 * Changes positon of special field base on which arrow key was pressed
 * @param num - last character of arrow key pressed
 */
static void move(int num);

/** @brief Process pressed key.
 * Recognize arrow keys pressed or action keys
 * @param ch - pressed key
 * @param id - current player id.
 * @return id of player after pressed key or 0 if EOF or everyone can't move
 */
static uint32_t processChar(char ch, uint32_t id);

/** @brief Plays game in Interactive Mode
 * Read characters and prints results.
 */
static void gameLoop();


// TODO -- skopiowac link do stacka lub zaminic?
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

/**
 * Current gamma game
 */
gamma_t *game;

/** first coordinate of special field */
uint32_t posX;

/** second coordinate of special field */
uint32_t posY;


static void move(int num) {
    if (num == 65) // up
        posY = (posY + 1) == getHeight(game) ? posY : posY + 1;
    else if (num == 66) // down
        posY = posY == 0 ? 0 : posY - 1;
    else if (num == 67) // right
        posX = (posX + 1) == getWidth(game) ? posX : posX + 1;
    else if (num == 68) // left
        posX = posX == 0 ? 0 : posX - 1;
}

bool initializeInteractive(uint32_t values[]) {
    game = gamma_new(values[0], values[1], values[2], values[3]);

    if (game == NULL)
        return false;

    posX = (values[0] - 1) / 2;
    posY = (values[1] - 1) / 2;

    okMessage();
    gameLoop();

    gamma_delete(game);
    return true;
}

static uint32_t processChar(char ch, uint32_t id) {
    // todo albo po prostu return id jak nie strzalka
    // minusy dodatkowe generowanie gamma board
    if (ch == 'c' || ch == 'C') {   // skip move
        id = nextPlayerId(game, id);
    }
    else if (ch == 27) {    // can be arrow key
        ch = getch();
        if (ch == 91) {     // can be arrow key

            ch = getch();
            if (ch >= 65 && ch <= 68)
                move(ch);
            else
                id = processChar(ch, id);
        }
        else {
            id = processChar(ch, id);
        }
    }
    else if (ch == 32) {
        if (gamma_move(game, id, posX, posY)) {
            id = nextPlayerId(game, id);
        }
    }
    else if (ch == 71 || ch == 103) {
        if (gamma_golden_move(game, id, posX, posY)) {
            id = nextPlayerId(game, id);
        }
    }
    else if (ch == 4)
        id = 0;    // EOF

    return id;
}

static void gameLoop() {
    uint32_t id = 1;
    int ch;
    char *board;

    clear();
    printf("\e[?25l");  // no cursor

    while (id != 0) {
        board = paintBoard(game, posX, posY);
        if (board == NULL)
            return;
        textMessage(board);

        printPlayerInfo(game, id);

        ch = getch();
//        ch = getchar();
        id = processChar(ch, id);

        clear();
    }

    // enable cursor
    printf("\e[?25h");

    board = gamma_board(game);
    if (board == NULL)
        return;

    textMessage(board);
    allPlayersSummary(game);
}