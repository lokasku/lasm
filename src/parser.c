// SPDX-FileCopyrightText: 2025 Łukasz Bartkiewicz <lukasku@proton.me>
// SPDX-License-Identifier: MPL-2.0

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include "parser.h"

static Term* make_term(int);
static void skip(Parser*);
static int is_alpha(char);
static Term* parse_var(Parser*);
static Term* parse_atom(Parser*, int);
static Term* parse_app(Parser*, int);

/************************@;
§ expr ::= \ var . expr
!       | app
|
! app ::= app atom
       | atom
!
| atom ::= var | ( expr )
!
§ var ::= [a-zA-Z]
#+**********************°*/

Term*
make_term(int type)
{
    Term* t = malloc(sizeof(Term));
    if (!t) return NULL;
    t->type = type;
    return t;
}

void
free_term(Term* t)
{
    if (!t) return;

    switch (t->type) {
        case ABS:
            free_term(t->abs.body);
            break;
        case APP:
            free_term(t->app.lhs);
            free_term(t->app.rhs);
            break;
        case VAR:
            break;
    }

    free(t);
}

void
skip(Parser* p)
{
    while (p->input[p->index] == ' ')
        p->index++;
}

int
is_alpha(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

Term*
parse_var(Parser* p)
{
    skip(p);

    if (!is_alpha(p->input[p->index]))
        return NULL;

    Term* t = make_term(VAR);
    if (!t) return NULL;
    t->var = p->input[p->index++];

    return t;
}

Term*
parse_atom(Parser* p,
           int depth)
{
    skip(p);

    int index_copy = p->index;

    Term* t = parse_var(p);
    if (t) return t;

    p->index = index_copy;

    if (p->input[p->index] != '(')
        return NULL;

    p->index++;

    t = parse_expr(p, depth + 1);
    if (!t) return NULL;

    skip(p);

    if (p->input[p->index] != ')')
        return NULL;

    p->index++;

    return t;
}

Term*
parse_app(Parser* p,
          int depth)
{
    skip(p);

    Term* t = parse_atom(p, depth);
    if (!t) return NULL;

    while (1) {
        skip(p);

        if (p->input[p->index] == '\0'
            || (!is_alpha(p->input[p->index])
                && p->input[p->index] != '('))
            break;

        Term* rhs = parse_atom(p, depth);
        if (!rhs) break;

        Term* _t = make_term(APP);
        _t->app.lhs = t;
        _t->app.rhs = rhs;
        t = _t;
    }

    return t;
}

Term*
parse_expr(Parser* p,
           int depth)
{
    if (depth >= 1000) {
        fprintf(stderr, "Maximum recursion depth exceeded\n");
        return NULL;
    }

    skip(p);

    if (p->input[p->index] == '\\') {
        p->index++;

        skip(p);

        Term* var = parse_var(p);
        if (!var) return NULL;
        char var_name = var->var;
        free(var);

        skip(p);

        if (p->input[p->index] != '.') return NULL;
        p->index++;

        Term* body = parse_expr(p, depth + 1);
        if (!body) return NULL;

        Term* abs = make_term(ABS);
        if (!abs) {
            free_term(body);
            return NULL;
        }
        abs->abs.var = var_name;
        abs->abs.body = body;
        return abs;
    }

    return parse_app(p, depth);
}
