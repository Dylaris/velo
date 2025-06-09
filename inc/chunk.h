#ifndef VELO_CHUNK_H
#define VELO_CHUNK_H

#include "common.h"
#include "value.h"

/* 
 * opcode format
 * OP_RETURN: [ OP_RETURN (1) | exit_code (1)       ]
 * OP_LOAD:   [ OP_LOAD (1)   | constant_idx (1)    ]
 */

typedef enum {
    OP_LOAD,
    OP_RETURN,
} opcode_t;

typedef union {
    uint8_t status;
    uint8_t index;
} operand_t;

typedef struct {
    opcode_t opcode;
    operand_t operand;
} inst_t;

typedef struct {
    size_t count;
    size_t capacity;
    uint8_t *codes;
    size_t *lines;
    value_pool_t constants;
} chunk_t;

PUBLIC void init_chunk(chunk_t *chunk);
PUBLIC void free_chunk(chunk_t *chunk);
PUBLIC void write_code_to_chunk(chunk_t *chunk, uint8_t byte, size_t line);
PUBLIC void dump_chunk(chunk_t *chunk);
PUBLIC uint8_t add_constant_to_chunk(chunk_t *chunk, value_t value);

#endif // VELO_CHUNK_H
