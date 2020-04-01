

#ifndef GAMMA_PLAYER_H
#define GAMMA_PLAYER_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include "../findUnionLib/findUnion.h"
#include "../Avl_Tree_lib/avl_tree.h"


struct member {
    uint32_t id;
    uint32_t areas;

    uint64_t owned;
    uint32_t goldenMoves;
    // array of roots of areas
    uint32_t size;
    AvlTree roots;
};


typedef struct member member;
typedef member *Member;

Member newMember(uint32_t id);

void removeMember(Member player);

//void addRoot(Member player, Node *elem);

#endif //GAMMA_PLAYER_H
