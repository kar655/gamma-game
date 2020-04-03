#include "gamma.h"
#include "findUnionLib/findUnion.h"
#include "playerLib/player.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>


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

void DATACHECK(gamma_t *g);
bool special = false;

static inline bool positive(uint32_t num) {
    return num > 0;
}

static void initMembers(Member *members, uint32_t players) {
    for (unsigned int i = 0; i < players; i++) {
        members[i] = newMember(i + 1);
    }
}

static void initBoard(Node ***board, uint32_t width, uint32_t height) {
    for (uint32_t i = 0; i < width; i++)
        for (uint32_t j = 0; j < height; j++) {
            board[i][j] = newRoot(0, i, j);
            if (board[i][j] == NULL)
                exit(1);
        }
}

static inline Member getPlayer(gamma_t *g, uint32_t player);

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

static inline bool hasGoldenMoves(gamma_t *g, uint32_t player) {
    return getPlayer(g, player)->goldenMoves < g->numGoldenMoves;
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

static inline bool isMine(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
    return g->board[x][y]->owner == getPlayer(g, player)->id;
}

static inline bool isEnemy(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
    return !isEmpty(g, x, y) && !isMine(g, player, x, y);
}

static inline uint32_t getAreas(gamma_t *g, uint32_t player) {
    return g->members[player - 1]->areas;
}

static inline AvlTree getPlayerRoots(gamma_t *g, uint32_t player) {
    return getPlayer(g, player)->roots;
}

static void takeField(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
    g->available--;
    g->members[player - 1]->owned++;
    setData(g->board[x][y], getPlayer(g, player)->id);
}

static inline uint32_t getOwner(gamma_t *g, uint32_t x, uint32_t y) {
    return g->board[x][y]->owner;
}


static inline Node *getDown(gamma_t *g, uint32_t x, uint32_t y) {
    return y == 0 ? NULL : g->board[x][y - 1];
}

static inline Node *getUp(gamma_t *g, uint32_t x, uint32_t y) {
    return y + 1 == g->height ? NULL : g->board[x][y + 1];
}

static inline Node *getLeft(gamma_t *g, uint32_t x, uint32_t y) {
    return x == 0 ? NULL : g->board[x - 1][y];
}

static inline Node *getRight(gamma_t *g, uint32_t x, uint32_t y) {
    return x + 1 == g->width ? NULL : g->board[x + 1][y];
}


static void mergeFields(gamma_t *g, uint32_t player, Node *arr[], Node *biggest) {
    for (int i = 0; i < 4; i++) {
        if (arr[i] != NULL && isMineNode(g, player, arr[i]) && find(arr[i]) != biggest) {
            getPlayer(g, player)->roots = deleteNode(getPlayerRoots(g, player), find(arr[i]));
            // todo arr[i] czy find(arr[i])
            merge(biggest, arr[i]);
        }
    }

//    for (int i = 0; i < 4; i++) {
//        if (arr[i] != NULL && isMineNode(g, player, arr[i])) {
//            merge(center, arr[i]);
//        }
//    }
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
static uint32_t areasChange(gamma_t *g, uint32_t player, uint32_t x, uint32_t y, bool merged) {
    // left up right down
    Node *arr[4];

    arr[0] = getLeft(g, x, y);
    arr[1] = getUp(g, x, y);
    arr[2] = getRight(g, x, y);
    arr[3] = getDown(g, x, y);

    uint32_t output = 0;

    Node *biggest = NULL;

    for (int i = 0; i < 4; i++) {
        if (arr[i] != NULL && !isAdded(find(arr[i])) && isMineNode(g, player, arr[i])) {
            setAdded(find(arr[i]), true);
            output++;

            if (biggest == NULL || biggest->rank < find(arr[i])->rank) {
                biggest = find(arr[i]);
            }
        }
    }

    // todo tu setAdded powinno byc od ROOTa
    // bo interesuje mnie ze caly obszar dodany a nie pojedyncze pole

    // TODO ale tutaj nie wiem bo nie pamietac co to mialo robic
    // albo juz jest OK XD
    for (int i = 0; i < 4; i++)
        if (arr[i] != NULL && isMineNode(g, player, arr[i])) {
            setAdded(find(arr[i]), false);
            if (!merged)
                insert(&getPlayer(g, player)->roots, find(arr[i]));
        }


    if (merged) {
        // insert(&getPlayer(g, player)->roots, find(g->board[x][y]));
        mergeFields(g, player, arr, biggest);
        merge(biggest, g->board[x][y]);
    }
//    else {
//        insert(&getPlayer(g, player)->roots, find());
//        insert(&getPlayer(g, player)->roots, find(g->board[x][y]));
//        insert(&getPlayer(g, player)->roots, find(g->board[x][y]));
//        insert(&getPlayer(g, player)->roots, find(g->board[x][y]));
//    }


    assert(output <= 4);
    return output - 1;
}

void clearRelations(gamma_t *g, Node *elem, uint32_t id) {
    if (elem == NULL)
        return;
    if (!elem->added && elem->owner == id) {
        elem->parent = elem;
        elem->rank = 0;
        elem->added = true;

        clearRelations(g, getLeft(g, elem->x, elem->y), id);
        clearRelations(g, getUp(g, elem->x, elem->y), id);
        clearRelations(g, getRight(g, elem->x, elem->y), id);
        clearRelations(g, getDown(g, elem->x, elem->y), id);
    }
}

void buildArea(gamma_t *g, Node *center, Node *elem, uint32_t id) {
    if (elem == NULL)
        return;
    // was deleted before or connects to other area
    if ((elem->added || !sameRoot(center, elem)) && elem->owner == id) {
        merge(center, elem);
        elem->added = false;

        buildArea(g, center, getLeft(g, elem->x, elem->y), id);
        buildArea(g, center, getUp(g, elem->x, elem->y), id);
        buildArea(g, center, getRight(g, elem->x, elem->y), id);
        buildArea(g, center, getDown(g, elem->x, elem->y), id);
    }
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


bool gamma_move(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
    if (wrongInput(g, player) ||
        wrongCoordinates(g, x, y) ||
        !isEmpty(g, x, y))
        return false;

    if (!special) {
        DATACHECK(g); // 999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999
    }
    else {
        special = false;
    }
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

    return true;
}

bool gamma_golden_move(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
    if (wrongInput(g, player) || wrongCoordinates(g, x, y) ||
        isEmpty(g, x, y) || isMine(g, player, x, y) ||
        !hasGoldenMoves(g, player))
        return false;

    // TODO check czy nie rozspojnie gracza kotremu zabieram
    // i tak musze zburzyc i tak???
    DATACHECK(g); // 999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999
    Member attackedPlayer = getPlayer(g, getOwner(g, x, y));
    attackedPlayer->roots = deleteNode(attackedPlayer->roots, find(g->board[x][y]));
    // removes all connections between attackedPlayer's fields
    clearRelations(g, g->board[x][y], attackedPlayer->id);
    // tymczasowo zamieniam wlasciciela
    g->available++;
    attackedPlayer->owned--;
    setData(g->board[x][y], 0);
    g->board[x][y]->added = false;

    // todo cos chyba bedzie nie tak z drzewem avl albo i nie
    /// bo dodaje juz zmergowane czyi tak naprawde x i y beda pochodzic od roota

    // jakies budowanie na nowo polaczen po 4 stronach
    buildArea(g, getLeft(g, x, y), getLeft(g, x, y), attackedPlayer->id);
    buildArea(g, getUp(g, x, y), getUp(g, x, y), attackedPlayer->id);
    buildArea(g, getRight(g, x, y), getRight(g, x, y), attackedPlayer->id);
    buildArea(g, getDown(g, x, y), getDown(g, x, y), attackedPlayer->id);

    // tylko to zle ze merguje juz
    uint32_t newAreas = areasChange(g, attackedPlayer->id, x, y, false);

    // policzyc czy obszary beda sie zgadzac
    // jesli tak to te 4 dodac do drzewa gracza i zwykly move
    // jesli nie to dodac to pole "centralne"
    // i mergeFields?

    // TODO HERE !!!!!!!!!!!!!!!!!!!!!!!!!
    special = true;
    attackedPlayer->areas += newAreas;
    if (attackedPlayer->areas <= g->areas) {
        // normalne przejecie chyba moze nawet na chama z gamma_move
        // dla gracza player
        if (gamma_move(g, player, x, y)) {
            // gracz przejal pole normalnie
            getPlayer(g, player)->goldenMoves++;
            return true;
        }
        else {
            // gracz nie moze przejac pola
            // powrocic do stanu sprzed ataku
            // usunac lewo prawo gora dol

            gamma_move(g, attackedPlayer->id, x, y);
//            attackedPlayer->areas -= newAreas - 1;

            //
            return false;
        }
    }
    else { // cant do this golden_move
        // tutaj tez normalne gamma_move ??
        // tylko dla gracza attackedPlayer

        gamma_move(g, attackedPlayer->id, x, y);
//        attackedPlayer->areas -= newAreas - 1;

        return false;
    }

    // areas of the previous owner will decrease by 1
//    if (numNeighbours(g, getPlayer(g, x, y), x, y) == 0) {
//        //todo usunac z tablicy rootow
//    }
//    else { //
//
//    }

//    if (numNeighbours(g, player, x, y) == 0) {
//        if (getAreas(g, player) == g->areas)
//            return false;
//
//        g->available++;
//        g->members[getOwner(g, x, y) - 1]->owned--;
//
//        takeField(g, player, x, y);
//        g->members[player - 1]->areas++;
//        // TODO poprawic obszary gracza ktory traci to pole
//    }
//    else {
//        g->available++;
//        getPlayer(g, getOwner(g, x, y))->owned--;
//
//        takeField(g, player, x, y);
//    }
//
//    return true;
}

uint64_t gamma_busy_fields(gamma_t *g, uint32_t player) {
    DATACHECK(g); // 999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999
    if (wrongInput(g, player))
        return 0;
    else
        return getPlayer(g, player)->owned;
}

// todo utworzyc jakies gamma_helper_lib

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

// DONE
uint64_t gamma_free_fields(gamma_t *g, uint32_t player) {
    DATACHECK(g); // 999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999

    if (wrongInput(g, player))
        return 0;
    else if (getAreas(g, player) == g->areas) {
//        uint32_t output = 0;
//
////        // TODO czy musze kwadratowo po calej planszy?
//        for (uint32_t x = 0; x < g->width; x++)
//            for (uint32_t y = 0; y < g->height; y++)
//                output += isEmpty(g, x, y) && numNeighbours(g, player, x, y) != 0;

        uint64_t output = iterate(g, getPlayer(g, player)->roots, getPlayer(g, player)->id, true);
        uint64_t test = iterate(g, getPlayer(g, player)->roots, getPlayer(g, player)->id, false);

//        printf("Output: %lu\n", output);
//        printf("Test: %lu\n", test);

        return output;
    }
    else
        return g->available;

}

bool gamma_golden_possible(gamma_t *g, uint32_t player) {
    DATACHECK(g); // 999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999
    return false;
    if (wrongInput(g, player) || !hasGoldenMoves(g, player))
        return false;

//    if (getAreas(g, player) == g->areas) {
//
//        for (uint32_t x = 0; x < g->width; x++)
//            for (uint32_t y = 0; y < g->height; y++)
//                if (numNeighbours(g, player, x, y) != 0 && isEnemy(g, player, x, y))
//                    return true;
//    }
//    else {
    return g->available +
           (uint64_t) getPlayer(g, player)->owned
           != (uint64_t) g->width * (uint64_t) g->height;
//    }
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

void checkBoard(gamma_t *g) {
    for (uint32_t x = 0; x < g->width; x++) {
        for (uint32_t y = 0; y < g->height; y++) {
            // Node *
            assert(g->board[x][y] != NULL);
            assert(g->board[x][y]->x == x);
            assert(g->board[x][y]->y == y);
            assert(g->board[x][y]->parent != NULL);
            assert(g->board[x][y]->added == false);
            assert(g->board[x][y]->owner <= g->players);
            assert(g->board[x][y]->parent->owner == g->board[x][y]->owner);

            if (g->board[x][y]->owner == 0) {
                assert(g->board[x][y]->parent == g->board[x][y]);
            }
            else {
                if (g->board[x][y]->parent == g->board[x][y]) {
                    assert(exists(getPlayer(g, getOwner(g, x, y))->roots, g->board[x][y]));
                }
                else {
                    assert(!exists(getPlayer(g, getOwner(g, x, y))->roots, g->board[x][y]));
                    assert(exists(getPlayer(g, getOwner(g, x, y))->roots, find(g->board[x][y])));
                }

            }
        }
    }
}

uint32_t checkTree(AvlTree tree) {
    if (tree == NULL)
        return 0;

    assert(tree->data != NULL);
    assert(tree->data->parent == tree->data);
    return 1 + checkTree(tree->left) +
           checkTree(tree->right);
//    assert()
}

void checkPlayers(gamma_t *g) {
    for (uint32_t i = 1; i <= g->players; i++) {
        assert(getPlayer(g, i) != NULL);
        assert(getPlayer(g, i)->id == i);
        assert(getPlayer(g, i)->areas <= g->areas);
        assert(getPlayer(g, i)->goldenMoves <= 1);
        assert(checkTree(getPlayer(g, i)->roots) == getAreas(g, i));
    }
}

void DATACHECK(gamma_t *g) {
//    checkBoard(g);
//    checkPlayers(g);
}

/// TODO
/// sciana placzu

// blad w 67 standard