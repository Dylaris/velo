#ifndef VELO_COMMON_H
#define VELO_COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define PRIVATE static
#define PUBLIC

#define fatal(msg)                                              \
    do {                                                        \
        fprintf(stderr, "ERROR: %s at <%s:%d>\n",               \
                (msg) ? (msg) : "(null)", __FILE__, __LINE__);  \
        exit(1);                                                \
    } while (0)

#define unreachable(msg) fatal(msg)

#endif // VELO_COMMON_H
