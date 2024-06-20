#ifndef LIB_FONT_H
#define LIB_FONT_H

#include "common/common.h"

#define FONT_BYTE_SIZE 6

#define FONT_FIRST_CHARACTER_CODE 0x20

/**
 * @brief      get font character code
 *
 * @param[in]  character  desired character
 *
 * @return     character code array ptr
 */
const uint8_t *get_charater_code(char character);

#endif
