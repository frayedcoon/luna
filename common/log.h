#ifndef COMMON_LOG_H
#define COMMON_LOG_H

#include "lib/string.h"

#define LOG_INF(fmt, ...)   printf("[INFO]  %s::%s():%d " fmt "\n", \
                            __FILE__, __func__, __LINE__, ##__VA_ARGS__)
#define LOG_DBG(fmt, ...)   printf("[DEBUG] %s::%s():%d " fmt "\n", \
                            __FILE__, __func__, __LINE__, ##__VA_ARGS__)
#define LOG_ERR(fmt, ...)   printf("[ERROR] %s::%s():%d " fmt "\n", \
                            __FILE__, __func__, __LINE__, ##__VA_ARGS__)
#define LOG_TRACE()         printf("[TRACE] %s::%s():%d\n", __FILE__, \
                            __func__, __LINE__)

#endif
