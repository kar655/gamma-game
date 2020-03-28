#include "gamma.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


struct member {
    uint32_t id;
    uint32_t areas;
    uint32_t fields;

    uint64_t owned;
    uint32_t goldenMoves;
};

typedef struct member member;
#define Member member*

struct gamma {
    uint32_t width;
    uint32_t height;
    uint32_t players;
    uint32_t areas;

    uint32_t numGoldenMoves;
    uint64_t available;
    Member *members;
    uint32_t **board;
};

static inline bool positive(uint32_t num) {
    return num > 0;
}

static void initMembers(Member*members, uint32_t players) {
    for (unsigned int i = 0; i < players; i++) {
        members[i] = malloc(sizeof(member));
        *members[i] = (member) {i + 1, 0, 0, 0, 0};
    }
}

static void initBoard(uint32_t **board, uint32_t width, uint32_t height) {
    for (uint32_t i = 0; i < width; i++)
        for (uint32_t j = 0; j < height; j++)
            board[i][j] = 0;
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
    return g->board[x][y] == 0;
}

static inline bool isMine(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
    return g->board[x][y] == g->members[player - 1]->id;
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
    g->board[x][y] = g->members[player - 1]->id;
}

static inline uint32_t getOwner(gamma_t *g, uint32_t x, uint32_t y) {
    return g->board[x][y];
}

static inline bool checkDown(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
    return y == 0 ? 0 : isMine(g, player, x, y - 1);
}

static inline bool checkUp(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
    return y + 1 == g->height ? 0 : isMine(g, player, x, y + 1);
}

static inline bool checkLeft(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
    return x == 0 ? false : isMine(g, player, x - 1, y);
}

static inline bool checkRight(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
    return x + 1 == g->width ? 0 : isMine(g, player, x + 1, y);
}


// num of friendly fields
static uint32_t numNeighbours(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
    uint32_t neighbours = 0;

    neighbours += checkLeft(g, player, x, y);
    neighbours += checkRight(g, player, x, y);
    neighbours += checkUp(g, player, x, y);
    neighbours += checkDown(g, player, x, y);


//    // a co jak 1 x 1 czy 2 x 2
//    if (x == 0) {
//        if (y == 0) { // upper-left
//            neighbours += isMine(g, player, x + 1, y);
//            neighbours += isMine(g, player, x, y + 1);
//        }
//        else if (y + 1 == g->height) { // bottom-left
//            neighbours += isMine(g, player, x, y - 1);
//            neighbours += isMine(g, player, x + 1, y);
//        }
//        else { // middle-left
//            neighbours += isMine(g, player, x, y - 1);
//            neighbours += isMine(g, player, x + 1, y);
//            neighbours += isMine(g, player, x, y + 1);
//        }
//    }
//    else if (x + 1 == g->width) {
//        if (y == 0) { // upper-right
//            neighbours += isMine(g, player, x, y + 1);
//            neighbours += isMine(g, player, x - 1, y);
//        }
//        else if (y + 1 == g->height) { // bottom-right
//            neighbours += isMine(g, player, x, y - 1);
//            neighbours += isMine(g, player, x - 1, y);
//        }
//        else { // middle-right
//            neighbours += isMine(g, player, x, y + 1);
//            neighbours += isMine(g, player, x, y - 1);
//            neighbours += isMine(g, player, x - 1, y);
//        }
//    }
//    else {
//        if (y == 0) { // upper-mid
//            neighbours += isMine(g, player, x - 1, y);
//            neighbours += isMine(g, player, x + 1, y);
//            neighbours += isMine(g, player, x, y + 1);
//        }
//        else if (y + 1 == g->height) { // bottom-mid
//            neighbours += isMine(g, player, x + 1, y);
//            neighbours += isMine(g, player, x - 1, y);
//            neighbours += isMine(g, player, x, y - 1);
//        }
//        else {  // middle-mid
//            neighbours += isMine(g, player, x, y + 1);
//            neighbours += isMine(g, player, x, y - 1);
//            neighbours += isMine(g, player, x + 1, y);
//            neighbours += isMine(g, player, x - 1, y);
//        }
//    }

    return neighbours;
}

// jakas taka ze
// 2 graczy i szuka pola 1 gracza ktore sasiaduja 2
// np jak gracz == 0 to po pustych
// lewo gora prawo dol

// liczba neigh >= 2
// TODO jakas taka funkcja ze dostaje polozenie i odpala bfsa
// of moich sasiadow tego pola i czy/ile sie polaczy
// bo wtedy oblicze ile stracilem / zyskalem obszarow (areas)
//
// odpala bfsa po lewej jesli znalazl ktoregos to juz z niego nie odpala i tak po kolei

//-----------------------------------------------------------------------------
// DONE
gamma_t *gamma_new(uint32_t width, uint32_t height,
                   uint32_t players, uint32_t areas) {
    if (!positive(width) || !positive(height) || !positive(players))
        return NULL;

    gamma_t *game = malloc(sizeof(gamma_t));
    Member*members = malloc(sizeof(member) * players);

    uint32_t **board = (uint32_t **) malloc(width * sizeof(uint32_t *));
    if (board == NULL)
        return NULL;

    for (unsigned int i = 0; i < width; i++) {
        board[i] = (uint32_t *) malloc(height * sizeof(uint32_t));
        if (board[i] == NULL)
            return NULL;
    }
    initBoard(board, width, height);

    if (game == NULL || members == NULL)
        return NULL;

    initMembers(members, players);
    *game = (gamma_t) {width, height, players, areas, 1,
                       width * height, members, board};

    return game;
}

// DONE
void gamma_delete(gamma_t *g) {
    if (g == NULL)
        return;

    for (uint32_t i = 0; i < g->players; i++)
        free(g->members[i]);
    free(g->members);
    for (uint32_t x = 0; x < g->width; x++)
        free(g->board[x]);
    free(g->board);
    free(g);
}


// w ruchach ogolnie
// moga spadac posiadane obszary wiec chyba trzeba sprawdzac bfsem ile tego jest
// chyba tak ze z sasiadujacych zapuszczam bfsa i ile sie powtorzy

// DONE
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
        g->members[player - 1]->areas++;
    }
    else {
        takeField(g, player, x, y);
        // TODO moze spadac liczba obszarow
    }

    return true;
}

bool gamma_golden_move(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
    if (wrongInput(g, player) || wrongCoordinates(g, x, y) ||
        isEmpty(g, x, y) || isMine(g, player, x, y))
        return false;

    // TODO jeszcze moge powiekszyc obszarow gracza co traci poza limit areas


    if (numNeighbours(g, player, x ,y) == 0) {
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
    if (g == NULL)
        return NULL;

    // uint32_t maxId = g->members[g->players - 1]->id;
    uint32_t line = g->width * 1;
    char *output = (char *) malloc(sizeof(char) * g->height * g->width * 1);
    if (output == NULL)
        return NULL;

    for (uint32_t y = 0; y < g->height; y++) {
        for (uint32_t x = 0; x < g->width; x++) {
            // itoa (i,buffer,10);
            if (g->board[x][y] == 0) { // add '.'
                strcat(output + line * y + x, ".");
            }
            else {
                // na inta
                char *helper = malloc(sizeof(char) * g->width * 1);
                strcpy(helper, output + line * y);

                sprintf(output + line * y, "%s%d", helper, g->board[x][y]);
                free(helper);
//                strcat(output + line * y + x, str);
            }
        }
        strcat(output + y * line + g->width, "\n");
    }

    return output;
}
