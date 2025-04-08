// SPDX-FileCopyrightText: 2025 Łukasz Bartkiewicz <lukasku@proton.me>
// SPDX-License-Identifier: MPL-2.0

#include "semantic.h"

static int occ[52] = {0};

static int
var_index(char c)
{
    if (c >= 'a' && c <= 'z')
        return c - 'a';
    return (c - 'A') + 26;
}

int
check(Term* t)
{
    if (!t) return 0;

    int i;

    switch (t->type) {
        case ABS:
            i = var_index(t->abs.var);
            occ[i]++;
            if (check(t->abs.body)) {
                occ[i]--;
                return 1;
            }
            return 0;
        case APP:
            return check(t->app.lhs) && check(t->app.rhs);
        case VAR:
            return occ[var_index(t->var)] > 0 ? 1 : 0;
    }
    return 0;
}
