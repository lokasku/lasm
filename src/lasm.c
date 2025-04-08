// SPDX-FileCopyrightText: 2025 Łukasz Bartkiewicz <lukasku@proton.me>
// SPDX-License-Identifier: MPL-2.0

#include <stdio.h>
#include <string.h>
#include "parser.h"
#include "semantic.h"

char*
win_ctxt(Parser* p)
{
    char* ctxt = malloc(22);
    if (!ctxt) return NULL;

    memset(ctxt, ' ', 21);
    ctxt[21] = '\0';

    int left = p->index < 10 ? p->index : 10;
    memcpy(ctxt + (10 - left), p->input + p->index - left, left);

    int right = 0;
    while (right < 11 && p->input[p->index + right]) {
        ctxt[10 + right] = p->input[p->index + right];
        right++;
    }

    return ctxt;
}

char*
sanitize(const char* str)
{
    if (!str) return NULL;

    size_t len = strlen(str);
    char* clean = malloc(len + 1);
    if (!clean) return NULL;

    char* dst = clean;
    while (*str) {
        if (*str >= 32 && *str != 127) {
            *dst++ = *str;
        }
        str++;
    }
    *dst = '\0';

    return clean;
}

int
main(int argc,
     char** argv)
{
    const char* input = argc > 1 ? sanitize(argv[1]) : "\\X.\\y.\\Z.XZ(yZ)";

    Parser p = {
        .index = 0,
        .input = input
    };

    Term* term = parse_expr(&p, 0);
    if (!term) {
        fprintf(stderr, "Parse error at position %zu\n", p.index);
        char* ctxt = win_ctxt(&p);
        if (ctxt) {
            fprintf(stderr, "`%s`\n", ctxt);
            free(ctxt);
        }
        return 1;
    }

    int valid = check(term);
    printf("`%s` is %s\n", input, valid ? "valid" : "invalid");

    free_term(term);
    return !valid;
}
