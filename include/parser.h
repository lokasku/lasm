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

Term* make_term(int type);
void skip(Parser* p);
int is_alpha(char c);
Term* parse_var(Parser* p);
Term* parse_atom(Parser* p);
Term* parse_app(Parser* p);

Term* parse_expr(Parser* p);
void free_term(Term* t);

#endif
