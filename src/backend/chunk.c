#include <stdio.h>
#include <stdlib.h>

#include "chunk.h"

/* ====================================================== *
 *           public function implementation               *
 * ====================================================== */

PUBLIC void init_chunk(chunk_t *chunk)
{
    chunk->count    = 0;
    chunk->capacity = 0;
    chunk->codes    = NULL;
    chunk->lines    = NULL;
    init_value_pool(&chunk->constants);
}

PUBLIC void free_chunk(chunk_t *chunk)
{
    if (chunk->codes) free(chunk->codes);
    if (chunk->lines) free(chunk->lines);
    free_value_pool(&chunk->constants);
    init_chunk(chunk);
}

PUBLIC void write_code_to_chunk(chunk_t *chunk, uint8_t byte, size_t line)
{
    if (chunk->capacity <= chunk->count) {
        chunk->capacity = (chunk->capacity==0) ? 10 : 2*chunk->capacity;
        chunk->codes = realloc(chunk->codes, chunk->capacity*sizeof(uint8_t));
        if (!chunk->codes) fatal("out of memory");
        chunk->lines = realloc(chunk->lines, chunk->capacity*sizeof(size_t));
        if (!chunk->lines) fatal("out of memory");
    }

    chunk->codes[chunk->count] = byte;
    chunk->lines[chunk->count] = line;
    chunk->count++;
}

PUBLIC uint8_t add_constant_to_chunk(chunk_t *chunk, value_t value)
{
    add_value_to_pool(&chunk->constants, value);
    return (uint8_t) (chunk->constants.count - 1);
}
