#ifndef VELO_CHUNK_H
#define VELO_CHUNK_H

#include "common.h"
#include "value.h"

/* 
 * opcode format
 * (n) means n bytes
 * OP_RETURN:   [ OP_RETURN (1)                     ]
 * OP_LOAD:     [ OP_LOAD (1)   | constant_idx (1)  ]
 * OP_NEG:      [ OP_NEG (1)                        ]
 * OP_ADD:      [ OP_ADD (1)                        ]
 * OP_SUB:      [ OP_SUB (1)                        ]
 * OP_MUL:      [ OP_MUL (1)                        ]
 * OP_DIV:      [ OP_DIV (1)                        ]
 * OP_NOT:      [ OP_NOT (1)                        ]
 * OP_EQUAL:    [ OP_EQUAL (1)                      ]
 * OP_GREATER:  [ OP_GREATER (1)                    ]
 * OP_LESS:     [ OP_LESS(1)                        ]
 * OP_TRUE:     [ OP_TRUE (1)                       ]
 * OP_FALSE:    [ OP_FALSE (1)                      ]
 * OP_NIL:      [ OP_NIL (1)                        ]
 */

typedef enum {
    OP_LOAD,
    OP_RETURN,
    OP_NEG,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_NOT,
    OP_EQUAL,
    OP_GREATER,
    OP_LESS,
    OP_TRUE,
    OP_FALSE,
    OP_NIL,
} opcode_t;

typedef union {
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
    valpool_t constants;
} chunk_t;

PUBLIC void init_chunk(chunk_t *chunk);
PUBLIC void free_chunk(chunk_t *chunk);
PUBLIC void write_code_to_chunk(chunk_t *chunk, uint8_t byte, size_t line);
PUBLIC uint8_t add_constant_to_chunk(chunk_t *chunk, value_t value);

#endif // VELO_CHUNK_H
