
#ifndef GAMMA_FINDUNION_H
#define GAMMA_FINDUNION_H

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct Node Node;


Node *makeSet(uint32_t x, uint32_t y);

Node *find(Node *elem);

bool sameRoot(Node *a, Node *b);

Node *merge(Node *a, Node *b);

void removeNode(Node *elem);

#endif //GAMMA_FINDUNION_H
