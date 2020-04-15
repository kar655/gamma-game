/** @file
 * Interface of Find and Union
 *
 * @author Karol Zagródka <karol.zagrodka@gmail.com>
 */

#ifndef GAMMA_FINDUNION_H
#define GAMMA_FINDUNION_H

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>


typedef struct Node Node;

/** @brief Structure that holds field data.
 *
 * Able to perform find and union.
 */
struct Node {
    uint32_t x,             /**< First coordinate */
    y;                      /**< Second coordinate */

    bool added;             /**< State of Node */
    uint32_t owner;         /**< Id of owner */
    uint32_t rank;          /**< Number of merges to this Node */

    Node *parent;           /**< Parent Node */
};

/** @brief Creates new root.
 * Creates root owned by @p player at coords (@p x, @p y)
 * @param player - new owner
 * @param x - first coordinate
 * @param y - second coordinate
 * @return Pointer to new root
 */
Node *newRoot(uint32_t player, uint32_t x, uint32_t y);

/** @brief Check if @p elem root is Added.
 * @param elem - Node being checked
 * @return true if @p elem root is added else false
 */
bool isRootAdded(Node *elem);

/** @brief Set @p elem added.
 * Changes @p elem root added to @p state
 * @param elem - Son of root being changed
 * @param state - new state
 */
void setRootAdded(Node *elem, bool state);

/** @brief Returns root of Node.
 * Return root of @p elem and performs path compression
 * @param elem - Root's son
 * @return NULL if @p elem is NULL else @p elem's root
 */
Node *find(Node *elem);

/** @brief Check if nodes have same root.
 * Check if node @p a and @p b have same root
 * @param a - first Node
 * @param b - second Node
 * @return true if @p a and @p b are not NULL and have same root else false
 */
bool sameRoot(Node *a, Node *b);

/** @brief Merge two roots.
 * Merge @p a and @p b roots, performs path compression
 * @param a - first Node
 * @param b - second Node
 * @return Root of merged nodes
 */
Node *merge(Node *a, Node *b);

/** @brief Clears temporary data.
 * Removes connection with other Nodes
 *
 * @param elem - Node to clear data
 */
void clearNodeData(Node *elem);

#endif //GAMMA_FINDUNION_H
