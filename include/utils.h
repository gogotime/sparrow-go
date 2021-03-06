//
// Created by gogotime on 2022/5/4.
//

#ifndef SPARROW_GO_UTILS_H
#define SPARROW_GO_UTILS_H

#include "common.h"
#include "vm_parser.h"

void* memManager(VM* vm, void* ptr, uint oldSize, uint newSize);

#define ALLOCATE(vmPtr, type) \
(type*) memManager(vmPtr,nil,0,sizeof(type))

#define ALLOCATE_EXTRA(vmPtr, type, extraSize) \
(type*) memManager(vmPtr,nil,0,sizeof(type)+(extraSize))

#define ALLOCATE_ARRAY(vmPtr, type, count) \
(type*) memManager(vmPtr,nil,0,sizeof(type)*(count))

#define DEALLOCATE_ARRAY(vmPtr, arrayPtr, count) \
memManager(vmPtr,arrayPtr,sizeof((arrayPtr)[0])*(count),0)

#define DEALLOCATE(vmPtr, memPtr, count) \
(type*) memManager(vmPtr,memPtr,0,0)

uint32 ceilToPowerOf2(uint32 v);


typedef uint8_t Byte;
typedef char Char;
typedef int Int;
DECLARE_BUFFER_TYPE(Byte)

DECLARE_BUFFER_TYPE(Char)

DECLARE_BUFFER_TYPE(Int)

typedef enum {
    ERROR_IO,
    ERROR_MEM,
    ERROR_LEX,
    ERROR_COMPILE,
    ERROR_RUNTIME
} ErrorType;

void symbolTableClear(VM* vm, SymbolTable* buffer);

void errorReport(void* parser, ErrorType errorType, const char* file, int line, const char* func, const char* fmt, ...);


#define IO_ERROR(...) errorReport(NULL,ERROR_IO,__FILE__,__LINE__,__func__,__VA_ARGS__)
#define MEM_ERROR(...) errorReport(NULL,ERROR_MEM,__FILE__,__LINE__,__func__,__VA_ARGS__)
#define LEX_ERROR(parser, ...) errorReport(parser,ERROR_LEX,__FILE__,__LINE__,__func__,__VA_ARGS__)
#define COMPILE_ERROR(parser, ...) errorReport(parser,ERROR_COMPILE,__FILE__,__LINE__,__func__,__VA_ARGS__)
#define RUNTIME_ERROR(...) errorReport(NULL,ERROR_RUNTIME,__FILE__,__LINE__,__func__,__VA_ARGS__)

#define DEFAULT_BUFFER_SIZE 512
#endif //SPARROW_GO_UTILS_H
