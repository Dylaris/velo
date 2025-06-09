#ifndef VELO_DEBUG_H
#define VELO_DEBUG_H

#include "common.h"
#include "chunk.h"

PUBLIC void disasm_chunk(chunk_t *chunk, const char *name);
PUBLIC size_t disasm_instruction(chunk_t *chunk, size_t offset);

#endif // VELO_DEBUG_H
