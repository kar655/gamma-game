/** @file
 * Implementation of gamma.h
 *
 * @author Karol Zagródka <karol.zagrodka@gmail.com>
 */

#include "gammaLib/gammaEngineLib.h"
#include "gamma.h"
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

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

    // No other player took any field
    if (g->available + (uint64_t) getPlayer(g, player)->owned
        == (uint64_t) g->width * (uint64_t) g->height)
        return false;

    if (getAreas(g, player) < g->areas)
        return true;

    uint32_t goldenUsed = getPlayer(g, player)->goldenMoves;

    for (uint32_t x = 0; x < g->width; x++) {
        for (uint32_t y = 0; y < g->height; y++) {
            if (!isEmpty(g, x, y) && !isMine(g, player, x, y)
                && numNeighbours(g, player, x, y) > 0) {

                getPlayer(g, player)->goldenMoves = 0;

                Member attackedPlayer = getPlayer(g, getOwner(g, x, y));
                uint32_t previousOwner = attackedPlayer->id;
                uint32_t previousOwnerGolden = attackedPlayer->goldenMoves;
                attackedPlayer->goldenMoves = 0;

                if (gamma_golden_move(g, player, x, y)) {
                    gamma_golden_move(g, previousOwner, x, y);
                    getPlayer(g, player)->goldenMoves = goldenUsed;
                    attackedPlayer->goldenMoves = previousOwnerGolden;

                    return true;
                }
                else {
                    attackedPlayer->goldenMoves = previousOwnerGolden;
                }
            }
        }
    }

    return false;
}

char *gamma_board(gamma_t *g) {
    if (g == NULL)
        return NULL;

    uint32_t numberLength = gamma_field_length(g);

    uint64_t maxLength =
            sizeof(char) *
            ((uint64_t) g->height * ((uint64_t) g->width * numberLength + 1) + 1);
    char *output = (char *) malloc(maxLength);

    if (output == NULL) {
        return NULL;
    }

    size_t length = 0;
    char integerString[32] = "";

    for (uint32_t y = g->height; y-- > 0;) { // prevent uint32_t flip
        for (uint32_t x = 0; x < g->width; x++) {
            if (isEmpty(g, x, y)) {
                for (uint32_t i = 0; i < numberLength - 1; i++)
                    output[length++] = ' ';

                output[length++] = '.';
            }
            else {
                sprintf(integerString, "%"PRIu32"", getOwner(g, x, y));
                uint32_t len = strlen(integerString);

                for (uint32_t i = 0; i < numberLength - len; i++)
                    output[length++] = ' ';

                memcpy(output + length, integerString, len);
                length += len;
            }
        }
        output[length++] = '\n';
    }

    output[maxLength - 1] = '\0';
    return output;
}

uint32_t gamma_next_player_id(gamma_t *g, uint32_t last) {
    uint32_t temp;

    for (uint32_t p = last; p < last + g->players; p++) {
        temp = p % g->players + 1;

        if (gamma_free_fields(g, temp) > 0
            || gamma_golden_possible(g, temp)) {
            return temp;
        }
    }

    return 0;
}

void gamma_print_player_info(gamma_t *g, uint32_t id) {
    printf("PLAYER %"PRIu32"\n"
           "Owns %"PRIu64" fields\n"
           "Can take %"PRIu64" new fields\n"
           "%s\n",
           id, gamma_busy_fields(g, id), gamma_free_fields(g, id),
           gamma_golden_possible(g, id) ?
           "Can use golden move" : "Can't use golden move");
}

char *gamma_update_field(gamma_t *g, uint32_t x, uint32_t y) {
    uint32_t length = 0;
    char integerString[32] = "";

    uint32_t fl = gamma_field_length(g);
    char *output = malloc(fl + 1);

    if (isEmpty(g, x, y)) {
        for (uint32_t i = 0; i < fl - 1; i++)
            output[length++] = ' ';

        output[length] = '.';
    }
    else {
        sprintf(integerString, "%"PRIu32"", getOwner(g, x, y));
        uint32_t len = strlen(integerString);

        for (uint32_t i = 0; i < fl - len; i++)
            output[length++] = ' ';

        memcpy(output + length, integerString, len);
    }

    output[fl] = '\0';
    return output;
}

inline uint32_t gamma_get_width(gamma_t *g) {
    return g->width;
}

inline uint32_t gamma_get_height(gamma_t *g) {
    return g->height;
}

void gamma_all_players_summary(gamma_t *g) {
    for (uint32_t id = 1; id <= g->players; id++) {
        printf("PLAYER %"PRIu32" owns %"PRIu64" fields\n", id,
               gamma_busy_fields(g, id));
    }
}

uint32_t gamma_field_length(gamma_t *g) {
    char helper[32] = "";
    sprintf(helper, "%"PRIu32"", g->players);
    uint32_t numberLength = strlen(helper);

    // one extra free space
    if (numberLength > 1)
        numberLength++;

    return numberLength;
}
