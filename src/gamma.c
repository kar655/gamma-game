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

    if (game == NULL || members == NULL || board == NULL)
        return NULL;

    for (unsigned int i = 0; i < width; i++) {
        board[i] = (Node **) malloc(height * sizeof(Node *));
        if (board[i] == NULL)
            return NULL;
    }

    initBoard(board, width, height);

    initMembers(members, players);
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
                else {
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
