#include "vm.h"
#include "compiler.h"
#ifdef DEBUG_TRACE_STACK
#include "debug.h"
#endif

#define READ_BYTE(vm)           (*(vm)->pc++)
#define READ_CONSTANT(vm, idx)  ((vm)->chunk.constants.values[(idx)])
#define RESET_STACK(vm)         ((vm)->sp = (vm)->ss)
#define PUSH(vm, value)         (*(vm)->sp++ = (value))
#define POP(vm)                 (*(--(vm)->sp))
#define UNARY_OP(vm, op)        \
    do {                        \
        value_t a = POP(vm);    \
        PUSH(vm, op a);         \
    } while (0)
#define BINARY_OP(vm, op)       \
    do {                        \
        value_t b = POP(vm);    \
        value_t a = POP(vm);    \
        PUSH(vm, a op b);       \
    } while (0)

/* ====================================================== *
 *           private function declaration                 *
 * ====================================================== */

PRIVATE inst_t read_instruction(vm_t *vm);
PRIVATE char *opcode_to_string(opcode_t opcode);
PRIVATE int run(vm_t *vm);

/* ====================================================== *
 *           private function implementation              *
 * ====================================================== */

PRIVATE int run(vm_t *vm)
{
    vm->pc = vm->chunk.codes;

#ifdef DEBUG_TRACE_STACK
        printf(">> DEBUG TRACE STACK <<\n");
#endif

    while (vm->pc < vm->chunk.codes + vm->chunk.count) {
        inst_t inst = read_instruction(vm);

#ifdef DEBUG_TRACE_STACK
        size_t offset = vm->pc - vm->chunk.codes;
#endif

        switch (inst.opcode) {
        case OP_LOAD: {
            value_t value = READ_CONSTANT(vm, inst.operand.index);
            PUSH(vm, value);
        } break;

        case OP_RETURN: break;

        case OP_NEG: UNARY_OP(vm, -);  break;
        case OP_ADD: BINARY_OP(vm, +); break;
        case OP_SUB: BINARY_OP(vm, -); break; 
        case OP_MUL: BINARY_OP(vm, *); break; 
        case OP_DIV: BINARY_OP(vm, /); break; 

        default: return 1;
        }

#ifdef DEBUG_TRACE_STACK
        printf("[%04ld] <line:%02ld> =opcode=: %s\n", offset,
            vm->chunk.lines[offset], opcode_to_string(inst.opcode));
        dump_stack(vm->ss, vm->sp - vm->ss);
#endif
    }

#ifdef DEBUG_TRACE_STACK
    printf("\n");
#endif

    return 0;
}

PRIVATE inst_t read_instruction(vm_t *vm)
{
    inst_t res = {0};
    res.opcode = READ_BYTE(vm);

    switch (res.opcode) {
    case OP_LOAD:
        res.operand.index  = READ_BYTE(vm);
        break;

    case OP_RETURN:
    case OP_NEG:
    case OP_ADD:
    case OP_SUB:
    case OP_MUL:
    case OP_DIV:
        break;

    default:
        unreachable("unknown opcode");
    }

    return res;
}

PRIVATE char *opcode_to_string(opcode_t opcode)
{
    switch (opcode) {
    case OP_LOAD:   return "OP_LOAD";
    case OP_RETURN: return "OP_RETURN";
    case OP_NEG:    return "OP_NEG";
    case OP_ADD:    return "OP_ADD"; 
    case OP_SUB:    return "OP_SUB";
    case OP_MUL:    return "OP_MUL";
    case OP_DIV:    return "OP_DIV";
    default:        unreachable("unknown opcode");
    }
}

/* ====================================================== *
 *           public function implementation               *
 * ====================================================== */

PUBLIC void init_vm(vm_t *vm)
{
    init_chunk(&vm->chunk);
    RESET_STACK(vm);
}

PUBLIC void free_vm(vm_t *vm)
{
    free_chunk(&vm->chunk);
    init_vm(vm);
}

PUBLIC status_t interpret(vm_t *vm, const char *source)
{
    if (!source) return INTERPRET_OK;
    if (compile(vm, source) != 0) return INTERPRET_COMPILE_ERROR;
    if (run(vm) != 0) return INTERPRET_RUNTIME_ERROR;
    return INTERPRET_OK;
}

#undef READ_BYTE
#undef READ_CONSTANT
#undef RESET_STACK
#undef PUSH
#undef POP
