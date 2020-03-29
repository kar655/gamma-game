

#include "player.h"


Member newMember(uint32_t id) {
    Member output = malloc(sizeof(member));
    if (output == NULL)
        exit(1);
    *output = (member) {id, 0, 0, 0, 0, NULL};

    return output;
}

void removeMember(Member player) {
//    for (uint32_t i = 0; i < player->areas; i++) {
//        removeNode(player->roots[i]);
//    }

    free(player->roots);
    free(player);
    player = NULL;
}

void addRoot(Member player, Node *elem) {
    if (player == NULL || elem == NULL)
        return;

    // Adding first root
    if (player->size == 0) {
        player->size = 1;
        player->roots = malloc(sizeof(Node *));
    }

    // have space
    if (player->areas >= player->size) {
        player->size *= 2;
        player->roots = realloc(player->roots, player->size);
    }

    player->roots[player->areas] = elem;
    player->areas++;
}