#ifndef COMMON_COMMON_H
#define COMMON_COMMON_H

#include <stdarg.h>
#include <stdint.h>

/**
 * @brief      just dirty hack to make compiler
 *             not warning about function pointers
 *
 */
#define TASK_PTR(task)                      ((void (*)())task)

/**
 * @defgroup REGISTER_MACRO macros to reduce typing later on
 *
 * TODO: move it out of here
 *
 * @{
 */
#define REGISTER_32(ADDRESS)                (*((volatile uint32_t *)(ADDRESS)))
#define REGISTER_16(ADDRESS)                (*((volatile uint16_t *)(ADDRESS)))
#define REGISTER_8(ADDRESS)                 (*((volatile uint8_t *)(ADDRESS)))

/** @} */

/**
 * @defgroup BIT_OPERATIONS bit operations macro
 *
 * TODO: move it out of here
 *
 * @{
 */

#define BIT(_n) (1 << (_n))

#define BIT0  (1 <<  0)
#define BIT1  (1 <<  1)
#define BIT2  (1 <<  2)
#define BIT3  (1 <<  3)
#define BIT4  (1 <<  4)
#define BIT5  (1 <<  5)
#define BIT6  (1 <<  6)
#define BIT7  (1 <<  7)
#define BIT8  (1 <<  8)
#define BIT9  (1 <<  9)
#define BIT10 (1 << 10)
#define BIT11 (1 << 11)
#define BIT12 (1 << 12)
#define BIT13 (1 << 13)
#define BIT14 (1 << 14)
#define BIT15 (1 << 15)
#define BIT16 (1 << 16)
#define BIT17 (1 << 17)
#define BIT18 (1 << 18)
#define BIT19 (1 << 19)
#define BIT20 (1 << 20)
#define BIT21 (1 << 21)
#define BIT22 (1 << 22)
#define BIT23 (1 << 23)
#define BIT24 (1 << 24)
#define BIT25 (1 << 25)
#define BIT26 (1 << 26)
#define BIT27 (1 << 27)
#define BIT28 (1 << 28)
#define BIT29 (1 << 29)
#define BIT30 (1 << 30)
#define BIT31 (1 << 31)

/** @} */

/**
 * @defgroup LD_EXTERNAL linker script externals
 *
 * TODO: move it out of here
 *
 * @{
 */

extern unsigned char  INIT_DATA_VALUES;
extern unsigned char  INIT_DATA_START;
extern unsigned char  INIT_DATA_END;
extern unsigned char  BSS_START;
extern unsigned char  BSS_END;
extern unsigned char  HEAP_START;
extern unsigned char  HEAP_END;
extern unsigned char  STACK_START;
extern unsigned char  STACK_END;

/** @} */

/**
 * @brief      initialize memory with given value
 *
 * @param      ptr    pointer to memory
 * @param[in]  value  init value
 * @param[in]  size   memory size
 */
void memset(void *ptr, const uint8_t value, uint32_t size);

/**
 * @brief      copy memory chunk
 *
 * @param      dst   destination memory ptr
 * @param      src   source memory ptr
 * @param[in]  n     memory size
 */
void memcpy(void *dst, const void *src, uint32_t n);

/**
 * @brief      compare memory chunks
 *
 * @param[in]  ptr1  first chunk ptr
 * @param[in]  ptr2  second chunk ptr
 * @param[in]  n     chunks size
 *
 * @return     negative/positive if not equal, otherwise 0
 */
int memcmp(const void *ptr1, const void *ptr2, uint32_t n);

#endif
