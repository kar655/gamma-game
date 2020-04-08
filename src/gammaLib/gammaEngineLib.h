/** @file
 * Interface of class Player
 *
 * @author Karol Zagródka <karol.zagrodka@gmail.com>
 */

#ifndef GAMMA_GAMMAENGINELIB_H
#define GAMMA_GAMMAENGINELIB_H

#include "../playerLib/player.h"
#include "../Avl_Tree_lib/avl_tree.h"
//#include "../gamma.h"

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

typedef struct gamma gamma_t;


bool positive(uint32_t num);

/** @brief Initialize all members.
 * Allocates memory for all Members in array
 * @param members - array of members
 * @param players - number of members
 */
void initMembers(Member *members, uint32_t players);

void initBoard(Node ***board, uint32_t width, uint32_t height);

Member getPlayer(gamma_t *g, uint32_t player);

Node *getField(gamma_t *g, uint32_t x, uint32_t y);

bool wrongInput(gamma_t *g, uint32_t player);

bool wrongCoordinates(gamma_t *g, uint32_t x, uint32_t y);

bool isEmpty(gamma_t *g, uint32_t x, uint32_t y);

bool hasGoldenMoves(gamma_t *g, uint32_t player);

bool isMineNode(gamma_t *g, uint32_t player, Node *elem);

Member getPlayer(gamma_t *g, uint32_t player);

bool isMine(gamma_t *g, uint32_t player, uint32_t x, uint32_t y);

bool isEnemy(gamma_t *g, uint32_t player, uint32_t x, uint32_t y);

/** @brief Returns number of areas owned by @p player.
 *
 * @param g - current game
 * @param player - Member's id
 * @return Number of areas owned by @p player
 */
uint32_t getAreas(gamma_t *g, uint32_t player);

AvlTree getPlayerRoots(gamma_t *g, uint32_t player);

/** @brief Takes empty field.
 * Changes field (@p x, @p y) owner. Opposite to resetField()
 * @param g - current game
 * @param player - Member's id
 * @param x - first coordinate
 * @param y - second coordinate
 */
void takeField(gamma_t *g, uint32_t player, uint32_t x, uint32_t y);

/** @brief Makes field empty.
 * Changes field (@p x, @p y) owner to 0. Opposite to takeField()
 * @param g - current game
 * @param player - Member's id
 * @param x - first coordinate
 * @param y - second coordinate
 */
void resetField(gamma_t *g, uint32_t player, uint32_t x, uint32_t y);

/** @brief Returns owner's id.
 * Return id of field (@p x, @p y) owner.
 * @param g - current game
 * @param x - first coordinate
 * @param y - second coordinate
 * @return owner's id
 */
uint32_t getOwner(gamma_t *g, uint32_t x, uint32_t y);

Node *getDown(gamma_t *g, uint32_t x, uint32_t y);

Node *getUp(gamma_t *g, uint32_t x, uint32_t y);

Node *getLeft(gamma_t *g, uint32_t x, uint32_t y);

Node *getRight(gamma_t *g, uint32_t x, uint32_t y);

/** @brief Merges fields.
 * Merge fields owned by @p player in @p arr with @p biggest
 * @param g - current game
 * @param player - Member's id
 * @param arr - array of nearby fields
 * @param biggest - field with biggest rank among nearby fields
 */
void mergeFields(gamma_t *g, uint32_t player, Node *arr[], Node *biggest);

/** @brief Returns number of friendly fields connected to certain field.
 * Calculates number of fields owned by @p player and connected to field at
 * coordinates (@p x, @p y).
 * @param g - current game
 * @param player - Member's id
 * @param x - first coordinate
 * @param y - second coordinate
 * @return Number from 0 to 4 of friendly fields
 */
uint32_t numNeighbours(gamma_t *g, uint32_t player, uint32_t x, uint32_t y);

/** @brief Return number of areas near field.
 * Calculates number of different areas owned by @p player
 * next to (@p x, @p y) field.
 * @param g - current game
 * @param player - Member's id
 * @param x - first coordinate
 * @param y - second coordinate
 * @param middle - merge with middle field
 * @return Number from 0 to 4 of different areas
 */
uint32_t areasChange(gamma_t *g, uint32_t player,
                     uint32_t x, uint32_t y, bool middle);

/** @brief Clear relations in area.
 *  Clear data in all Nodes in area, mark as added.
 * @param g - current game
 * @param elem - initial Node
 * @param id - Member's id
 */
void clearRelations(gamma_t *g, Node *elem, uint32_t id);


/** @brief Build areas on nearby fields.
 * Build areas nearby field (@p x, @p y) owned by Member's id @p player
 * @param g - current game
 * @param player - Member's id
 * @param x - first coordinate
 * @param y - second coordinate
 */
void buildConnected(gamma_t *g, uint32_t player, uint32_t x, uint32_t y);



#endif //GAMMA_GAMMAENGINELIB_H
