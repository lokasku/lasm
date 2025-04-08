#ifndef PARSER_H
#define PARSER_H

#include <stdlib.h>

typedef struct Term {
    enum { ABS, APP, VAR } type;
    union {
        struct { char var; struct Term* body; } abs;
        struct { struct Term *lhs, *rhs; } app;
        char var;
    };
} Term;

typedef struct {
    size_t index;
    const char* input;
} Parser;

Term* parse_expr(Parser*, int);
void free_term(Term*);

#endif
