
#include <assert.h>
#include "gammaEngineLib.h"

inline Member getPlayer(gamma_t *g, uint32_t player);

inline bool positive(uint32_t num) {
    return num > 0;
}

void initMembers(Member *members, uint32_t players) {
    for (unsigned int i = 0; i < players; i++) {
        members[i] = newMember(i + 1);
    }
}

void initBoard(Node ***board, uint32_t width, uint32_t height) {
    for (uint32_t i = 0; i < width; i++)
        for (uint32_t j = 0; j < height; j++) {
            board[i][j] = newRoot(0, i, j);
            if (board[i][j] == NULL)
                exit(1);
        }
}

inline bool wrongInput(gamma_t *g, uint32_t player) {
    return g == NULL ||
           !positive(player) ||
           g->players < player;
}

inline bool wrongCoordinates(gamma_t *g, uint32_t x, uint32_t y) {
    return g->width <= x || g->height <= y;
}

inline bool isEmpty(gamma_t *g, uint32_t x, uint32_t y) {
    return g->board[x][y]->owner == 0;
}

inline bool hasGoldenMoves(gamma_t *g, uint32_t player) {
    return getPlayer(g, player)->goldenMoves < g->numGoldenMoves;
}

bool isMineNode(gamma_t *g, uint32_t player, Node *elem) {
    if (elem == NULL)
        return false;
    else
        return elem->owner == g->members[player - 1]->id;
}

inline Member getPlayer(gamma_t *g, uint32_t player) {
    return g->members[player - 1];
}

inline bool isMine(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
    return g->board[x][y]->owner == getPlayer(g, player)->id;
}

inline bool isEnemy(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
    return !isEmpty(g, x, y) && !isMine(g, player, x, y);
}

inline uint32_t getAreas(gamma_t *g, uint32_t player) {
    return g->members[player - 1]->areas;
}

inline AvlTree getPlayerRoots(gamma_t *g, uint32_t player) {
    return getPlayer(g, player)->roots;
}

void takeField(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
    g->available--;
    g->members[player - 1]->owned++;
    setData(g->board[x][y], getPlayer(g, player)->id);
}

inline uint32_t getOwner(gamma_t *g, uint32_t x, uint32_t y) {
    return g->board[x][y]->owner;
}


inline Node *getDown(gamma_t *g, uint32_t x, uint32_t y) {
    return y == 0 ? NULL : g->board[x][y - 1];
}

inline Node *getUp(gamma_t *g, uint32_t x, uint32_t y) {
    return y + 1 == g->height ? NULL : g->board[x][y + 1];
}

inline Node *getLeft(gamma_t *g, uint32_t x, uint32_t y) {
    return x == 0 ? NULL : g->board[x - 1][y];
}

inline Node *getRight(gamma_t *g, uint32_t x, uint32_t y) {
    return x + 1 == g->width ? NULL : g->board[x + 1][y];
}


void mergeFields(gamma_t *g, uint32_t player, Node *arr[], Node *biggest) {
    for (int i = 0; i < 4; i++) {
        if (arr[i] != NULL && isMineNode(g, player, arr[i]) &&
            find(arr[i]) != biggest) {
            getPlayer(g, player)->roots = deleteNode(getPlayerRoots(g, player),
                                                     find(arr[i]));
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
uint32_t numNeighbours(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
    uint32_t neighbours = 0;

    neighbours += isMineNode(g, player, getLeft(g, x, y));
    neighbours += isMineNode(g, player, getRight(g, x, y));
    neighbours += isMineNode(g, player, getUp(g, x, y));
    neighbours += isMineNode(g, player, getDown(g, x, y));


    return neighbours;
}


// jak zmieni sie ilosz obszarow gracza gdyby dolozyl pionek na x, y
// o ile spadnie
uint32_t areasChange(gamma_t *g, uint32_t player,
        uint32_t x, uint32_t y, bool merged) {
    // left up right down
    Node *arr[4];

    arr[0] = getLeft(g, x, y);
    arr[1] = getUp(g, x, y);
    arr[2] = getRight(g, x, y);
    arr[3] = getDown(g, x, y);

    uint32_t output = 0;

    Node *biggest = NULL;

    for (int i = 0; i < 4; i++) {
        if (arr[i] != NULL && !isAdded(find(arr[i])) &&
            isMineNode(g, player, arr[i])) {
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