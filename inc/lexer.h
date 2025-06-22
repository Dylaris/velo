#ifndef VELO_LEXER_H
#define VELO_LEXER_H

#include "common.h"

typedef enum {
    /* Operator */
    TOKEN_PLUS, TOKEN_MINUS, TOKEN_STAR, TOKEN_SLASH,
    TOKEN_BANG, TOKEN_BANG_EQUAL, TOKEN_EQUAL, TOKEN_EQUAL_EQUAL,
    TOKEN_GREATER, TOKEN_GREATER_EQUAL, TOKEN_LESS, TOKEN_LESS_EQUAL,

    /* Parentheses */
    TOKEN_LPAREN, TOKEN_RPAREN, TOKEN_LBRACE, TOKEN_RBRACE,

    /* Delimiter */
    TOKEN_SEMICOLON, TOKEN_COMMA, TOKEN_DOT,

    /* Experssion atom */
    TOKEN_NUMBER, TOKEN_STRING, TOKEN_IDENTIFIER,

    /* Keyword */
    TOKEN_VAR, TOKEN_RETURN, TOKEN_PRINT, TOKEN_TRUE, TOKEN_FALSE,
    TOKEN_NIL,

    TOKEN_ERROR, TOKEN_EOF,
} toktype_t;

typedef struct {
    toktype_t type;
    const char *start;
    size_t length;
    size_t line;
} token_t;

typedef struct {
    const char *start;
    const char *current;
    size_t line;
} lexer_t;

PUBLIC void init_lexer(lexer_t *lexer, const char *source);
PUBLIC token_t scan_token(lexer_t *lexer);
PUBLIC char *token_to_string(token_t token);

#endif // VELO_LEXER_H
