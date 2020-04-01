

#ifndef SMALLPROJECT_AVL_TREE_H
#define SMALLPROJECT_AVL_TREE_H

#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include "../findUnionLib/findUnion.h"


struct TreeNode;

typedef struct TreeNode TreeNode;

typedef TreeNode* AvlTree;

// Add new node to tree
void insert(AvlTree *tree, Node* field);

// Removes node with name = key
AvlTree deleteNode(AvlTree tree, Node *field);

// opdala bfsa jak print xd

// Delete AvlTree but not fields
void removeAll(AvlTree tree);

#endif //SMALLPROJECT_AVL_TREE_H
