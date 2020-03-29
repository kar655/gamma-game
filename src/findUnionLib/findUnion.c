
#include "findUnion.h"

typedef struct Node Node;

struct Node {
    uint32_t x;
    uint32_t y;
    uint32_t rank;

    Node *parent;
};


Node *makeSet(uint32_t x, uint32_t y) {
    Node *output = (Node *) malloc(sizeof(Node));
    if (output == NULL)
        return NULL;

    *output = (Node) {x, y, 0, output};

    return output;
}

Node *find(Node *elem) {
    if (elem == NULL)
        return NULL;

    while (elem->parent != elem) {
        elem = elem->parent;
        elem->parent = find(elem->parent);
    }

    return elem;
}

bool sameRoot(Node *a, Node *b) {
    return find(a) == find(b);
}

Node *merge(Node *a, Node *b) {
    Node *aRoot = find(a);
    Node *bRoot = find(b);

    if (aRoot == bRoot)
        return aRoot;


    if (aRoot->rank >= bRoot->rank) {
        bRoot->parent = aRoot;
        aRoot->rank++;
        return aRoot;
    }
    else {
        aRoot->parent = bRoot;
        bRoot->rank++;
        return bRoot;
    }
}

void removeNode(Node *elem) {
    free(elem);
    elem = NULL;
}
