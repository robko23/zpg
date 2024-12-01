//
// Created by robko on 10/1/24.
//

#ifndef ZPG_ASSERTIONS_H
#define ZPG_ASSERTIONS_H

#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <stacktrace>
#define BACKWARD_HAS_BFD 1
#include <backward.hpp>

#define DEBUG_ASSERTIONS

inline void printCurrentStacktrace() {
    backward::StackTrace st;
    st.load_here(32);
    backward::Printer p;
    p.print(st);
}

#ifdef DEBUG_ASSERTIONS

#define DEBUG_ASSERT(x)                                                        \
    {                                                                          \
        if (!static_cast<bool>(x)) {                                           \
            fprintf(stderr, "Assertion failed: %s. In file: %s, line %d\n",    \
                    #x, __FILE__, __LINE__);                                   \
            printCurrentStacktrace();                                          \
            abort();                                                           \
        }                                                                      \
    }

#define DEBUG_ASSERTF(x, ...)                                                  \
    {                                                                          \
        if (!static_cast<bool>(x)) {                                           \
            fprintf(stderr, "Assertion failed: %s. In file: %s, line %d\n",    \
                    #x, __FILE__, __LINE__);                                   \
            fprintf(stderr, __VA_ARGS__);                                      \
            fprintf(stderr, "\n");                                             \
            printCurrentStacktrace();                                          \
            abort();                                                           \
        }                                                                      \
    }

#define DEBUG_ASSERT_NOT_NULL(x)                                               \
    {                                                                          \
        if (nullptr == x) {                                                    \
            fprintf(stderr,                                                    \
                    "Assertion failed. %s is NULL. In file: %s, line %d\n",    \
                    #x, __FILE__, __LINE__);                                   \
            printCurrentStacktrace();                                          \
            abort();                                                           \
        }                                                                      \
    }

#define UNREACHABLE(...)                                                       \
    {                                                                          \
        fprintf(stderr, "Unreachable reached. In file: %s, line %d\n",         \
                __FILE__, __LINE__);                                           \
        fprintf(stderr, __VA_ARGS__);                                          \
        fprintf(stderr, "\n");                                                 \
        printCurrentStacktrace();                                              \
        abort();                                                               \
    }

#define TODO()                                                                 \
    {                                                                          \
        fprintf(stderr, "Not yet implemented. In file: %s, line %d\n",         \
                __FILE__, __LINE__);                                           \
        printCurrentStacktrace();                                              \
        abort();                                                               \
    }

#define TODOF(...)                                                             \
    {                                                                          \
        fprintf(stderr, "Not yet implemented. In file: %s, line %d\n",         \
                __FILE__, __LINE__);                                           \
        fprintf(stderr, __VA_ARGS__);                                          \
        fprintf(stderr, "\n");                                                 \
        printCurrentStacktrace();                                              \
        abort();                                                               \
    }

#else // DEBUG_ASSERTIONS

#define DEBUG_ASSERT_NOT_NULL(x)
#define #define DEBUG_ASSERT(x)

#endif // DEBUG_ASSERTIONS

#endif // ZPG_ASSERTIONS_H
