/** @file
 * Implementation of gamma.h
 *
 * @author Karol Zagródka <karol.zagrodka@gmail.com>
 */

#include "gammaLib/gammaEngineLib.h"
#include "gamma.h"
#include <stdio.h>
#include <string.h>

/**
 * Maximal number of golden moves of one player
 */
#define NUM_GOLDEN_MOVES 1

/** @brief Free @p g, @p members, @p board.
 * @param g - current game
 * @param members - array of players
 * @param board - game board
 */
static void freeData(gamma_t *g, Member *members, Node ***board);

/** @brief Do quick check if golden move can be done.
 *  Checks if attacking player has maximal amount of areas and doesn't have
 *  friendly fields nearby.
 * @param g - current game
 * @param player - attacking player
 * @param x - first coordinate
 * @param y - second coordinate
 * @return true if golden move can be done, false when golden move can't be done
 */
static bool canGoldFastCheck(gamma_t *g, uint32_t player,
                             uint32_t x, uint32_t y);

/** @brief Prepares data for golden move.
 * Calls function to clear and prepare area for golden move
 * @param g - current game
 * @param attackedPlayer - Previous owner of field (@p x, @p y)
 * @param x - first coordinate
 * @param y - second coordinate
 */
static void goldenMovePrep(gamma_t *g, Member attackedPlayer,
                           uint32_t x, uint32_t y);

/** @brief Use gamma_move() on attacked field.
 * Checks number of areas and calls gamma_move()
 * @param g - current game
 * @param attackedPlayer - Previous owner of field (@p x, @p y)
 * @param player - id of attacking player
 * @param x - first coordinate
 * @param y - second coordinate
 * @return boolean if golden_move was performed
 */
static bool goldenMoveFinish(gamma_t *g, Member attackedPlayer, uint32_t player,
                             uint32_t x, uint32_t y);

// ----------------------------------------------------------------------------

static void freeData(gamma_t *g, Member *members, Node ***board) {
    free(g);
    free(members);
    free(board);
}

static bool canGoldFastCheck(gamma_t *g, uint32_t player,
                             uint32_t x, uint32_t y) {

    if (getPlayer(g, player)->areas == g->areas)
        return numNeighbours(g, player, x, y) != 0;

    return true;
}

static void goldenMovePrep(gamma_t *g, Member attackedPlayer,
                           uint32_t x, uint32_t y) {

    attackedPlayer->surrounding -= numEmpty(g, attackedPlayer->id, x, y);

    // removes all connections between attackedPlayer's fields
    clearRelations(g, g->board[x][y], attackedPlayer->id);

    // change owner to 0
    resetField(g, attackedPlayer->id, x, y);

    getField(g, x, y)->added = false;

    // builds attacked player's areas but not the middle
    buildConnected(g, attackedPlayer->id, x, y);

    attackedPlayer->areas +=
            areasChange(g, attackedPlayer->id, x, y, false);

    moveOnEmpty(g, x, y, false);
}

static bool goldenMoveFinish(gamma_t *g, Member attackedPlayer, uint32_t player,
                             uint32_t x, uint32_t y) {
    if (attackedPlayer->areas <= g->areas) {
        // try to move
        if (gamma_move(g, player, x, y)) {

            // made golden_move
            getPlayer(g, player)->goldenMoves++;
            return true;
        }
        else { // cant move
            // back to previous state
            gamma_move(g, attackedPlayer->id, x, y);
            return false;
        }
    }
    else { // cant do this golden_move

        gamma_move(g, attackedPlayer->id, x, y);
        return false;
    }
}

//-----------------------------------------------------------------------------

gamma_t *gamma_new(uint32_t width, uint32_t height,
                   uint32_t players, uint32_t areas) {
    if (!positive(width) || !positive(height) || !positive(players)
        || !positive(areas))
        return NULL;

    gamma_t *game = malloc(sizeof(gamma_t));
    Member *members = malloc(sizeof(member) * players);
    Node ***board = (Node ***) malloc(width * sizeof(Node **));

    if (game == NULL || members == NULL || board == NULL) {
        freeData(game, members, board);
        return NULL;
    }

    for (uint32_t i = 0; i < width; i++) {
        board[i] = (Node **) malloc(height * sizeof(Node *));
        if (board[i] == NULL) {
            for (uint32_t j = 0; j < i; j++) // clear previous
                free(board[j]);
            freeData(game, members, board);
            return NULL;
        }
    }

    if (!initBoard(board, width, height) || !initMembers(members, players)) {
        gamma_delete(game);
        return NULL;
    }

    *game = (gamma_t) {width, height, players, areas,
                       NUM_GOLDEN_MOVES, width * height,
                       members, board};

    return game;
}

void gamma_delete(gamma_t *g) {
    if (g == NULL)
        return;

    for (uint32_t x = 0; x < g->width; x++) {
        for (uint32_t y = 0; y < g->height; y++)
            free(g->board[x][y]);

        free(g->board[x]);
    }
    free(g->board);

    for (uint32_t i = 0; i < g->players; i++) {
        removeMember(getPlayer(g, i + 1));
    }
    free(g->members);

    free(g);
}

bool gamma_move(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
    if (wrongInput(g, player) ||
        wrongCoordinates(g, x, y) ||
        !isEmpty(g, x, y))
        return false;

    // new area
    if (numNeighbours(g, player, x, y) == 0) {
        if (getAreas(g, player) == g->areas) {
            return false;
        }

        takeField(g, player, x, y);
        getPlayer(g, player)->areas++;

    }
    else {
        takeField(g, player, x, y);
        getPlayer(g, player)->areas -= areasChange(g, player, x, y, true);
    }

    getPlayer(g, player)->surrounding += numEmpty(g, player, x, y);
    moveOnEmpty(g, x, y, true);
    return true;
}

bool gamma_golden_move(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
    if (wrongInput(g, player) || wrongCoordinates(g, x, y) ||
        isEmpty(g, x, y) || isMine(g, player, x, y) ||
        !hasGoldenMoves(g, player))
        return false;

    Member attackedPlayer = getPlayer(g, getOwner(g, x, y));

    if (!canGoldFastCheck(g, player, x, y))
        return false;

    goldenMovePrep(g, attackedPlayer, x, y);

    return goldenMoveFinish(g, attackedPlayer, player, x, y);
}

uint64_t gamma_busy_fields(gamma_t *g, uint32_t player) {
    if (wrongInput(g, player))
        return 0;
    else
        return getPlayer(g, player)->owned;
}

uint64_t gamma_free_fields(gamma_t *g, uint32_t player) {
    if (wrongInput(g, player))
        return 0;
    else if (getAreas(g, player) == g->areas)
        return getPlayer(g, player)->surrounding;
    else
        return g->available;
}

bool gamma_golden_possible(gamma_t *g, uint32_t player) {
    if (wrongInput(g, player) || !hasGoldenMoves(g, player))
        return false;

    return g->available +
           (uint64_t) getPlayer(g, player)->owned
           != (uint64_t) g->width * (uint64_t) g->height;
}

char *gamma_board(gamma_t *g) {
    if (g == NULL)
        return NULL;

    size_t maxLength = sizeof(char) * (g->height * (g->width + 1) + 1);
    char *output = (char *) malloc(maxLength);

    if (output == NULL) {
        return NULL;
    }

    size_t length = 0;
    char integerString[32] = "";

    for (uint32_t y = g->height; y-- > 0;) { // prevent uint32_t flip
        for (uint32_t x = 0; x < g->width; x++) {
            if (isEmpty(g, x, y)) {
                output[length++] = '.';
            }
            else {
                sprintf(integerString, "%d", getOwner(g, x, y));
                uint32_t len = strlen(integerString);

                if (len == 1)
                    output[length++] = integerString[0];
                else { // player with id > 9
                    maxLength += (1 + len) * sizeof(char);
                    output = (char *) realloc(output, maxLength);
                    if (output == NULL)
                        return NULL;
                    output[length++] = '[';
                    memcpy(output + length, integerString, len);
                    length += len;
                    output[length++] = ']';
                }
            }
        }
        output[length++] = '\n';
    }

    output[maxLength - 1] = '\0';
    return output;
}

// -1 wszyscy nie moga sie ruszyc
static int nextPlayerId(gamma_t *g, int last) {
    uint32_t temp;
    for (uint32_t p = last; p < last + g->players; p++) {
        temp = p % g->players + 1;

        if (gamma_free_fields(g, temp) > 0
            || gamma_golden_possible(g, temp)) {
            return temp;
        }
    }

//    for (int p = last % (int) g->players + 1; p != last;
//         p = p % (int) g->players + 1) {
//
//        if (gamma_free_fields(g, p) > 0
//            || gamma_golden_possible(g, p)) {
//            return p;
//        }
//    }

    return -1;
}

char *nextPlayerInfo(gamma_t *g, int *last) {
    *last = *last == 0 ? 1 : nextPlayerId(g, *last);

    if (*last == -1)
        return NULL;

    // 'PLAYER {id} {busy_fields} {free_fields} {Golden Possible / nic}\n\0'
    size_t length = 500;
    char *output = malloc(length);
    sprintf(output, "PLAYER %d %lu %lu %s", *last, gamma_busy_fields(g, *last),
            gamma_free_fields(g, *last), gamma_golden_possible(g, *last) ? "G" : "");

    return output;
}
// both length 4
#define BACKGROUND_WHITE "\x1b[7m"
#define COLOR_RESET "\x1b[0m"

char *paintBoard(gamma_t *g, uint32_t x, uint32_t y) {
    // char *output = gamma_board(g);
    // output = (char *) realloc(output, strlen(output) + 14);

    char *board = gamma_board(g);
    char *output = malloc(strlen(board) + 15);  // 2 * 7 + 1
    y = g->height - y - 1; // flip y

    // y * g->height + x
    // size_t len = x * g->width + y;
    size_t len = y * (g->height + 1) + x;
    memcpy(output, board, len);
//    printf("DEBUG INFO. output = %s    DLUGOSC: %lu\n\n", output, strlen(output));
    memcpy(output + len, BACKGROUND_WHITE, 4);
//    printf("DEBUG INFO. output = %s    DLUGOSC: %lu\n\n", output, strlen(output));

    // tutaj jakies get szerokosc najwieksza
    memcpy(output + len + 4, board + len, 1);
//    printf("DEBUG INFO. output = %s    DLUGOSC: %lu\n\n", output, strlen(output));

    memcpy(output + len + 4 + 1, COLOR_RESET, 4);
//    printf("DEBUG INFO. output = %s    DLUGOSC: %lu\n\n", output, strlen(output));
    memcpy(output + len + 4 + 1 + 4, board + len + 1, g->height * (g->width + 1) - len);
//    printf("DEBUG INFO. output = %s    DLUGOSC: %lu\n\n", output, strlen(output));

    free(board);
    return output;
}

inline uint32_t getWidth(gamma_t *g) {
    return g->width;
}

inline uint32_t getHeight(gamma_t *g) {
    return g->height;
}

char *playerInfo(gamma_t *g) {

}

char *playersSummary(gamma_t *g) {
    char *output = malloc(g->players * 11 + 1);
    output[g->players * 11] = '\0';
//    return NULL;
    size_t length = 0;
    size_t lineLenth = 0;
    size_t maxLength = 0; //g->players * 11 + 1;
    char idString[32] = "";
    char fieldsString[32] = "";


//    sprintf(integerString, "%d", getOwner(g, x, y));


    // "PLAYER {id} {ownedFields}"
    // 6 + 1 + id + 1 + ownedfield + 1
    // na koncu + 1 za \o
    for (uint32_t id = 1; id <= g->players; id++) {
        sprintf(idString, "%u", id);
        sprintf(fieldsString, "%lu", gamma_busy_fields(g, id));



        memcpy(output + length, "PLAYER ", 7);
        length += 7;
        memcpy(output + length, idString, 1);
        length += strlen(idString);
        output[length++] = ' ';
        memcpy(output + length, fieldsString, 1);
        length += strlen(fieldsString);
        output[length++] = '\n';
    }

    return output;
}