#ifndef LIB_STRING_H
#define LIB_STRING_H

#include "common/common.h"
#include "platform/usart.h"

#define STR_BUF_SIZE 24

#define strsize(STR) (strlen(STR) + 1)

/**
 * @brief      compare two strings
 *
 * @param[in]  str1  the string 1
 * @param[in]  str2  the string 2
 * @param[in]  n     bytes to compare
 *
 * @return     result of comparison
 */
int strncmp(const char *str1, const char *str2, int n);

/**
 * @brief      compare two strings
 * NOTE:       returns error if strings don't have equal length
 *
 * @param[in]  str1  the string 1
 * @param[in]  str2  the string 2

 * @return     result of comparison
 */
int strcmp(const char *str1, const char *str2);

/**
 * @brief      get length of string
 *
 * @param[in]  str   the string
 *
 * @return     lenght of the string
 */
int strlen(const char *str);

/**
 * @brief      get length of string (safe variant)
 *
 * @param[in]  str   the string
 *
 * @param[in]  size  max string size
 *
 * @return     lenght of the string
 */
int strnlen(const char *str, uint32_t size);

/**
 * @brief      implementation of functions from prinf family
 *
 * @param      buffer/file buffer to store result
 * @param[in]  buffer_len  the buffer length
 * @param[in]  fmt         formatting string
 * @param[in]  va          arguments to put
 *
 * @return     number of characters put into buffer
 */
int snprintf(char *buffer, unsigned int buffer_len, const char *fmt, ...);
int vsnprintf(char *buffer, unsigned int buffer_len,
    const char *fmt, va_list va);
void vfprintf(usart stdio, const char *fmt, va_list va);

/**
 * @brief      convert integer to string
 *
 * @param[in]  value      value to be converted to a string
 * @param[in]  radix      the integer radix
 * @param[in]  uppercase  use/don't use uppercase flag
 * @param[in]  unsig      signed/unsigned value flag
 * @param      buffer     the buffer to store the result
 * @param[in]  zero_pad   desired number of digits (filling extra with zero)
 *
 * @return     length of result string
 */
unsigned int itoa(int value, unsigned int radix, unsigned int uppercase,
    unsigned int unsig, char *buffer, unsigned int zero_pad);

/**
 * @brief      convert string to int
 *
 * @param      str   the string
 *
 * @return     converted integer
 */
int atoi(const char *str);

/**
 * @brief      tokenize string
 *
 * @param      str    the string
 * @param      delim  the delimiter
 *
 * @return     substring
 */
char *strtok(char *str, const char *delim);

/**
 * @brief      get span of character set in string
 *
 * @param      str1  the string 1
 * @param      str2  the string 2
 *
 * @return     length of the initial portion of str1
 * which consists only of characters that are part of str2
 */
int strspn(const char *str1,const char *str2);

/**
 * @brief      copy string
 *
 * @param      dest  the destination
 * @param      src   the source
 *
 * @return     destination is returned
 */
char * strcpy(char *dest, const char *src);

/**
 * @brief      find substring in a string
 *
 * @param[in]  str     string to be scanned
 * @param[in]  target  string to be found
 *
 * @return     pointer to the first occurance of target in str
 */
char* strstr(const char *str, const char *target);

/**
 * @brief      duplicate string
 *
 * @param      str   the string
 *
 * @return     suplicated string
 */
char *strdup(const char *src);

/**
 * @brief      append string to another
 *
 * @param      dst   the destination
 * @param      src   the source
 *
 * @return     destination is returned
 */
char *strcat(char *dst, const char *src);

/**
 * @brief      print formatted data to STDIO
 *
 * @param[in]  fmt        formatting string
 */
void printf(const char *fmt, ...);

char *strrchr(const char *s, char c);

char *strchr(const char *s, char c);

void *memchr(const void *s, int c, int n);

#endif
