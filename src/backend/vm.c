#include <stdarg.h>
#include <string.h>

#include "object.h"
#include "vm.h"
#include "compiler.h"
#ifdef DEBUG_TRACE_STACK
#include "debug.h"
#endif

#define READ_BYTE(vm)           (*(vm)->pc++)
#define READ_CONSTANT(vm, idx)  ((vm)->chunk.constants.values[(idx)])
#define RESET_STACK(vm)         ((vm)->sp = (vm)->ss)
#define BINARY_OP(pack, vm, op)                                     \
    do {                                                            \
        if (!IS_NUMBER(peek(vm, 0)) || !IS_NUMBER(peek(vm, 1))) {   \
            error(vm, "operands must be numbers");                  \
            return false;                                           \
        }                                                           \
        double b = UNPACK_NUMBER(pop(vm));                          \
        double a = UNPACK_NUMBER(pop(vm));                          \
        push(vm, pack(a op b));                                     \
    } while (0)

/* ====================================================== *
 *           private function declaration                 *
 * ====================================================== */

PRIVATE inst_t read_instruction(vm_t *vm);
PRIVATE char *opcode_to_string(opcode_t opcode);
PRIVATE bool run(vm_t *vm);
PRIVATE void error(vm_t *vm, const char *fmt, ...);
PRIVATE void concat(vm_t *vm);
PRIVATE void free_objects(object_t *objs);
/* The push/pop/peek operations are frequently used, 
   and using them as macros can result in multiple 
   evaluations during macro expansion. */
PRIVATE void push(vm_t *vm, value_t value);
PRIVATE value_t pop(vm_t *vm);
PRIVATE value_t peek(vm_t *vm, int dist);
PRIVATE bool is_falsey(value_t value);

/* ====================================================== *
 *           private function implementation              *
 * ====================================================== */

PRIVATE void free_objects(object_t *objs)
{
    object_t *cur = objs;
    while (cur) {
        object_t *next = cur->next;
        free_object(cur);
        cur = next;
    }
}

PRIVATE void concat(vm_t *vm)
{
    string_t *b = UNPACK_STRING(pop(vm));
    string_t *a = UNPACK_STRING(pop(vm));

    /* We can't just modified a or b because of 
       'string internaling' */
    size_t len = a->len + b->len;
    char *chars = malloc(len + 1);
    memcpy(chars, a->chars, a->len);
    memcpy(chars + a->len, b->chars, b->len);
    chars[len] = '\0';

    string_t *res = take_string(vm, chars, len);
    push(vm, PACK_OBJECT(res));
}

PRIVATE bool is_falsey(value_t value)
{
    return IS_NIL(value) || (IS_BOOLEAN(value) && !UNPACK_BOOLEAN(value));
}

PRIVATE value_t peek(vm_t *vm, int dist)
{
    return vm->sp[-1 - dist];
}

PRIVATE value_t pop(vm_t *vm)
{
    return *(--vm->sp);
}

PRIVATE void push(vm_t *vm, value_t value)
{
    *vm->sp++ = value;
}

PRIVATE void error(vm_t *vm, const char *fmt, ...)
{
    size_t off = vm->pc - vm->chunk.codes - 1;
    size_t line = vm->chunk.lines[off];
    fprintf(stderr, "<RT> [line %04ld] ERROR: ", line);

    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "\n");

    RESET_STACK(vm);
}

PRIVATE bool run(vm_t *vm)
{
    vm->pc = vm->chunk.codes;

#ifdef DEBUG_TRACE_STACK
        printf(">> DEBUG TRACE STACK <<\n");
#endif

    while (vm->pc < vm->chunk.codes + vm->chunk.count) {
#ifdef DEBUG_TRACE_STACK
        size_t offset = vm->pc - vm->chunk.codes;
#endif

        inst_t inst = read_instruction(vm);

        switch (inst.opcode) {
        case OP_LOAD: {
            value_t value = READ_CONSTANT(vm, inst.operand.index);
            push(vm, value);
            break;
        }
        case OP_RETURN: break;
        case OP_NEG: {
            if (!IS_NUMBER(peek(vm, 0))) {
                error(vm, "operand must be number");
                return false;
            }
            double a = UNPACK_NUMBER(pop(vm));
            push(vm, PACK_NUMBER(-a));
            break;
        }
        case OP_ADD: {
            if (IS_STRING(peek(vm, 0)) && IS_STRING(peek(vm, 1))) {
                concat(vm);
            } else if (IS_NUMBER(peek(vm, 0)) && IS_NUMBER(peek(vm, 1))) {
                double b = UNPACK_NUMBER(pop(vm));
                double a = UNPACK_NUMBER(pop(vm));
                push(vm, PACK_NUMBER(a + b));
            } else {
                error(vm, "operands must be two numbers or two strings");
                return false;
            }
            break;
        }
        case OP_SUB: BINARY_OP(PACK_NUMBER, vm, -); break; 
        case OP_MUL: BINARY_OP(PACK_NUMBER, vm, *); break; 
        case OP_DIV: BINARY_OP(PACK_NUMBER, vm, /); break; 
        case OP_NOT: push(vm, PACK_BOOLEAN(is_falsey(pop(vm)))); break;
        case OP_EQUAL: {
            value_t b = pop(vm);
            value_t a = pop(vm);
            push(vm, PACK_BOOLEAN(values_equal(a, b)));
            break;
        }
        case OP_GREATER: BINARY_OP(PACK_BOOLEAN, vm, > ); break;
        case OP_LESS:    BINARY_OP(PACK_BOOLEAN, vm, < ); break;
        case OP_TRUE:  push(vm, PACK_BOOLEAN(true));  break;
        case OP_FALSE: push(vm, PACK_BOOLEAN(false)); break;
        case OP_NIL:   push(vm, PACK_NIL(0));         break;
        default: return false;
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

    return true;
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
    case OP_NOT:
    case OP_EQUAL:
    case OP_GREATER:
    case OP_LESS:
    case OP_TRUE:
    case OP_FALSE:
    case OP_NIL:
        break;

    default:
        unreachable("unknown opcode");
    }

    return res;
}

PRIVATE char *opcode_to_string(opcode_t opcode)
{
    switch (opcode) {
    case OP_LOAD:       return "OP_LOAD";
    case OP_RETURN:     return "OP_RETURN";
    case OP_NEG:        return "OP_NEG";
    case OP_ADD:        return "OP_ADD"; 
    case OP_SUB:        return "OP_SUB";
    case OP_MUL:        return "OP_MUL";
    case OP_DIV:        return "OP_DIV";
    case OP_NOT:        return "OP_NOT";
    case OP_EQUAL:      return "OP_EQUAL";
    case OP_GREATER:    return "OP_GREATER";
    case OP_LESS:       return "OP_LESS";
    case OP_TRUE:       return "OP_TRUE";
    case OP_FALSE:      return "OP_FALSE";
    case OP_NIL:        return "OP_NIL";
    default:            unreachable("unknown opcode");
    }
}

/* ====================================================== *
 *           public function implementation               *
 * ====================================================== */

PUBLIC void init_vm(vm_t *vm)
{
    init_chunk(&vm->chunk);
    RESET_STACK(vm);
    vm->objects = NULL;
    init_table(&vm->strings);
}

PUBLIC void free_vm(vm_t *vm)
{
    free_chunk(&vm->chunk);
    free_objects(vm->objects);
    free_table(&vm->strings);
    init_vm(vm);
}

PUBLIC status_t interpret(vm_t *vm, const char *source)
{
    if (!source) return INTERPRET_OK;
    if (!compile(vm, source)) return INTERPRET_COMPILE_ERROR;
    if (!run(vm)) return INTERPRET_RUNTIME_ERROR;
    return INTERPRET_OK;
}

#undef READ_BYTE
#undef READ_CONSTANT
#undef RESET_STACK
