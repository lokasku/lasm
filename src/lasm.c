// SPDX-FileCopyrightText: 2025 Łukasz Bartkiewicz <lukasku@proton.me>
// SPDX-License-Identifier: MPL-2.0

#include <stdio.h>
#include "parser.h"
#include "semantic.h"

int main(int argc, char** argv) {
    const char* input = argc > 1 ? argv[1] : "\\x.\\y.\\z.xz(yz)";

    Parser p = {
        .index = 0,
        .input = input
    };

    Term* term = parse_expr(&p);
    if (!term) {
        fprintf(stderr, "Parse error\n");
        return 1;
    }

    int valid = check(term);
    printf("`%s` is %s\n", input, valid ? "valid" : "invalid");

    free_term(term);
    return !valid;
}
