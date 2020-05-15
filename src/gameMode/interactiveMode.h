/** @file
 * Interactive mode interface
 *
 * @author Karol Zagródka <karol.zagrodka@gmail.com>
 */

#ifndef GAMMA_INTERACTIVEMODE_H
#define GAMMA_INTERACTIVEMODE_H

#include <stdint.h>
#include <stdbool.h>

/** @brief Initialize game in Interactive mode
 * Creates new game and allow to play it in Batch mode
 * @param values - array of values needed to initialize gamma game
 * @return true if game was played else false
 */
bool initializeInteractive(uint32_t values[]);

#endif //GAMMA_INTERACTIVEMODE_H
