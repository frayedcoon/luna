#ifndef COMMON_YPES_H
#define COMMON_TYPES_H

#define INT_MAX            __INT_MAX__
#define INT_MIN            ~(__INT_MAX__ - 1)

#define NULL               ((void*) 0)

#define ASSERT(expr) \
if (!(expr)) {\
    printf("Assertion at %s:%d",__FILE__, __LINE__); \
    while(1); \
}

#define STATIC_ASSERT(cond) typedef char static_assertion_##cond[(cond)?1:-1]

#endif
