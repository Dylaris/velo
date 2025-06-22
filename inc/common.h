#ifndef VELO_COMMON_H
#define VELO_COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define DEBUG_TRACE_STACK

#define PRIVATE static
#define PUBLIC

#define fatal(fmt, ...)                                 \
    do {                                                \
        fprintf(stderr, "ERROR: "fmt" at <%s:%d>\n",    \
            ##__VA_ARGS__, __FILE__, __LINE__);         \
        exit(1);                                        \
    } while (0)

#define unreachable(msg, ...) fatal(msg, ##__VA_ARGS__)

#endif // VELO_COMMON_H
