
#ifndef GAMMA_PARSER_H
#define GAMMA_PARSER_H

#include <stdint.h>
#include <stdbool.h>

void textMessage(char *str);

void okMessage();

void errorMessage();

bool giveLine(char **str);

bool readNumbers(uint32_t values[], char *str, int expectingValues);

void playGame();

#endif //GAMMA_PARSER_H
