
#include "gammaEngineLib.h"

static void buildArea(gamma_t *g, Node *center, Node *elem, uint32_t id);

inline bool positive(uint32_t num) {
    return num > 0;
}

void initMembers(Member *members, uint32_t players) {
    for (unsigned int i = 0; i < players; i++) {
        members[i] = newMember(i + 1);
        if (members[i] == NULL)
            exit(1);
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

bool isEmptyNode(Node *elem) {
    if (elem == NULL)
        return false;
    else
        return elem->owner == 0;
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

inline Node *getField(gamma_t *g, uint32_t x, uint32_t y) {
    return g->board[x][y];
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
    getPlayer(g, player)->owned++;
    getField(g, x, y)->owner = getPlayer(g, player)->id;
}

void resetField(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
    g->available++;
    getPlayer(g, player)->owned--;
    getField(g, x, y)->owner = 0;
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

            getPlayer(g, player)->roots =
                    deleteNode(getPlayer(g, player)->roots, find(arr[i]));

            merge(biggest, arr[i]);
        }
    }
}

bool inArray(uint32_t player, uint32_t arr[]) {
    for (uint32_t i = 0; i < 4; i++)
        if (arr[i] == player)
            return true;

    return false;
}

//uint32_t correctSurrounding(gamma_t *g, Node *current, uint32_t player,
//                        uint32_t x, uint32_t y) {
//    if (isEmptyNode(current))
//        return numNeighbours(g, player, x, y) != 1;
//    else if (!inArray(getOwner(g, x - 1, y), arr)) {
//        getPlayer(g, getOwner(g, x - 1, y))->surrounding--;
//        arr[i++] = getOwner(g, x - 1, y);
//    }
//}

uint32_t moveOnEmpty(gamma_t *g, uint32_t player, uint32_t x, uint32_t y, bool sub) {

    uint32_t empty = 0;
    int change = sub ? -1 : 1;

    uint32_t arr[4] = {0};
    uint32_t i = 0;
//    uint32_t newX[4] = {x - 1, x, x + 1, x};
//    uint32_t newY[4] = {y, y + 1, y, y - 1};

    // musze zmniejszyc surrounding innnym gracza ktorzy sasiadowali z tym polem
    // ale pamietac ktorym zmniejszylem zeby kazdemu dokladnie raz

    Node *current = getLeft(g, x, y);
    if (current != NULL) {
//        if (isEmptyNode(current))
//            empty += numNeighbours(g, player, x - 1, y) == 1;
//        else
        if (!isEmptyNode(current) && !inArray(getOwner(g, x - 1, y), arr)) {
            getPlayer(g, getOwner(g, x - 1, y))->surrounding += change;
            arr[i++] = getOwner(g, x - 1, y);
        }
    }

    current = getUp(g, x, y);
    if (current != NULL) {
//        if (isEmptyNode(current))
//            empty += numNeighbours(g, player, x, y + 1) == 1;
//        else
        if (!isEmptyNode(current) && !inArray(getOwner(g, x, y + 1), arr)) {
            getPlayer(g, getOwner(g, x, y + 1))->surrounding += change;
            arr[i++] = getOwner(g, x, y + 1);
        }
    }

    current = getRight(g, x, y);
    if (current != NULL) {
//        if (isEmptyNode(current))
//            empty += numNeighbours(g, player, x + 1, y) == 1;
//        else
        if (!isEmptyNode(current) && !inArray(getOwner(g, x + 1, y), arr)) {
            getPlayer(g, getOwner(g, x + 1, y))->surrounding += change;
            arr[i++] = getOwner(g, x + 1, y);
        }
    }


    current = getDown(g, x, y);
    if (current != NULL) {
//        if (isEmptyNode(current))
//            empty += numNeighbours(g, player, x, y - 1) == 1;
//        else
        if (!isEmptyNode(current) && !inArray(getOwner(g, x, y - 1), arr)) {
            getPlayer(g, getOwner(g, x, y - 1))->surrounding += change;
            arr[i++] = getOwner(g, x, y - 1);
        }
    }


//    if (isEmptyNode(getLeft(g, x, y)))
//        empty += numNeighbours(g, player, x - 1, y) != 1;
//    if (isEmptyNode(getUp(g, x, y)))
//        empty += numNeighbours(g, player, x, y + 1) != 1;
//    if (isEmptyNode(getRight(g, x, y)))
//        empty += numNeighbours(g, player, x + 1, y) != 1;
//    if (isEmptyNode(getDown(g, x, y)))
//        empty += numNeighbours(g, player, x, y - 1) != 1;

    return empty;
}

uint32_t numEmpty(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
    uint32_t empty = 0;

    Node *current = getLeft(g, x, y);
    if (current != NULL) {
        if (isEmptyNode(current))
            empty += numNeighbours(g, player, x - 1, y) == 1;
    }

    current = getUp(g, x, y);
    if (current != NULL) {
        if (isEmptyNode(current))
            empty += numNeighbours(g, player, x, y + 1) == 1;
    }

    current = getRight(g, x, y);
    if (current != NULL) {
        if (isEmptyNode(current))
            empty += numNeighbours(g, player, x + 1, y) == 1;
    }


    current = getDown(g, x, y);
    if (current != NULL) {
        if (isEmptyNode(current))
            empty += numNeighbours(g, player, x, y - 1) == 1;
    }

    return empty;
}

uint32_t numNeighbours(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
    uint32_t neighbours = 0;

    neighbours += isMineNode(g, player, getLeft(g, x, y));
    neighbours += isMineNode(g, player, getRight(g, x, y));
    neighbours += isMineNode(g, player, getUp(g, x, y));
    neighbours += isMineNode(g, player, getDown(g, x, y));

    return neighbours;
}

void nearbyFields(gamma_t *g, Node *arr[], uint32_t x, uint32_t y) {
    arr[0] = getLeft(g, x, y);
    arr[1] = getUp(g, x, y);
    arr[2] = getRight(g, x, y);
    arr[3] = getDown(g, x, y);
}

void mergeWithMiddle(gamma_t *g, uint32_t player, Node *arr[],
                     uint32_t x, uint32_t y) {
    Node *biggest = NULL;

    for (int i = 0; i < 4; i++)
        if (arr[i] != NULL && isMineNode(g, player, arr[i])) {
            setRootAdded(arr[i], false);

            if (biggest == NULL || biggest->rank < find(arr[i])->rank) {
                biggest = find(arr[i]);
            }
        }


    mergeFields(g, player, arr, biggest);
    merge(biggest, getField(g, x, y));
}

void mergeNearbyFields(gamma_t *g, uint32_t player, Node *arr[]) {
    for (int i = 0; i < 4; i++)
        if (arr[i] != NULL && isMineNode(g, player, arr[i])) {
            setRootAdded(arr[i], false);
            insert(&getPlayer(g, player)->roots, find(arr[i]));
        }
}

// jak zmieni sie ilosz obszarow gracza gdyby dolozyl pionek na x, y
// o ile spadnie
uint32_t areasChange(gamma_t *g, uint32_t player,
                     uint32_t x, uint32_t y, bool middle) {

    uint32_t output = 0;
    Node *arr[4]; // nearby fields
    nearbyFields(g, arr, x, y);

    for (int i = 0; i < 4; i++) {
        if (arr[i] != NULL && !isRootAdded(arr[i]) &&
            isMineNode(g, player, arr[i])) {

            setRootAdded(arr[i], true);
            output++;
        }
    }

    if (middle)
        mergeWithMiddle(g, player, arr, x, y);
    else
        mergeNearbyFields(g, player, arr);

    return output - 1;
}

void clearRelations(gamma_t *g, Node *elem, uint32_t id) {
    if (elem == NULL)
        return;
    if (!elem->added && elem->owner == id) {
        clearNodeData(elem);
        elem->added = true;

        clearRelations(g, getLeft(g, elem->x, elem->y), id);
        clearRelations(g, getUp(g, elem->x, elem->y), id);
        clearRelations(g, getRight(g, elem->x, elem->y), id);
        clearRelations(g, getDown(g, elem->x, elem->y), id);
    }
}

// todo
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

void buildConnected(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
    buildArea(g, getLeft(g, x, y), getLeft(g, x, y), player);
    buildArea(g, getUp(g, x, y), getUp(g, x, y), player);
    buildArea(g, getRight(g, x, y), getRight(g, x, y), player);
    buildArea(g, getDown(g, x, y), getDown(g, x, y), player);
}