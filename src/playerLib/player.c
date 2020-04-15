/** @file
 * Implementation of class Player
 *
 * @author Karol Zagródka <karol.zagrodka@gmail.com>
 */

#include "player.h"


Member newMember(uint32_t id) {
    Member output = malloc(sizeof(member));
    if (output == NULL)
        return NULL;
    *output = (member) {id, 0, 0, 0, 0, false};

    return output;
}

void removeMember(Member player) {
    free(player);
}
