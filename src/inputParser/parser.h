/** @file
 * Interface of input parser
 *
 * @author Karol Zagródka <karol.zagrodka@gmail.com>
 */

#ifndef GAMMA_PARSER_H
#define GAMMA_PARSER_H

#include <stdint.h>
#include <stdbool.h>

/** @brief Print message.
 * Print message in @p str and frees it
 * @param str - text that is printed
 */
void textMessage(char *str);

/** @brief Prints ok message.
 * Prints OK with line number
 */
void okMessage();

/** @brief Prints error message.
 * Prints ERROR with line number
 */
void errorMessage();

/** @brief Extract number from @p str.
 * Extract @p expectingValues numbers from @p str and store it in @p values
 * @param values - Array where read numbers will be stored
 * @param str - string with numbers
 * @param expectingValues - number of numbers to be read
 * @return true if exaclty @p expectingValues numbers were read and the string
 * contains only whitespaces else false
 */
bool readNumbers(uint32_t values[], char *str, int expectingValues);

/** @brief Reads line from standard input.
 * Reads line and save it to @p str
 * @param str - read line
 * @return false if EOF else true
 */
bool giveLine(char **str);

/** @brief Plays gamma game.
 */
void playGame();

#endif //GAMMA_PARSER_H
