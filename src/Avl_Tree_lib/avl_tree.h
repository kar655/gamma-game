

#ifndef SMALLPROJECT_AVL_TREE_H
#define SMALLPROJECT_AVL_TREE_H

#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include "../findUnionLib/findUnion.h"


struct TreeNode;

typedef struct TreeNode TreeNode;

typedef TreeNode* AvlTree;

// TODO sortowanie po x, y
struct TreeNode {
    Node *data; // This is node from FindUnion
    int height;
    AvlTree left, right;
};

// Add new node to tree
void insert(AvlTree *tree, Node* field);

// Removes node with name = key
AvlTree deleteNode(AvlTree tree, Node *field);

// przechodzi bfsem po wszystkich pustych sasiadujacych
// i liczy ile ich jest dla kazdego TreeNoda w root gracza
// zamienia added na true i potem drugi raz i zamienia added na false
// bool iterate(AvlTree tree);

// Delete AvlTree but not fields
void removeAll(AvlTree tree);

#endif //SMALLPROJECT_AVL_TREE_H
