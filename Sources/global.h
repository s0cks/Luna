#ifndef LUNA_GLOBAL_H
#define LUNA_GLOBAL_H

#include <stdint.h>

typedef intptr_t word;
typedef uintptr_t uword;

static int kWordSize = sizeof(word);

template<typename T> static inline bool IsInt(int N, T value){
    T limit = static_cast<T>(1) << (N - 1);
    return (-limit <= value) && (value < limit);
}

template<typename T>static inline bool IsUint(int N, T value){
    T limit = static_cast<T>(1) << N;
    return (0 <= value) && (value < limit);
}

#if defined(_M_X64) || defined(__x86_64__)
#define ARCH_IS_X64 1
#elif defined(_M_IX86) || defined(__i386__)
#define ARCH_IS_X32 1
#else
#error "Cannot determine CPU architecture"
#endif

#endif