/** @file
 * Implementation of class Player
 *
 * @author Karol Zagródka <karol.zagrodka@gmail.com>
 */

#include "player.h"


Member newMember(uint32_t id) {
    Member output = malloc(sizeof(member));
    if (output == NULL)
        exit(1);
    *output = (member) {id, 0, 0, 0, 0, NULL, 0, false};

    return output;
}

void removeMember(Member player) {
//    for (uint32_t i = 0; i < player->areas; i++) {
//        removeNode(player->roots[i]);
//    }

//    free(player->roots);
    // removeAll(player->roots);
    free(player);
}
