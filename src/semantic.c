// SPDX-FileCopyrightText: 2025 Łukasz Bartkiewicz <lukasku@proton.me>
// SPDX-License-Identifier: MPL-2.0

#include "semantic.h"

static int occ[26] = {0};

int
check(Term* t)
{
    if (!t) return 0;

    int i;

    switch (t->type) {
        case ABS:
            i = t->abs.var - 'a';
            occ[i]++;
            if (check(t->abs.body)) {
                occ[i]--;
                return 1;
            }
            return 0;
        case APP:
            return check(t->app.lhs) && check(t->app.rhs);
        case VAR:
            return occ[t->var - 'a'] > 0 ? 1 : 0;
    }
    return 0;
}
