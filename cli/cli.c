//
// Created by gogotime on 2022/5/6.
//

#include "cli.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/common.h"
#include "../include/utils.h"
#include "../parser/parser.h"
#include "../vm/vm.h"
#include "../vm/core.h"
#include "../object//class.h"
#include "unistd.h"


static void testMalloc(VM* vm) {
    char* ptr = memManager(vm, nil, 0, 1);
    memManager(vm, ptr, 0, 0);
}



static void runFile(const char* path) {

    char* buf = malloc(1024);
    rootDir = getcwd(buf, 1024);

    if (rootDir == NULL) {
        printf("malloc memory for rootDir failed");
    }
    printf("hello sprgo\n");
    VM* vm = newVM();
//    testMalloc(vm);

    printf("read source code\n");
    const char* sourceCode = readFile(path);
    executeModule(vm, OBJ_TO_VALUE(newObjString(vm, path, strlen(path))), sourceCode);

    Parser* parser = newParser(vm, path, sourceCode);

    vm->curParser = parser;

    while (parser->curToken.type != TOKEN_EOF) {
        getNextToken(parser);
        printf("%dL: %s [", parser->curToken.lineNo, tokenTypeMap[parser->curToken.type]);
        uint32 idx = 0;
        while (idx < parser->curToken.length) {
            printf("%c", *(parser->curToken.start + idx));
            idx++;
        }
        printf("]\n");
    }
}

int main(int argc, const char** argv) {
    if (argc == 1) {
    } else {
        runFile(argv[1]);
    }
}