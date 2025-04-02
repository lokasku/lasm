#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>

typedef struct Term {
    enum { ABS, APP, VAR } type;
    union {
        char var;
        struct { char var; struct Term* body; } abs;
        struct { struct Term *lhs, *rhs; } app;
    } t;
} Term;

typedef struct {
    int index;
    const char* input;
} Parser;

static Term* parse_expr(Parser*);
static Term* parse_abs(Parser*);
static Term* parse_app(Parser*);
static Term* parse_term(Parser*);

Term*
make_term(int type)
{
    Term* t = malloc(sizeof(Term));
    t->type = type;
    return t;
}

void
skip(Parser* p)
{
    while (p->input[p->index] == ' '
        || p->input[p->index] == '\n')
        p->index++;
}

int
is_alpha(char c)
{
    static const char alphabet[] = "abcdefghijklmnopqrstuvwxyz";
    for (int i = 0; alphabet[i]; i++)
        if (c == alphabet[i])
            return 1;
    return 0;
}

int
is_eof(Parser* p)
{
    return p->input[p->index] == '\0';
}

/************************@;
| expr ::= \ var . expr
!       | app
|
! app ::= app atom
|       | atom
!
| atom ::= var | ( expr )
!
| var ::= [a-z]
#+**********************°*/

Term*
parse_var(Parser* p)
{
    skip(p);

    if (!is_alpha(p->input[p->index]))
        return NULL;

    Term* t = make_term(VAR);
    t->t.var = p->input[p->index++];

    return t;
}

Term*
parse_atom(Parser* p)
{
    skip(p);

    int index_copy = p->index;

    Term* t = parse_var(p);
    if (t) return t;

    p->index = index_copy;

    if (p->input[p->index] != '(')
        return NULL;

    p->index++;

    t = parse_expr(p);
    if (!t) return NULL;

    skip(p);

    if (p->input[p->index] != ')')
        return NULL;

    p->index++;

    return t;
}

Term*
parse_app(Parser* p)
{
    skip(p);

    Term* t = parse_atom(p);
    if (!t) return NULL;

    while (1) {
        skip(p);

        if (is_eof(p)
            || (!is_alpha(p->input[p->index])
                && p->input[p->index] != '('))
            break;

        Term* rhs = parse_atom(p);
        if (!rhs) break;

        Term* _t = make_term(APP);
        _t->t.app.lhs = t;
        _t->t.app.rhs = rhs;
        t = _t;
    }

    return t;
}

Term*
parse_expr(Parser* p)
{
    skip(p);

    if (p->input[p->index] == '\\') {
        p->index++;
        Term* t = parse_var(p);
        if (!t) return NULL;

        skip(p);

        if (p->input[p->index] != '.')
            return NULL;

        p->index++;

        Term* body = parse_expr(p);
        if (!body) return NULL;

        Term* abs = make_term(ABS);
        abs->t.abs.var = t->t.var;
        abs->t.abs.body = body;

        return abs;
    }

    return parse_app(p);
}

static Term*
parse(const char* s)
{
    Parser p = {
        .index = 0,
        .input = s
    };

    return parse_expr(&p);
}

static void
print(Term* t) {
    switch (t->type) {
        case VAR: printf("%c", t->t.var); break;
        case ABS: printf("(λ%c.", t->t.abs.var);
                 print(t->t.abs.body);
                 printf(")"); break;
        case APP: printf("(");
                 print(t->t.app.lhs);
                 printf(" ");
                 print(t->t.app.rhs);
                 printf(")"); break;
    }
}

static void
free_term(Term* t) {
    if (!t) return;
    if (t->type == ABS) free_term(t->t.abs.body);
    if (t->type == APP) {
        free_term(t->t.app.lhs);
        free_term(t->t.app.rhs);
    }
    free(t);
}

int
main(void)
{
    const char* tests[] = {
        "x", "\\x.x", "(\\x.x)", "\\x.\\y.x y",
        "(\\x.x) y", "x y z", "(x y) z", "x (y z)"
    };

    for (int i = 0; i < sizeof(tests)/sizeof(*tests); i++) {
        Term* t = parse(tests[i]);
        printf("%s => ", tests[i]);
        print(t);
        printf("\n");
        free_term(t);
    }
}
