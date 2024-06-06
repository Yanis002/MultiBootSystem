#ifndef _MACROS_H
#define _MACROS_H

// The VERSION macro will be set to one of these version numbers.
#define MQ_J 1
#define MQ_U 2
#define MQ_E 3
#define CE_J 4
#define CE_U 5
#define CE_E 6
#define IS_MQ (VERSION == MQ_J || VERSION == MQ_U || VERSION == MQ_E)
#define IS_CE (VERSION == CE_J || VERSION == CE_U || VERSION == CE_E)

#define ALIGN_PREV(X, N) ((X) & ~((N) - 1))
#define ALIGN_NEXT(X, N) ALIGN_PREV(((X) + (N) - 1), N)

#define ARRAY_COUNT(arr) (s32)(sizeof(arr) / sizeof(arr[0]))
#define ARRAY_COUNTU(arr) (u32)(sizeof(arr) / sizeof(arr[0]))

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) < (b) ? (b) : (a))
#define SQ(x) ((x) * (x))
#define CLAMP(x, l, h) (((x) > (h)) ? (h) : (((x) < (l)) ? (l) : (x)))

// Adds no-ops to increase a function's size, preventing automatic inlining
#define NO_INLINE() \
    (void)0;        \
    (void)0;        \
    (void)0;        \
    (void)0;        \
    (void)0;        \
    (void)0;        \
    (void)0;        \
    (void)0;        \
    (void)0;        \
    (void)0;        \
    (void)0;        \
    (void)0;        \
    (void)0;        \
    (void)0

// Adds a stack variable in an inline function, which can be used to pad the
// stack after other functions have been inlined
inline void padStack(void) { int pad = 0; }
#define PAD_STACK() padStack()

#ifndef __INTELLISENSE__
#define GLUE(a, b) a##b
#define GLUE2(a, b) GLUE(a, b)
#define STATIC_ASSERT(cond) typedef char GLUE2(static_assertion_failed, __LINE__)[(cond) ? 1 : -1]
#define ATTRIBUTE_ALIGN(num) __attribute__((aligned(num)))
#else
#define STATIC_ASSERT(...)
#define ATTRIBUTE_ALIGN(...)
#endif

#ifdef __MWERKS__
#define ASM asm
#define WEAK __declspec(weak)
#define INIT __declspec(section ".init")
#define CTORS __declspec(section ".ctors")
#define DTORS __declspec(section ".dtors")
#define AT_ADDRESS(xyz) : (xyz)
#else
#define ASM
#define WEAK
#define INIT
#define CTORS
#define DTORS
#define AT_ADDRESS(xyz)
#endif

#define JUT_EXPECT(...)
#define ASSERT(...)
#define LOGF(FMT, ...)

#endif
