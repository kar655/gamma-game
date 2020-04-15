/** @file
 * Interface of class Player
 *
 * @author Karol Zagródka <karol.zagrodka@gmail.com>
 */

#ifndef GAMMA_PLAYER_H
#define GAMMA_PLAYER_H

#include "../findUnionLib/findUnion.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/** @brief Player structure.
 *
 * Remembers player's basic information.
 */
struct member {
    uint32_t id;                /**< Player's id */
    uint32_t areas;             /**< Number of owned areas */

    uint64_t owned;             /**< Number of owned fields */
    uint32_t goldenMoves;       /**< Used golden moves */

    uint64_t surrounding;       /**< Number of empty fields surrounding */
    bool changedSurrounding;    /**< If surrounding fields were changed */
};

/**
 * Player structure.
 */
typedef struct member member;

/**
 * Pointer to player structure.
 */
typedef member * Member;

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


#endif //GAMMA_PLAYER_H
