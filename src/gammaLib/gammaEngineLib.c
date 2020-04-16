/** @file
 * Implementation of helper functions to gamma functions.
 *
 * @author Karol Zagródka <karol.zagrodka@gmail.com>
 */

#include "gammaEngineLib.h"

/** @brief Builds connections of fields.
 * Build areas of fields owned by @p id player starting from Node @p center
 * @param g - current game
 * @param center - Node that we are merging with
 * @param elem - current field
 * @param id - player id
 */
static void buildArea(gamma_t *g, Node *center, Node *elem, uint32_t id);

/** @brief Load 4 nearby fields
 * @param g - current game
 * @param arr - Node * array with size equal to 4
 * @param x - first coordinate
 * @param y - second coordinate
 */
static void nearbyFields(gamma_t *g, Node *arr[], uint32_t x, uint32_t y);

/** @brief Check if Node is empty.
 * Check @p elem.
 * @param elem - Node being checked
 * @return false if node is not empty or its NULL else true
 */
static bool isEmptyNode(Node *elem);

/** @brief Checks if player owns field.
 * Checks if @p player owns @p elem.
 * @param g - current game
 * @param player - Member's id
 * @param elem - field
 * @return true if @p elem is not NULL and player owns field else false
 */
static bool isMineNode(gamma_t *g, uint32_t player, Node *elem);

/** @brief Give bottom field.
 * Give bottom field of field at (@p x, @p y)
 * @param g - current game
 * @param x - first coordinate
 * @param y - second coordinate
 * @return bottom field or NULL.
 */
static Node *getDown(gamma_t *g, uint32_t x, uint32_t y);

/** @brief Give upper field.
 * Give upper field of field at (@p x, @p y)
 * @param g - current game
 * @param x - first coordinate
 * @param y - second coordinate
 * @return upper field or NULL.
 */
static Node *getUp(gamma_t *g, uint32_t x, uint32_t y);

/** @brief Give left field.
 * Give left field of field at (@p x, @p y)
 * @param g - current game
 * @param x - first coordinate
 * @param y - second coordinate
 * @return left field or NULL.
 */
static Node *getLeft(gamma_t *g, uint32_t x, uint32_t y);

/** @brief Give right field.
 * Give right field of field at (@p x, @p y)
 * @param g - current game
 * @param x - first coordinate
 * @param y - second coordinate
 * @return right field or NULL.
 */
static Node *getRight(gamma_t *g, uint32_t x, uint32_t y);

/** @brief Set nearby field added to false
 * @param g - current game
 * @param player - Member's id
 * @param arr - array ofisEmptyNode nearby fields
 */
static void setNearbyFalse(gamma_t *g, uint32_t player, Node **arr);

/** @brief Get owner of field.
 * Get owner of field at (@p x, @p y) in game @p g
 * @param g - current game
 * @param x - first coordinate
 * @param y - second coordinate
 * @return Owner of field (@p x, @p y)
 */
static inline Member getOwnerPlayer(gamma_t *g, uint32_t x, uint32_t y);

inline bool positive(uint32_t num) {
    return num > 0;
}

bool initMembers(Member *members, uint32_t players) {
    for (unsigned int i = 0; i < players; i++) {
        members[i] = newMember(i + 1);
        if (members[i] == NULL)
            return false;
    }
    return true;
}

bool initBoard(Node ***board, uint32_t width, uint32_t height) {
    for (uint32_t i = 0; i < width; i++)
        for (uint32_t j = 0; j < height; j++) {
            board[i][j] = newRoot(0, i, j);
            if (board[i][j] == NULL)
                return false;
        }
    return true;
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

static bool isEmptyNode(Node *elem) {
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

inline uint32_t getAreas(gamma_t *g, uint32_t player) {
    return g->members[player - 1]->areas;
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

static inline Member getOwnerPlayer(gamma_t *g, uint32_t x, uint32_t y) {
    return getPlayer(g, getOwner(g, x, y));
}

void moveOnEmpty(gamma_t *g, uint32_t x, uint32_t y, bool sub) {

    int change = sub ? -1 : 1;

    Node *arr[4];
    nearbyFields(g, arr, x, y);
    uint32_t newX[4] = {x - 1, x, x + 1, x};
    uint32_t newY[4] = {y, y + 1, y, y - 1};

    for (int i = 0; i < 4; i++) {
        if (arr[i] != NULL && !isEmptyNode(arr[i])
            && !getOwnerPlayer(g, newX[i], newY[i])->changedSurrounding) {

            getOwnerPlayer(g, newX[i], newY[i])->changedSurrounding = true;
            getOwnerPlayer(g, newX[i], newY[i])->surrounding += change;
        }
    }

    for (int i = 0; i < 4; i++) {
        if (arr[i] != NULL && !isEmptyNode(arr[i]) &&
            getOwnerPlayer(g, newX[i], newY[i])->changedSurrounding)
            getOwnerPlayer(g, newX[i], newY[i])->changedSurrounding = false;
    }
}

uint32_t numEmpty(gamma_t *g, uint32_t player, uint32_t x, uint32_t y) {
    uint32_t empty = 0;

    uint32_t newX[4] = {x - 1, x, x + 1, x};
    uint32_t newY[4] = {y, y + 1, y, y - 1};
    Node *arr[4];
    nearbyFields(g, arr, x, y);

    for (int i = 0; i < 4; i++) {
        if (arr[i] != NULL && isEmptyNode(arr[i]))
            empty += numNeighbours(g, player, newX[i], newY[i]) == 1;
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

static void nearbyFields(gamma_t *g, Node *arr[], uint32_t x, uint32_t y) {
    arr[0] = getLeft(g, x, y);
    arr[1] = getUp(g, x, y);
    arr[2] = getRight(g, x, y);
    arr[3] = getDown(g, x, y);
}

static void setNearbyFalse(gamma_t *g, uint32_t player, Node **arr) {
    for (int i = 0; i < 4; i++)
        if (arr[i] != NULL && isMineNode(g, player, arr[i])) {
            setRootAdded(arr[i], false);
        }
}

uint32_t areasChange(gamma_t *g, uint32_t player,
                     uint32_t x, uint32_t y, bool middle) {

    uint32_t output = 0;
    Node *arr[4]; // nearby fields
    nearbyFields(g, arr, x, y);
    Node *mine = NULL;

    for (int i = 0; i < 4; i++) {
        if (arr[i] != NULL && !isRootAdded(arr[i]) &&
            isMineNode(g, player, arr[i])) {

            if (mine == NULL)
                mine = arr[i];
            else if (middle) {
                // prevent arr[i] becoming a root and mine->added remaining true
                setRootAdded(mine, false);
                merge(mine, arr[i]);
            }

            setRootAdded(arr[i], true);
            output++;
        }
    }

    setNearbyFalse(g, player, arr);
    if (middle) {
        merge(mine, getField(g, x, y));
    }

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
