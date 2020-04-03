
#include "avl_tree.h"

#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>




// returns bigger int
static int max(int a, int b);

static int compareNodes(AvlTree tree, Node *field);

// creates new node
static AvlTree newNode(Node *field);

// returns tree's height
static int getHeight(AvlTree tree);

// updates height
static void correctHeight(AvlTree tree);

// returns balance factor of node *tree
static int balanceFactor(AvlTree tree);

// rotates left
static void rotateLeft(AvlTree *tree);

// rotates right
static void rotateRight(AvlTree *tree);

// keeps logarithmic height
static void balance(AvlTree *tree, Node *field);

// keeps logarithmic height
static void balanceAfterDel(AvlTree *tree);

// returns Node with minimal key value
// of non-empty tree
static AvlTree minValueNode(AvlTree tree);

static void delNodeHelper(AvlTree *tree);


// Implementation
// ----------------------------------------------------------------------------

static inline int max(int a, int b) {
    return (a >= b) ? a : b;
}


static int compareNodes(AvlTree tree, Node *field) {
    if (tree->data->x < field->x)
        return -1;
    else if (tree->data->x > field->x)
        return 1;
    else if (tree->data->y < field->y)   // tree->data->x == field->x
        return -1;
    else if (tree->data->y > field->y)
        return 1;
    else
        return 0;
}


static AvlTree newNode(Node *field) {
    AvlTree tree = (AvlTree) malloc(sizeof(TreeNode));
    if (tree == NULL)
        exit(1);
    *tree = (TreeNode) {field, 1, NULL, NULL};
    return tree;
}


static inline int getHeight(AvlTree tree) {
    return (tree == NULL) ? 0 : tree->height;
}


static inline void correctHeight(AvlTree tree) {
    tree->height = max(getHeight(tree->left), getHeight(tree->right)) + 1;
}


static inline int balanceFactor(AvlTree tree) {
    if (tree == NULL)
        return 0;
    else
        return getHeight(tree->left) - getHeight(tree->right);
}


static void rotateLeft(AvlTree *tree) {
    AvlTree rightTree = (*tree)->right;
    (*tree)->right = rightTree->left;
    rightTree->left = *tree;

    correctHeight(*tree);
    correctHeight(rightTree);
    *tree = rightTree;
}


static void rotateRight(AvlTree *tree) {
    AvlTree leftTree = (*tree)->left;
    (*tree)->left = leftTree->right;
    leftTree->right = *tree;

    correctHeight(*tree);
    correctHeight(leftTree);
    *tree = leftTree;
}


static void balance(AvlTree *tree, Node *field) {
    int comparison;
    int bFactor = balanceFactor(*tree);

    if (bFactor > 1) {
        comparison = compareNodes((*tree)->left, field);

        if (comparison > 0) // left left
            rotateRight(tree);
        else { // left right
            rotateLeft(&(*tree)->left);
            rotateRight(tree);
        }
    }
    else if (bFactor < -1) {
        comparison = compareNodes((*tree)->right, field);

        if (comparison > 0) { // right left
            rotateRight(&(*tree)->right);
            rotateLeft(tree);
        }
        else // right right
            rotateLeft(tree);
    }
}


static void balanceAfterDel(AvlTree *tree) {
    int bFactor = balanceFactor(*tree);

    if (bFactor > 1) {
        if (balanceFactor((*tree)->left) >= 0) // left left
            rotateRight(tree);
        else { // left right
            rotateLeft(&(*tree)->left);
            rotateRight(tree);
        }
    }
    else if (bFactor < -1) {
        if (balanceFactor((*tree)->right) <= 0) // right right
            rotateLeft(tree);
        else { // right left
            rotateRight(&(*tree)->right);
            rotateLeft(tree);
        }
    }
}


static AvlTree minValueNode(AvlTree tree) {
    AvlTree current = tree;

    while (current->left != NULL)
        current = current->left;

    return current;
}


void insert(AvlTree *tree, Node* field) {
//    if (field == NULL)
//        return;

    if ((*tree) == NULL) {
        *tree = newNode(field);
        return;
    }
    int comparison = compareNodes(*tree, field);
            // (*tree)->name, key);
    if (comparison == 0)
        return;

    if (comparison < 0)
        insert(&((*tree)->right), field);
    else if (comparison > 0)
        insert(&((*tree)->left), field);

    correctHeight(*tree);
    balance(tree, field);
}


void printAll(AvlTree tree) {
    if (tree != NULL) {
        printAll(tree->left);
//        printf("%s\n", tree->name);
        printAll(tree->right);
    }
}




static void delNodeHelper(AvlTree *tree) {
    if ((*tree)->left == NULL || (*tree)->right == NULL) {
        // not Null child
        AvlTree temp = (*tree)->left ? (*tree)->left : (*tree)->right;

        if (temp == NULL) { // No child case

            free(*tree);
            *tree = NULL;
        }
        else { // One child case

            **tree = *temp;
        }
        // the non-empty child
        free(temp);
    }
    else {
        // node with two children
        AvlTree temp = minValueNode((*tree)->right);

        // todo dziala?
        (*tree)->data = temp->data;

        // Delete copied but not it's dictionary
        (*tree)->right = deleteNode((*tree)->right, temp->data);
    }
}


AvlTree deleteNode(AvlTree tree, Node *field) {
    if (tree == NULL)
        return NULL;

    int comparison = compareNodes(tree, field);

    if (comparison < 0)
        tree->right = deleteNode(tree->right, field);
    else if (comparison > 0)
        tree->left = deleteNode(tree->left, field);
    else { // comparison == 0
        delNodeHelper(&tree);
    }

    if (tree == NULL)
        return tree;

    // update height
    correctHeight(tree);
    balanceAfterDel(&tree);

    return tree;
}


void removeAll(AvlTree tree) {
    if (tree != NULL) {
        removeAll(tree->left);
        removeAll(tree->right);
        free(tree);
    }
}
