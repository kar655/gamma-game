/** @file
 * Interface of class Player
 *
 * @author Karol Zagródka <karol.zagrodka@gmail.com>
 */

#ifndef GAMMA_FINDUNION_H
#define GAMMA_FINDUNION_H

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>


typedef struct Node Node;

struct Node {
    uint32_t x, y;

    bool added;
    uint32_t currentReset;
    uint32_t owner;
    uint32_t rank;

    Node *parent;
};

/** @brief Creates new root.
 * Creates root owned by @p player at coords (@p x, @p y)
 * @param player - new owner
 * @param x - first coordinate
 * @param y - second coordinate
 * @return Pointer to new root
 */
Node *newRoot(uint32_t player, uint32_t x, uint32_t y);

uint32_t getData(Node *elem);

void setData(Node *elem, uint32_t id);

bool isAdded(Node *elem);

bool isRootAdded(Node *elem);

void setAdded(Node *elem, bool state);

void setRootAdded(Node *elem, bool state);

/** @brief Returns root of node.
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

void clearNodeData(Node *elem);

void removeNode(Node *elem);

#endif //GAMMA_FINDUNION_H
