#include "gammaLib/gammaEngineLib.h"
#include "gamma.h"
//#include "findUnionLib/findUnion.h"
//#include "playerLib/player.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>


#define NUM_GOLDEN_MOVES 1

//struct gamma {
//    uint32_t width;
//    uint32_t height;
//    uint32_t players;
//    uint32_t areas;
//
//    uint32_t resetCounter;
//    uint32_t numGoldenMoves;
//    uint64_t available;
//    Member *members;
//    Node ***board;
//};

///** @brief Do quick check if golden move can be done.
// *
// * @param g - current game
// * @param player - attacking player
// * @param x - first coor
// * @param y
// * @return
// */
//static bool canGoldFastCheck(gamma_t *g, uint32_t player,
//                             uint32_t x, uint32_t y);

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


// -----------------------------------------------------------------------------------------

// TODO szybkie sprawdzenie czy atakujacy gracz (player) ma mniej pol lub sasiedzie?

//static bool canGoldFastCheck(gamma_t *g, uint32_t player,
//                             uint32_t x, uint32_t y) {
//
//    if (getPlayer(g, player)->areas == g->areas)
//        return numNeighbours(g, player, x, y) != 0;
//
//    return true;
//}

//static uint32_t

static void goldenMovePrep(gamma_t *g, Member attackedPlayer,
                           uint32_t x, uint32_t y) {
    attackedPlayer->roots =
            deleteNode(attackedPlayer->roots, find(g->board[x][y]));

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

    moveOnEmpty(g, 21321321, x, y, false);
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
    if (!positive(width) || !positive(height) || !positive(players))
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
    *game = (gamma_t) {width, height, players, areas, 0,
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
        insert(&getPlayer(g, player)->roots, find(g->board[x][y]));

    }
    else {
        takeField(g, player, x, y);
        getPlayer(g, player)->areas -= areasChange(g, player, x, y, true);
    }

    getPlayer(g, player)->surrounding += numEmpty(g, player, x, y);
    moveOnEmpty(g, player, x, y, true);
    return true;
}

bool gamma_golden_move(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
    if (wrongInput(g, player) || wrongCoordinates(g, x, y) ||
        isEmpty(g, x, y) || isMine(g, player, x, y) ||
        !hasGoldenMoves(g, player))
        return false;

    Member attackedPlayer = getPlayer(g, getOwner(g, x, y));



//    todo
//    if (!canGoldFastCheck(g, player, x, y)) {
//        return false;
//    }

    goldenMovePrep(g, attackedPlayer, x, y);

    return goldenMoveFinish(g, attackedPlayer, player, x, y);
}

uint64_t gamma_busy_fields(gamma_t *g, uint32_t player) {
    if (wrongInput(g, player))
        return 0;
    else
        return getPlayer(g, player)->owned;
}

uint64_t dfs(gamma_t *g, Node *elem, uint32_t id, bool state) {
    if (elem == NULL)
        return 0;

    uint64_t output = 0;
    // empty
    if (elem->added != state && elem->owner == 0) {
        elem->added = state;
        output++;
    }   // is mine
    else if (elem->added != state && elem->owner == id) {
        elem->added = state;
        output += dfs(g, getLeft(g, elem->x, elem->y), id, state);
        output += dfs(g, getUp(g, elem->x, elem->y), id, state);
        output += dfs(g, getRight(g, elem->x, elem->y), id, state);
        output += dfs(g, getDown(g, elem->x, elem->y), id, state);
    }

    return output;
}

uint64_t iterate(gamma_t *g, AvlTree tree, uint32_t id, bool state) {
    if (tree == NULL)
        return 0;

    uint64_t output = 0;


    output += dfs(g, tree->data, id, state);
    output += iterate(g, tree->left, id, state);
    output += iterate(g, tree->right, id, state);

    return output;

}

uint64_t gamma_free_fields(gamma_t *g, uint32_t player) {

    if (wrongInput(g, player))
        return 0;
    else if (getAreas(g, player) == g->areas) {

//        uint64_t output = iterate(g, getPlayer(g, player)->roots,
//                                  getPlayer(g, player)->id, true);
//        uint64_t test = iterate(g, getPlayer(g, player)->roots,
//                                getPlayer(g, player)->id, false);
//
//
//        assert(output == test);
//        return output;
        return getPlayer(g, player)->surrounding;
    }
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
//    return NULL;
    if (g == NULL)
        return NULL;

    uint32_t maxId = g->members[g->players - 1]->id;
    if (maxId >= 10) {
        printf("XDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDd\n");
        exit(2137);
    }

    char integer_string[32] = "";


    uint32_t line = g->width * 1;
    size_t length = 0;
    size_t maxLength = sizeof(char) * 1 * g->height *
                       (g->width + 1) + 1 * sizeof(char);
    char *output = (char *) malloc(maxLength);

    if (output == NULL) {
        return NULL;
    }

    output[maxLength - 1] = '\0';

    for (uint32_t y = g->height; y-- > 0;) { // to prevent uint32_t flip
        for (uint32_t x = 0; x < g->width; x++) {
            if (isEmpty(g, x, y)) {
                memcpy(output + length++, ".",1);
            }
            else {
                sprintf(integer_string, "%d", getOwner(g, x, y));
                memcpy(output + length++, integer_string, 1);
            }
        }
        memcpy(output + length++, "\n", 1);
    }

    return output;
}