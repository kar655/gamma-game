/** @file
 * Interface of class Player
 *
 * @author Karol Zagródka <karol.zagrodka@gmail.com>
 */

#ifndef GAMMA_PLAYER_H
#define GAMMA_PLAYER_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include "../findUnionLib/findUnion.h"
#include "../Avl_Tree_lib/avl_tree.h"

/** @brief Player structure.
 *
 * Remembers player's basic information.
 */
struct member {
    uint32_t id;                /**< Player's id */
    uint32_t areas;             /**< Number of owned areas */

    uint64_t owned;             /**< Number of owned fields */
    uint32_t goldenMoves;       /**< Used golden moves */

    uint32_t size;
    AvlTree roots;              /**< todo */
    uint64_t surrounding;       /**< Number of empty fields surrounding*/
    bool changedSurrounding;    /**< todo*/
};


typedef struct member member;

typedef member *Member;

/** @brief Creates new Member
 * Creates new member with id = @p id
 * @param id - new member's id
 * @return pointer to new Member
 */
Member newMember(uint32_t id);

/** @brief Removes Member
 * Removes Member and clears memory
 * @param player - Member to be removed
 */
void removeMember(Member player);

//void addRoot(Member player, Node *elem);

#endif //GAMMA_PLAYER_H
