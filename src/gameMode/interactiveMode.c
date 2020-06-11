/** @file
 * Interactive mode implementation
 *
 * @author Karol Zagródka <karol.zagrodka@gmail.com>
 */

#include "interactiveMode.h"
#include "../gamma.h"
#include "../inputParser/parser.h"

#include <stddef.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>

#include <unistd.h>
#include <termio.h>


/** Sets background color to white */
#define BACKGROUND_WHITE printf("%s", "\x1b[7m")

/** Sets background color to default */
#define COLOR_RESET printf("%s", "\x1b[0m")


/**
 * Current gamma game
 */
gamma_t *game;

/** First coordinate of special field */
uint32_t posX;

/** Second coordinate of special field */
uint32_t posY;


/** @brief Moves special field.
 * Changes position of special field base on which arrow key was pressed
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

static uint32_t fieldLength(uint32_t players);

/** @brief Plays game in Interactive Mode
 * Read characters and prints results.
 */
static void gameLoop();

/** @brief Reads character from input.
 * COPIED FROM:
 * <https://stackoverflow.com/questions/421860/capture-characters-from-standard-input-without-waiting-for-enter-to-be-pressed>
 * @return read character
 */
static char getch() {
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

static void move(int num) {
    if (num == 65) // up
        posY = (posY + 1) == gamma_get_height(game) ? posY : posY + 1;
    else if (num == 66) // down
        posY = posY == 0 ? 0 : posY - 1;
    else if (num == 67) // right
        posX = (posX + 1) == gamma_get_width(game) ? posX : posX + 1;
    else if (num == 68) // left
        posX = posX == 0 ? 0 : posX - 1;
}

static uint32_t processChar(char ch, uint32_t id) {
    if (ch == 'c' || ch == 'C') {   // skip move
        id = gamma_next_player_id(game, id);
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
    else if (ch == 32) { // space
        if (gamma_move(game, id, posX, posY)) {
            id = gamma_next_player_id(game, id);
        }
    }
    else if (ch == 'g' || ch == 'G') {
        if (gamma_golden_move(game, id, posX, posY)) {
            id = gamma_next_player_id(game, id);
        }
    }
    else if (ch == 4) // EOF
        id = 0;

    return id;
}

static uint32_t fieldLength(uint32_t players) {
    char helper[32] = "";
    sprintf(helper, "%"PRIu32"", players);
    uint32_t numberLength = strlen(helper);

    // one extra free space
    if (numberLength > 1)
        numberLength++;

    return numberLength;
}

static void gameLoop() {
    uint32_t id = 1;
    uint32_t fl = gamma_field_length(game);

    gamma_print_player_info(game, id);

    // no cursor
    printf("\e[?25l");

    // cursor to beginning of current field
    printf("\e[%"PRIu32";%"PRIu32"H", gamma_get_height(game) - posY,
           fl * posX + 1);
    BACKGROUND_WHITE;
    textMessage(gamma_update_field(game, posX, posY));
    printf("\e[%"PRIu32";%"PRIu32"H", gamma_get_height(game) - posY,
           fl * posX + 1);

    while (id != 0) {

        fflush(stdout);
        uint32_t previousX = posX;
        uint32_t previousY = posY;
        id = processChar(getch(), id);

        // remove background color on previous field
        COLOR_RESET;
        textMessage(gamma_update_field(game, previousX, previousY));

        // cursor to beginning of current field
        printf("\e[%"PRIu32";%"PRIu32"H", gamma_get_height(game) - posY,
               fl * posX + 1);

        BACKGROUND_WHITE;
        textMessage(gamma_update_field(game, posX, posY));
        COLOR_RESET;

        // remove last line
        printf("\e[%"PRIu32";0H\e[2K", gamma_get_height(game) + 1);

        gamma_print_player_info(game, id);

        // cursor to beginning of current field
        printf("\e[%"PRIu32";%"PRIu32"H", gamma_get_height(game) - posY,
               fl * posX + 1);
    }

    COLOR_RESET;
    textMessage(gamma_update_field(game, posX, posY));

    // enable cursor
    printf("\e[?25h");

    // remove last line
    printf("\e[%"PRIu32";0H\e[2K", gamma_get_height(game) + 1);
    gamma_all_players_summary(game);
}

bool initializeInteractive(uint32_t values[]) {
    // get console width/height
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    // console is too small; one extra for player summary
    if (w.ws_col <= values[0] * fieldLength(values[2])
        || w.ws_row - 1 <= values[1]) {
        printf("Console is too small to hold game of this size\n");
        return false;
    }

    game = gamma_new(values[0], values[1], values[2], values[3]);
    char *board = gamma_board(game);

    if (game == NULL || board == NULL) {
        gamma_delete(game);
        return false;
    }

    posX = (values[0] - 1) / 2;
    posY = (values[1] - 1) / 2;

    okMessage();

    // one line down and clear
    printf("\eE\e[2J\e[1;1H");

    textMessage(board);

    gameLoop();

    gamma_delete(game);
    return true;
}
