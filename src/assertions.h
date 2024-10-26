//
// Created by robko on 10/1/24.
//

#ifndef ZPG_ASSERTIONS_H
#define ZPG_ASSERTIONS_H

#include <csignal>
#include <cstdio>
#include <stacktrace>
#include <iostream>
#define BACKWARD_HAS_BFD 1
#include <backward.hpp>

#define DEBUG_ASSERTIONS

void printCurrentStacktrace() {
    backward::StackTrace st;
    st.load_here(32);
    backward::Printer p;
    p.print(st);
}

#ifdef DEBUG_ASSERTIONS

#define DEBUG_ASSERT(x) \
    { \
        if (!static_cast<bool>(x)) { \
            fprintf(stderr, \
                    "Assertion failed: %s. In file: %s, line %d\n", \
            #x, __FILE__, __LINE__); \
            printCurrentStacktrace(); \
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
            printCurrentStacktrace(); \
            raise(SIGABRT); \
        } \
    }

#define DEBUG_ASSERT_NOT_NULL(x) \
    { \
        if (nullptr == x) { \
            fprintf(stderr, \
                    "Assertion failed. %s is NULL. In file: %s, line %d\n", \
            #x, __FILE__, __LINE__); \
            printCurrentStacktrace(); \
            raise(SIGABRT); \
        } \
    }

#else // DEBUG_ASSERTIONS

#define DEBUG_ASSERT_NOT_NULL(x)
#define #define DEBUG_ASSERT(x)

#endif // DEBUG_ASSERTIONS

#endif //ZPG_ASSERTIONS_H
