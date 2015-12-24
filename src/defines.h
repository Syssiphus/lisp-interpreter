#pragma once

/* DEBUGGING */
#ifdef _DEBUG
#define DEBUG_BREAK __builtin_trap()
#else 
#define DEBUG_BREAK do {} while(0)
#endif
#define DEBUGGING_ON
#ifdef DEBUGGING_ON
#define dbg_pos() fprintf(stderr, "%s, %d, %s()\n", __FILE__, __LINE__, \
        __func__);
#define dbg_print(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__);
#else
#define dbg_pos()
#define dbg_print(fmt, ...) 
#endif

/* Unused parameter marker */
#define UNUSED(x) (void)(x)

/* static inline declaration */
#define _static_inline_ __attribute__((always_inline)) static inline
