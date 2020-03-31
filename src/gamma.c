#include "gamma.h"
#include "findUnionLib/findUnion.h"
#include "playerLib/player.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>



#define NUM_GOLDEN_MOVES 1

struct gamma {
    uint32_t width;
    uint32_t height;
    uint32_t players;
    uint32_t areas;

    uint32_t resetCounter;
    uint32_t numGoldenMoves;
    uint64_t available;
    Member *members;
    Node ***board;
};

static inline bool positive(uint32_t num) {
    return num > 0;
}

static void initMembers(Member*members, uint32_t players) {
    for (unsigned int i = 0; i < players; i++) {
        members[i] = newMember(i + 1);
    }
}

static void initBoard(Node ***board, uint32_t width, uint32_t height) {
    for (uint32_t i = 0; i < width; i++)
        for (uint32_t j = 0; j < height; j++) {
            board[i][j] =  newNode(0);
            if (board[i][j] == NULL)
                exit(1);
            // TODO poprawic
        }
}

static inline bool wrongInput(gamma_t *g, uint32_t player) {
    return g == NULL ||
           !positive(player) ||
           g->players < player;
}

static inline bool wrongCoordinates(gamma_t *g, uint32_t x, uint32_t y) {
    return g->width <= x || g->height <= y;
}

static inline bool isEmpty(gamma_t *g, uint32_t x, uint32_t y) {
    return g->board[x][y]->owner == 0;
}

static bool isMineNode(gamma_t *g, uint32_t player, Node *elem) {
    if (elem == NULL)
        return false;
    else
        return elem->owner == g->members[player - 1]->id;
}

static inline Member getPlayer(gamma_t *g, uint32_t player) {
    return g->members[player - 1];
}

static inline bool isMine (gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
    return g->board[x][y]->owner == getPlayer(g, player)->id;
}

static inline bool isEnemy(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
    return !isEmpty(g, x, y) && !isMine(g, player, x, y);
}

static inline uint32_t getAreas(gamma_t *g, uint32_t player) {
    return g->members[player - 1]->areas;
}

static void takeField(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
    g->available--;
    g->members[player - 1]->owned++;
    // g->board[x][y] = g->members[player - 1]->id;
    setData(g->board[x][y], getPlayer(g, player)->id);
}

static inline uint32_t getOwner(gamma_t *g, uint32_t x, uint32_t y) {
    return g->board[x][y]->owner;
}


// TODO przerobic te checki aby zwracaly Node* a isMine obudowac to
static inline Node *getDown(gamma_t *g, uint32_t x, uint32_t y) {
    return y == 0 ? NULL : g->board[x][y - 1];
}

static inline Node *getUp(gamma_t *g, uint32_t x, uint32_t y) {
    return y + 1 == g->height ? NULL : g->board[x][y + 1];
}

static inline Node *getLeft(gamma_t *g, uint32_t x, uint32_t y) {
    return x == 0 ? NULL : g->board[x - 1][y];
}

static inline Node *getRight(gamma_t *g,  uint32_t x, uint32_t y) {
    return x + 1 == g->width ? NULL : g->board[x + 1][y];
}


// num of friendly fields
static uint32_t numNeighbours(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
    uint32_t neighbours = 0;

    neighbours += isMineNode(g, player, getLeft(g, x, y));
    neighbours += isMineNode(g, player, getRight(g, x, y));
    neighbours += isMineNode(g, player, getUp(g, x, y));
    neighbours += isMineNode(g, player, getDown(g, x, y));


    return neighbours;
}


// jak zmieni sie ilosz obszarow gracza gdyby dolozyl pionek na x, y
// o ile spadnie
static uint32_t areasChange (gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
    // left up right down
    Node *arr[4];

    arr[0] = getLeft(g, x, y);
    arr[1] = getUp(g, x, y);
    arr[2] = getRight(g, x, y);
    arr[3] = getDown(g, x, y);

    uint32_t output = 0;
    uint32_t division = 1;

    for (int i = 0; i < 4; i++) {
        for (int j = i + 1; j < 4; j++) {
            if (sameRoot(arr[i], arr[j]))
                division = 2;
            output += arr[i] != NULL && arr[j] != NULL && !sameRoot(arr[i], arr[j]);
        }
    }

    return output / division;
}

//-----------------------------------------------------------------------------
// DONE
gamma_t *gamma_new(uint32_t width, uint32_t height,
                   uint32_t players, uint32_t areas) {
    if (!positive(width) || !positive(height) || !positive(players))
        return NULL;

    gamma_t *game = malloc(sizeof(gamma_t));
    Member *members = malloc(sizeof(member) * players);

    Node ***board = (Node ***) malloc(width * sizeof(Node **));
    if (board == NULL)
        return NULL;

    for (unsigned int i = 0; i < width; i++) {
        board[i] = (Node **) malloc(height * sizeof(Node *));
        if (board[i] == NULL)
            return NULL;
    }
    initBoard(board, width, height);

    if (game == NULL || members == NULL)
        return NULL;

    initMembers(members, players);
    *game = (gamma_t) {width, height, players, areas, 0,
                       NUM_GOLDEN_MOVES, width * height,
                       members, board};

    return game;
}

// DONE
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


// w ruchach ogolnie
// moga spadac posiadane obszary wiec chyba trzeba sprawdzac bfsem ile tego jest
// chyba tak ze z sasiadujacych zapuszczam bfsa i ile sie powtorzy

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
        addRoot(getPlayer(g, player), g->board[x][y]);//newNode(getPlayer(g, player)->id));
//        getPlayer(g, player)->areas++;

    }
    else {
        takeField(g, player, x, y);
        getPlayer(g, player)->areas -= areasChange(g, player, x, y);
        // TODO moze spadac liczba obszarow
    }

    return true;
}

bool gamma_golden_move(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
    if (wrongInput(g, player) || wrongCoordinates(g, x, y) ||
        isEmpty(g, x, y) || isMine(g, player, x, y))
        return false;
    // TODO jeszcze czy nie uzyl wczesniej golden move xd

    // TODO jeszcze moge powiekszyc obszarow gracza co traci poza limit areas


    if (numNeighbours(g, player, x, y) == 0) {
        if (getAreas(g, player) == g->areas)
            return false;

        g->available++;
        g->members[getOwner(g, x, y) - 1]->owned--;

        takeField(g, player, x, y);
        g->members[player - 1]->areas++;
        // TODO poprawic obszary gracza ktory traci to pole
    }
    else {
        g->available++;
        g->members[getOwner(g, x, y) - 1]->owned--;

        takeField(g, player, x, y);
    }

    return true;
}

// DONE
uint64_t gamma_busy_fields(gamma_t *g, uint32_t player) {
    if (wrongInput(g, player))
        return 0;
    else
        return g->members[player - 1]->owned;
}

// DONE
uint64_t gamma_free_fields(gamma_t *g, uint32_t player) {
    if (wrongInput(g, player))
        return 0;
    else if (g->members[player - 1]->areas == g->areas) {
        uint32_t output = 0;

        // TODO czy musze kwadratowo po calej planszy?
        for (uint32_t x = 0; x < g->width; x++)
            for (uint32_t y = 0; y < g->height; y++)
                output += isEmpty(g, x, y) && numNeighbours(g, player, x, y) != 0;

        return output;
    }
    else
        return g->available;

}

bool gamma_golden_possible(gamma_t *g, uint32_t player) {
    if (wrongInput(g, player))
        return false;

    if (getAreas(g, player) == g->areas) {
        for (uint32_t x = 0; x < g->width; x++)
            for (uint32_t y = 0; y < g->height; y++)
                if (numNeighbours(g, player, x, y) != 0 && isEnemy(g, player, x, y))
                    return true;
    }
    else {
        // enemy took anything
        for (uint32_t p = 0; p < g->players; p++)
            if (p != player - 1 && g->members[p]->owned > 0)
                return true;
    }

    return false;
}

char *gamma_board(gamma_t *g) {
    return NULL;
//    if (g == NULL)
//        return NULL;
//
//    // uint32_t maxId = g->members[g->players - 1]->id;
//    uint32_t line = g->width * 1;
//    char *output = (char *) malloc(sizeof(char) * g->height * g->width * 1);
//    if (output == NULL)
//        return NULL;
//
//    for (uint32_t y = 0; y < g->height; y++) {
//        for (uint32_t x = 0; x < g->width; x++) {
//            // itoa (i,buffer,10);
//            if (g->board[x][y] == 0) { // add '.'
//                strcat(output + line * y + x, ".");
//            }
//            else {
//                // na inta
//                char *helper = malloc(sizeof(char) * g->width * 1);
//                strcpy(helper, output + line * y);
//
//                sprintf(output + line * y, "%s%d", helper, g->board[x][y]);
//                free(helper);
////                strcat(output + line * y + x, str);
//            }
//        }
//        strcat(output + y * line + g->width, "\n");
//    }
//
//    return output;
}
