//
// Created by robko on 10/1/24.
//

#ifndef ZPG_ASSERTIONS_H
#define ZPG_ASSERTIONS_H

#include <csignal>

#define DEBUG_ASSERTIONS

#ifdef DEBUG_ASSERTIONS

#define DEBUG_ASSERT(x) \
    { \
        if (!static_cast<bool>(x)) { \
            fprintf(stderr, \
                    "Assertion failed: %s. In file: %s, line %d\n", \
            #x, __FILE__, __LINE__); \
            raise(SIGABRT); \
        } \
    }

#define DEBUG_ASSERTF(x, ...) \
    { \
        if (!static_cast<bool>(x)) { \
            fprintf(stderr, \
                    "Assertion failed: %s. In file: %s, line %d\n", \
            #x, __FILE__, __LINE__); \
            fprintf(stderr, __VA_ARGS__); \
            raise(SIGABRT); \
        } \
    }

#define DEBUG_ASSERT_NOT_NULL(x) \
    { \
        if (nullptr == x) { \
            fprintf(stderr, \
                    "Assertion failed. %s is NULL. In file: %s, line %d\n", \
            #x, __FILE__, __LINE__); \
            raise(SIGABRT); \
        } \
    }

#else // DEBUG_ASSERTIONS

#define DEBUG_ASSERT_NOT_NULL(x)
#define #define DEBUG_ASSERT(x)

#endif // DEBUG_ASSERTIONS

#endif //ZPG_ASSERTIONS_H
