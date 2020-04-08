
#ifndef GAMMA_GAMMAENGINELIB_H
#define GAMMA_GAMMAENGINELIB_H

#include "../playerLib/player.h"
#include "../Avl_Tree_lib/avl_tree.h"
#include "../playerLib/player.h"
//#include "../gamma.h"

struct gamma {
    uint32_t width;
    uint32_t height;
    uint32_t players;
    uint32_t areas;

    uint32_t resetCounter;
    uint32_t numGoldenMoves;
    uint64_t available;
    Member *members;
    Node ***board;
};

typedef struct gamma gamma_t;


bool positive(uint32_t num);

void initMembers(Member *members, uint32_t players);

void initBoard(Node ***board, uint32_t width, uint32_t height);

Member getPlayer(gamma_t *g, uint32_t player);

bool wrongInput(gamma_t *g, uint32_t player);

bool wrongCoordinates(gamma_t *g, uint32_t x, uint32_t y);

bool isEmpty(gamma_t *g, uint32_t x, uint32_t y);

bool hasGoldenMoves(gamma_t *g, uint32_t player);

bool isMineNode(gamma_t *g, uint32_t player, Node *elem);

Member getPlayer(gamma_t *g, uint32_t player);

bool isMine(gamma_t *g, uint32_t player, uint32_t x, uint32_t y);

bool isEnemy(gamma_t *g, uint32_t player, uint32_t x, uint32_t y);

uint32_t getAreas(gamma_t *g, uint32_t player);

AvlTree getPlayerRoots(gamma_t *g, uint32_t player);

void takeField(gamma_t *g, uint32_t player, uint32_t x, uint32_t y);

uint32_t getOwner(gamma_t *g, uint32_t x, uint32_t y);

Node *getDown(gamma_t *g, uint32_t x, uint32_t y);

Node *getUp(gamma_t *g, uint32_t x, uint32_t y);

Node *getLeft(gamma_t *g, uint32_t x, uint32_t y);

Node *getRight(gamma_t *g, uint32_t x, uint32_t y);

void mergeFields(gamma_t *g, uint32_t player, Node *arr[], Node *biggest);

uint32_t numNeighbours(gamma_t *g, uint32_t player, uint32_t x, uint32_t y);

uint32_t areasChange(gamma_t *g, uint32_t player,
                     uint32_t x, uint32_t y, bool merged);

void clearRelations(gamma_t *g, Node *elem, uint32_t id);

void buildArea(gamma_t *g, Node *center, Node *elem, uint32_t id);



#endif //GAMMA_GAMMAENGINELIB_H
