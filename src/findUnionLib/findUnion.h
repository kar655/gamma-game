
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


Node *newRoot(uint32_t player, uint32_t x, uint32_t y);

uint32_t getData(Node *elem);

void setData(Node *elem, uint32_t id);

bool isAdded(Node *elem);

void setAdded(Node *elem, bool state);

Node *find(Node *elem);

bool sameRoot(Node *a, Node *b);

Node *merge(Node *a, Node *b);

void removeNode(Node *elem);

#endif //GAMMA_FINDUNION_H
