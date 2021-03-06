//
// Created by gogotime on 2022/5/4.
//

#include "utils.h"
#include "common.h"
#include "../vm/vm.h"
#include "../parser/parser.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

void* memManager(VM* vm, void* ptr, uint oldSize, uint newSize) {
    if (newSize == 0) {
        if (ptr != nil) {
            free(ptr);
        }
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

void
errorReport(void* parser, ErrorType errorType, const char* file, int line, const char* func, const char* fmt, ...) {
    char buffer[DEFAULT_BUFFER_SIZE] = {0};
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buffer, DEFAULT_BUFFER_SIZE, fmt, ap);
    va_end(ap);
    switch (errorType) {
        case ERROR_IO:
            fprintf(stderr, "ERROR_IO: %s:%d:%s %s\n", file, line, func, buffer);
            break;
        case ERROR_MEM:
            fprintf(stderr, "ERROR_MEM: %s:%d:%s %s\n", file, line, func, buffer);
            break;
        case ERROR_LEX:
            ASSERT(parser != nil, "parentParser is nil!");
            fprintf(stderr, "ERROR_LEX: %s:%d %s\n", ((Parser*) parser)->file, ((Parser*) parser)->preToken.lineNo,
                    buffer);
            break;
        case ERROR_COMPILE:
            ASSERT(parser != nil, "parentParser is nil!");
            fprintf(stderr, "ERROR_COMPILE: %s:%d %s\n", ((Parser*) parser)->file,
                    ((Parser*) parser)->preToken.lineNo, buffer);
            break;
        case ERROR_RUNTIME:
            fprintf(stderr, "ERROR_MEM: %s:%d:%s %s\n", file, line, func, buffer);
            break;
        default:
            NOT_REACHED("error type not support");
    }
    exit(1);
}