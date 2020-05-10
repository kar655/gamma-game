//
// Created by karol on 05/05/2020.
//

#ifndef GAMMA_PARSER_H
#define GAMMA_PARSER_H

#include <stdint.h>

void nextLine();

void textMessage(char *str);

void lineMessage(int vlaue);

void okMessage();

void errorMessage();

int giveLine(char **str);

int readNumbers(uint32_t values[], char *str, int expectingValues);

int getMode();

#endif //GAMMA_PARSER_H
