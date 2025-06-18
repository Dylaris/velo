#include <string.h>

#include "chunk.h"
#include "compiler.h"
#include "lexer.h"

typedef struct {
    token_t previous;
    token_t current;
    lexer_t lexer;
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
PRIVATE int consume(parser_t *parser, toktype_t type, const char *msg);
PRIVATE rule_t *get_rule(toktype_t type);

PRIVATE void parse_precedence(vm_t *vm, parser_t *parser, prec_t prec);
PRIVATE void expr(vm_t *vm, parser_t *parser);
PRIVATE void expr_number(vm_t *vm, parser_t *parser);
PRIVATE void expr_unary(vm_t *vm, parser_t *parser);
PRIVATE void expr_binary(vm_t *vm, parser_t *parser);
PRIVATE void expr_grouping(vm_t *vm, parser_t *parser);

PRIVATE void emit_byte(vm_t *vm, uint8_t byte, size_t line);
PRIVATE void emit_bytes(vm_t *vm, uint8_t byte1, uint8_t byte2, size_t line);
PRIVATE void emit_return(vm_t *vm, size_t line);
PRIVATE void emit_load(vm_t *vm, value_t value, size_t line);

PRIVATE rule_t rules[] = {
    [TOKEN_PLUS]            = {NULL, expr_binary, PREC_TERM},
    [TOKEN_MINUS]           = {expr_unary, expr_binary, PREC_TERM},
    [TOKEN_STAR]            = {NULL, expr_binary, PREC_FACTOR},
    [TOKEN_SLASH]           = {NULL, expr_binary, PREC_FACTOR},
    [TOKEN_BANG]            = {NULL, NULL, PREC_NONE},
    [TOKEN_BANG_EQUAL]      = {NULL, NULL, PREC_NONE},
    [TOKEN_EQUAL]           = {NULL, NULL, PREC_NONE},
    [TOKEN_EQUAL_EQUAL]     = {NULL, NULL, PREC_NONE},
    [TOKEN_GREATER]         = {NULL, NULL, PREC_NONE},
    [TOKEN_GREATER_EQUAL]   = {NULL, NULL, PREC_NONE},
    [TOKEN_LESS]            = {NULL, NULL, PREC_NONE},
    [TOKEN_LESS_EQUAL]      = {NULL, NULL, PREC_NONE},
    [TOKEN_LPAREN]          = {expr_grouping, NULL, PREC_NONE},
    [TOKEN_RPAREN]          = {NULL, NULL, PREC_NONE},
    [TOKEN_LBRACE]          = {NULL, NULL, PREC_NONE},
    [TOKEN_RBRACE]          = {NULL, NULL, PREC_NONE},
    [TOKEN_SEMICOLON]       = {NULL, NULL, PREC_NONE},
    [TOKEN_COMMA]           = {NULL, NULL, PREC_NONE},
    [TOKEN_DOT]             = {NULL, NULL, PREC_NONE},
    [TOKEN_NUMBER]          = {expr_number, NULL, PREC_NONE},
    [TOKEN_STRING]          = {NULL, NULL, PREC_NONE},
    [TOKEN_IDENTIFIER]      = {NULL, NULL, PREC_NONE},
    [TOKEN_VAR]             = {NULL, NULL, PREC_NONE},
    [TOKEN_RETURN]          = {NULL, NULL, PREC_NONE},
    [TOKEN_PRINT]           = {NULL, NULL, PREC_NONE},
    [TOKEN_ERROR]           = {NULL, NULL, PREC_NONE},
    [TOKEN_EOF]             = {NULL, NULL, PREC_NONE},
};

/* ====================================================== *
 *             private function implementation            *
 * ====================================================== */

PRIVATE int consume(parser_t *parser, toktype_t type, const char *msg)
{
    if (parser->current.type != type) {
        fprintf(stderr, "ERROR: %s\n", msg);
        return 1;
    }
    advance(parser);
    return 0;
}

PRIVATE void init_parser(parser_t *parser, const char *source)
{
    memset(parser, 0, sizeof(parser_t));
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
    if (!prefix_fn) fatal("get prefix rule");

    prefix_fn(vm, parser);

    while (get_rule(parser->current.type)->prec > prec) {
        advance(parser);
        parsefn_t infix_fn = get_rule(parser->previous.type)->infix;
        if (!prefix_fn) fatal("get prefix rule");
        infix_fn(vm, parser);
    }
}

PRIVATE void expr(vm_t *vm, parser_t *parser)
{
    parse_precedence(vm, parser, PREC_NONE);
}

PRIVATE void expr_number(vm_t *vm, parser_t *parser)
{
    double value = strtod(parser->previous.start, NULL);
    emit_load(vm, value, parser->previous.line);
}

PRIVATE void expr_unary(vm_t *vm, parser_t *parser)
{
    toktype_t optype = parser->previous.type;
    parse_precedence(vm, parser, PREC_UNARY);

    switch (optype) {
    case TOKEN_MINUS:
        emit_byte(vm, OP_NEG, parser->previous.line);
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

PRIVATE void emit_return(vm_t *vm, size_t line)
{
    emit_byte(vm, OP_RETURN, line);
}

PRIVATE void emit_load(vm_t *vm, value_t value, size_t line)
{
    int constant_idx = add_constant_to_chunk(&vm->chunk, value);
    emit_bytes(vm, OP_LOAD, constant_idx, line);
}

/* ====================================================== *
 *             public function implementation             *
 * ====================================================== */

PUBLIC int compile(vm_t *vm, const char *source)
{
    parser_t parser;
    init_parser(&parser, source);

    int status = 0;
    expr(vm, &parser);
    consume(&parser, TOKEN_EOF, "expected end of expression");

#if 0
    while (1) {
        token_t token = scan_token(&lexer);
        printf("<line:%04ld> %-30s %.*s\n", token.line,
                token_to_string(token), (int) token.length, token.start);
        if (token.type == TOKEN_ERROR) status = 1;
        if (token.type == TOKEN_EOF) break;
    }
#endif

    return status;
}
