#include <string.h>

#include "chunk.h"
#include "compiler.h"
#include "lexer.h"
#include "object.h"

typedef struct {
    token_t previous;
    token_t current;
    lexer_t lexer;
    bool had_error;
    bool panic_mode;
} parser_t;

typedef enum {
    PREC_NONE,
    PREC_ASSIGN,    // =
    PREC_EQUAL,     // == !=
    PREC_CMP,       // > < >= <=
    PREC_TERM,      // + -
    PREC_FACTOR,    // * /
    PREC_UNARY,     // - !
} prec_t;

typedef void (*parsefn_t)(vm_t *, parser_t *);

typedef struct {
    parsefn_t prefix;
    parsefn_t infix;
    prec_t prec;
} rule_t;

/* ====================================================== *
 *             private function declaration               *
 * ====================================================== */

PRIVATE void init_parser(parser_t *parser, const char *source);
PRIVATE void advance(parser_t *parser);
PRIVATE void consume(parser_t *parser, toktype_t type, const char *msg);
PRIVATE rule_t *get_rule(toktype_t type);
PRIVATE void error_at_current(parser_t *parser, const char *msg);
PRIVATE void error(parser_t *parser, token_t *token, const char *msg);

PRIVATE void parse_precedence(vm_t *vm, parser_t *parser, prec_t prec);
PRIVATE void expr(vm_t *vm, parser_t *parser);
PRIVATE void expr_number(vm_t *vm, parser_t *parser);
PRIVATE void expr_string(vm_t *vm, parser_t *parser);
PRIVATE void expr_literal(vm_t *vm, parser_t *parser);
PRIVATE void expr_unary(vm_t *vm, parser_t *parser);
PRIVATE void expr_binary(vm_t *vm, parser_t *parser);
PRIVATE void expr_grouping(vm_t *vm, parser_t *parser);

PRIVATE void emit_byte(vm_t *vm, uint8_t byte, size_t line);
PRIVATE void emit_bytes(vm_t *vm, uint8_t byte1, uint8_t byte2, size_t line);
PRIVATE void emit_load(vm_t *vm, value_t value, size_t line);

PRIVATE rule_t rules[] = {
    [TOKEN_PLUS]            = {NULL, expr_binary, PREC_TERM},
    [TOKEN_MINUS]           = {expr_unary, expr_binary, PREC_TERM},
    [TOKEN_STAR]            = {NULL, expr_binary, PREC_FACTOR},
    [TOKEN_SLASH]           = {NULL, expr_binary, PREC_FACTOR},
    [TOKEN_BANG]            = {expr_unary, NULL, PREC_NONE},
    [TOKEN_BANG_EQUAL]      = {NULL, expr_binary, PREC_EQUAL},
    [TOKEN_EQUAL]           = {NULL, NULL, PREC_NONE},
    [TOKEN_EQUAL_EQUAL]     = {NULL, expr_binary, PREC_EQUAL},
    [TOKEN_GREATER]         = {NULL, expr_binary, PREC_CMP},
    [TOKEN_GREATER_EQUAL]   = {NULL, expr_binary, PREC_CMP},
    [TOKEN_LESS]            = {NULL, expr_binary, PREC_CMP},
    [TOKEN_LESS_EQUAL]      = {NULL, expr_binary, PREC_CMP},
    [TOKEN_LPAREN]          = {expr_grouping, NULL, PREC_NONE},
    [TOKEN_RPAREN]          = {NULL, NULL, PREC_NONE},
    [TOKEN_LBRACE]          = {NULL, NULL, PREC_NONE},
    [TOKEN_RBRACE]          = {NULL, NULL, PREC_NONE},
    [TOKEN_SEMICOLON]       = {NULL, NULL, PREC_NONE},
    [TOKEN_COMMA]           = {NULL, NULL, PREC_NONE},
    [TOKEN_DOT]             = {NULL, NULL, PREC_NONE},
    [TOKEN_NUMBER]          = {expr_number, NULL, PREC_NONE},
    [TOKEN_STRING]          = {expr_string, NULL, PREC_NONE},
    [TOKEN_IDENTIFIER]      = {NULL, NULL, PREC_NONE},
    [TOKEN_VAR]             = {NULL, NULL, PREC_NONE},
    [TOKEN_RETURN]          = {NULL, NULL, PREC_NONE},
    [TOKEN_PRINT]           = {NULL, NULL, PREC_NONE},
    [TOKEN_TRUE]            = {expr_literal, NULL, PREC_NONE},
    [TOKEN_FALSE]           = {expr_literal, NULL, PREC_NONE},
    [TOKEN_NIL]             = {expr_literal, NULL, PREC_NONE},
    [TOKEN_ERROR]           = {NULL, NULL, PREC_NONE},
    [TOKEN_EOF]             = {NULL, NULL, PREC_NONE},
};

/* ====================================================== *
 *             private function implementation            *
 * ====================================================== */

PRIVATE void error_at_current(parser_t *parser, const char *msg)
{
    error(parser, &parser->current, msg);
}

PRIVATE void error(parser_t *parser, token_t *token, const char *msg)
{
    /* Compile time error */

    if (parser->panic_mode) return;

    parser->panic_mode = true;

    fprintf(stderr, "<CT> [line: %04ld] ERROR: %s ", token->line, msg);
    if (token->type == TOKEN_EOF) {
        fprintf(stderr, "at end\n");
    } else if (token->type == TOKEN_ERROR) {
        /* Nothing */
    } else {
        fprintf(stderr, "at '%*.s'\n", (int) token->length, token->start);
    }

    parser->had_error = true;
}

PRIVATE void consume(parser_t *parser, toktype_t type, const char *msg)
{
    if (parser->current.type != type) {
        error_at_current(parser, msg);
    }
    advance(parser);
}

PRIVATE void init_parser(parser_t *parser, const char *source)
{
    memset(parser, 0, sizeof(parser_t));
    parser->had_error = false;
    parser->panic_mode = false;
    init_lexer(&parser->lexer, source);
    advance(parser); // force parser->current to point to first token
}

PRIVATE rule_t *get_rule(toktype_t type)
{
    return &rules[type];
}

PRIVATE void advance(parser_t *parser)
{
    parser->previous = parser->current;
    parser->current = scan_token(&parser->lexer);
}

PRIVATE void parse_precedence(vm_t *vm, parser_t *parser, prec_t prec)
{
    advance(parser);
    parsefn_t prefix_fn = get_rule(parser->previous.type)->prefix;
    if (!prefix_fn) {
        error_at_current(parser, "get prefix rule");
        return;
    }

    prefix_fn(vm, parser);

    while (get_rule(parser->current.type)->prec > prec) {
        advance(parser);
        parsefn_t infix_fn = get_rule(parser->previous.type)->infix;
        if (!infix_fn) {
            error_at_current(parser, "get infix rule");
            return;
        }
        infix_fn(vm, parser);
    }
}

PRIVATE void expr(vm_t *vm, parser_t *parser)
{
    parse_precedence(vm, parser, PREC_ASSIGN);
}

PRIVATE void expr_literal(vm_t *vm, parser_t *parser)
{
    token_t tk = parser->previous;
    switch (tk.type) {
    case TOKEN_TRUE:  emit_byte(vm, OP_TRUE,  tk.line); break;
    case TOKEN_FALSE: emit_byte(vm, OP_FALSE, tk.line); break;
    case TOKEN_NIL:   emit_byte(vm, OP_NIL,   tk.line); break;
    default:          unreachable("unknown type");
    }
}

PRIVATE void expr_number(vm_t *vm, parser_t *parser)
{
    value_t value = PACK_NUMBER(strtod(parser->previous.start, NULL));
    emit_load(vm, value, parser->previous.line);
}

PRIVATE void expr_string(vm_t *vm, parser_t *parser)
{
    /* Skip left '"' and right '"' */
    emit_load(vm, PACK_OBJECT(copy_string(vm, parser->previous.start + 1,
                    parser->previous.length - 2)), parser->previous.line);
}

PRIVATE void expr_unary(vm_t *vm, parser_t *parser)
{
    toktype_t optype = parser->previous.type;
    parse_precedence(vm, parser, PREC_UNARY);

    switch (optype) {
    case TOKEN_MINUS:
        emit_byte(vm, OP_NEG, parser->previous.line);
        break;
    case TOKEN_BANG:
        emit_byte(vm, OP_NOT, parser->previous.line);
        break;
    default:
        unreachable("expr_unary()");
    }
}

PRIVATE void expr_binary(vm_t *vm, parser_t *parser)
{
    toktype_t optype = parser->previous.type;
    parse_precedence(vm, parser, get_rule(optype)->prec);

    switch (optype) {
    case TOKEN_MINUS:
        emit_byte(vm, OP_SUB, parser->previous.line);
        break;
    case TOKEN_PLUS:
        emit_byte(vm, OP_ADD, parser->previous.line);
        break;
    case TOKEN_STAR:
        emit_byte(vm, OP_MUL, parser->previous.line);
        break;
    case TOKEN_SLASH:
        emit_byte(vm, OP_DIV, parser->previous.line);
        break;
    case TOKEN_BANG_EQUAL:
        emit_bytes(vm, OP_EQUAL, OP_NOT, parser->previous.line);
        break;
    case TOKEN_EQUAL_EQUAL:
        emit_byte(vm, OP_EQUAL, parser->previous.line);
        break;
    case TOKEN_GREATER:
        emit_byte(vm, OP_GREATER, parser->previous.line);
        break;
    case TOKEN_GREATER_EQUAL:
        emit_bytes(vm, OP_LESS, OP_NOT, parser->previous.line);
        break;
    case TOKEN_LESS:
        emit_byte(vm, OP_LESS, parser->previous.line);
        break;
    case TOKEN_LESS_EQUAL:
        emit_bytes(vm, OP_GREATER, OP_NOT, parser->previous.line);
        break;
    default:
        unreachable("expr_binary()");
    }
}

PRIVATE void expr_grouping(vm_t *vm, parser_t *parser)
{
    expr(vm, parser);
    consume(parser, TOKEN_RPAREN, "lack of ')'");
}

PRIVATE void emit_byte(vm_t *vm, uint8_t byte, size_t line)
{
    write_code_to_chunk(&vm->chunk, byte, line);
}

PRIVATE void emit_bytes(vm_t *vm, uint8_t byte1, uint8_t byte2, size_t line)
{
    emit_byte(vm, byte1, line);
    emit_byte(vm, byte2, line);
}

PRIVATE void emit_load(vm_t *vm, value_t value, size_t line)
{
    int constant_idx = add_constant_to_chunk(&vm->chunk, value);
    emit_bytes(vm, OP_LOAD, constant_idx, line);
}

/* ====================================================== *
 *             public function implementation             *
 * ====================================================== */

PUBLIC bool compile(vm_t *vm, const char *source)
{
#if 1
    parser_t parser;
    init_parser(&parser, source);

    expr(vm, &parser);
    consume(&parser, TOKEN_EOF, "expected end of expression");
#else
    (void) vm;

    lexer_t lexer;
    init_lexer(&lexer, source);

    while (1) {
        token_t token = scan_token(&lexer);
        printf("<line:%04ld> %-30s %.*s\n", token.line,
                token_to_string(token), (int) token.length, token.start);
        if (token.type == TOKEN_EOF) break;
    }

    printf("\n\n");
#endif

    return !parser.had_error;
}
