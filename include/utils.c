//
// Created by gogotime on 2022/5/4.
//

#include "utils.h"
#include "common.h"
#include "../parser/parser.h"
#include <stdlib.h>
#include <stdarg.h>
#include "../vm/vm.h"

void* memManager(VM* vm, void* ptr, uint32 oldSize, uint32 newSize) {
    vm->allocatedByte += newSize - oldSize;
    if (newSize == 0) {
        free(ptr);
        return nil;
    }
    return realloc(ptr, newSize);
}

uint32 ceilToPowerOf2(uint32 v) {
    v += (v == 0);
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;
    return v;
}

DECLARE_BUFFER_METHOD(String)

DECLARE_BUFFER_METHOD(Int)

DECLARE_BUFFER_METHOD(Char)

DECLARE_BUFFER_METHOD(Byte)


void symbolTableClear(VM* vm, SymbolTable* buffer) {
    uint32 idx = 0;
    while (idx < buffer->cnt) {
        memManager(vm, buffer->data[idx++].str, 0, 0);
    }
    StringBufferClear(vm, buffer);
}

void errorReport(void* parser, ErrorType errorType, const char* fmt, ...) {
    char buffer[DEFAULT_BUFFER_SIZE] = {0};
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buffer, DEFAULT_BUFFER_SIZE, fmt, ap);
    va_end(ap);
    switch (errorType) {
        case ERROR_IO:
            fprintf(stderr, "ERROR_IO: %s:%d:%s %s\n", __FILE__, __LINE__, __func__, buffer);
        case ERROR_MEM:
            fprintf(stderr, "ERROR_MEM: %s:%d:%s %s\n", __FILE__, __LINE__, __func__, buffer);
        case ERROR_LEX:
            ASSERT(parser != nil, "parser is nil!");
            fprintf(stderr, "ERROR_LEX: %s:%d %s\n", ((Parser*) parser)->file, ((Parser*) parser)->preToken.lineNo,
                    buffer);
        case ERROR_COMPILE:
            ASSERT(parser != nil, "parser is nil!");
            fprintf(stderr, "ERROR_COMPILE: %s:%d %s\n", ((Parser*) parser)->file,
                    ((Parser*) parser)->preToken.lineNo, buffer);
        case ERROR_RUNTIME:
            fprintf(stderr, "ERROR_MEM: %s:%d:%s %s\n", __FILE__, __LINE__, __func__, buffer);
        default:
            NOT_REACHED("error type not support");
    }
    exit(1);
}