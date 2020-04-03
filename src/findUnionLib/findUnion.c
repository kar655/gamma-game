
#include "findUnion.h"


Node *newRoot(uint32_t player, uint32_t x, uint32_t y) {
    Node *output = (Node *) malloc(sizeof(Node));
    if (output == NULL)
        return NULL;

    *output = (Node) {x, y, false, 0, player, 0, output};

    return output;
}

inline uint32_t getData(Node *elem) {
    return elem == NULL ? 0 : elem->owner;
}

void setData(Node *elem, uint32_t id) {
    elem->owner = id;
}

inline bool isAdded(Node *elem) {
    return elem == NULL ? true : elem->added;
}

void setAdded(Node *elem, bool state) {
    if (elem == NULL)
        return;
    elem->added = state; // TODO co jest???
}

Node *find(Node *elem) {
    if (elem == NULL)
        return NULL;

    while (elem->parent != elem) {
        elem->parent = find(elem->parent);
        elem = elem->parent;
    }

    return elem;
}

bool sameRoot(Node *a, Node *b) {
    if (a == NULL || b == NULL)
        return false;
    else
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
