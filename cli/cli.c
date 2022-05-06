//
// Created by gogotime on 2022/5/6.
//

#include "cli.h"
#include <stdio.h>
#include <stdlib.h>
#include "../include/common.h"
#include "../parser/parser.h"
#include "../vm/vm.h"
#include "../vm/core.h"
#include "unistd.h"

static void runFile(const char* path) {
    char* buf = malloc(1024);
    rootDir = getcwd(buf, 1024);

    VM* vm = newVM();

    const char* sourceCode = readFile(path);

    Parser* parser = newParser(vm, path, sourceCode);

    while (parser->curToken.type != TOKEN_EOF) {
        getNextToken(parser);
        printf("%dL: %s [", parser->curToken.lineNo, tokenTypeMap[parser->curToken.type]);
        uint32 idx = 0;
        while (idx < parser->curToken.length) {
            printf("%c", *(parser->curToken.start + idx));
            idx++;
        }
        printf(")\n");
    }
}

int main(int argc, const char** argv) {
    if (argc == 1) {
    } else {
        runFile(argv[1]);
    }
}