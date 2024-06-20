#ifndef COMMON_UTILS_H
#define COMMON_UTILS_H

#ifndef countof
#define countof(array)                  (sizeof(array)/sizeof(array[0]))
#endif

#ifndef offsetof
#define offsetof(type, member)          __builtin_offsetof(type, member)
#endif

#ifndef containerof
#define containerof(ptr, type, member) \
    ((type *)((uint8_t *)(ptr) - offsetof(type, member)))
#endif

#endif
