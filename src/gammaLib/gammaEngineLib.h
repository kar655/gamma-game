/** @file
 * Helper functions to gamma functions.
 *
 * @author Karol Zagródka <karol.zagrodka@gmail.com>
 */

#ifndef GAMMA_GAMMAENGINELIB_H
#define GAMMA_GAMMAENGINELIB_H

#include "../playerLib/player.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/** @brief Structure that holds gama data.
 *
 * Remember gama data.
 */
struct gamma {
    uint32_t width;             /**< Game width */
    uint32_t height;            /**< Game height */
    uint32_t players;           /**< Number of players */
    uint32_t areas;             /**< Maximal number of areas */

    uint32_t numGoldenMoves;    /**< Maximal number of golden moves */
    uint64_t available;         /**< Number of empty fields */
    Member *members;            /**< Array of players */
    Node ***board;              /**< 2D Array of fields */
};

/**
 * Game structure.
 */
typedef struct gamma gamma_t;

/** @brief if @p num > 0.
 * @param num - number
 * @return true if @p num > 0 else false
 */
bool positive(uint32_t num);

/** @brief Initialize all members.
 * Allocates memory for all Members in array
 * @param members - array of members
 * @param players - number of members
 * @return true if init had enough memory else false
 */
bool initMembers(Member *members, uint32_t players);

/** @brief Initialize game board.
 * Allocates memory for all fields
 * @param board - allocated memory for new board
 * @param width - board width
 * @param height - board height
 * @return true if init had enough memory else false
 */
bool initBoard(Node ***board, uint32_t width, uint32_t height);

/** @brief Get player from current game.
 * Get player with id @p player from @p g game
 * @param g - current game
 * @param player - Member's id
 * @return Member with id @p player
 */
Member getPlayer(gamma_t *g, uint32_t player);

/** @brief Give field with given coordinates.
 * @param g - current game
 * @param x - first coordinate
 * @param y - second coordinate
 * @return Field in game @p g at (@p x, @p y)
 */
Node *getField(gamma_t *g, uint32_t x, uint32_t y);

/** @brief Check input data.
 * Check if @p g is not NULL and if @p player exists.
 * @param g - current game
 * @param player - player id
 * @return true if data is correct else false
 */
bool wrongInput(gamma_t *g, uint32_t player);

/** @brief Check coordinates.
 * Check if @p g contains field at (@p x, @p y)
 * @param g - current game
 * @param x - first coordinate
 * @param y - second coordinate
 * @return true if data is correct else false
 */
bool wrongCoordinates(gamma_t *g, uint32_t x, uint32_t y);

/** @brief Check if field is empty.
 * Check if field (@p x, @p y) doesn't have owner
 * @param g - current game
 * @param x - first coordinate
 * @param y - second coordinate
 * @return
 */
bool isEmpty(gamma_t *g, uint32_t x, uint32_t y);

/** @brief Check if player has golden move.
 * Check if @p player has golden move.
 * @param g - current game
 * @param player - Member's id
 * @return
 */
bool hasGoldenMoves(gamma_t *g, uint32_t player);

/** @brief Check if player owns field.
 * Check if @p player owns field at (@p x, @p y)
 * @param g - current game
 * @param player - Member's id
 * @param x - first coordinate
 * @param y - second coordinate
 * @return true if player owns field else false
 */
bool isMine(gamma_t *g, uint32_t player, uint32_t x, uint32_t y);

/** @brief Returns number of areas owned by @p player.
 *
 * @param g - current game
 * @param player - Member's id
 * @return Number of areas owned by @p player
 */
uint32_t getAreas(gamma_t *g, uint32_t player);

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

/** @brief Changes surrounding counter.
 * Changes surrounding counter of players that had fields nearby (@p x, @p y)
 * @param g - current game
 * @param x - first coordinate
 * @param y - second coordinate
 * @param sub - subtraction
 */
void moveOnEmpty(gamma_t *g, uint32_t x, uint32_t y, bool sub);

/** @brief Calculates number of empty nearby fields.
 * Calculates number of empty fields of field at (@p x, @p y)
 * which has only 1 neighbour.
 * @param g - current game
 * @param player - Member's id
 * @param x - first coordinate
 * @param y - second coordinate
 * @return number of empty nearby fields with exactly 1 neighbour
 */
uint32_t numEmpty(gamma_t *g, uint32_t player, uint32_t x, uint32_t y);

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
