//
// Created by gogotime on 2022/5/8.
//

#ifndef SPARROW_GO_VM_PARSER_H
#define SPARROW_GO_VM_PARSER_H

#include "common.h"

#include "token.h"

extern char* tokenTypeMap[];

typedef enum {
    OT_CLASS,
    OT_LIST,
    OT_MAP,
    OT_MODULE,
    OT_RANGE,
    OT_STRING,
    OT_UPVALUE,
    OT_FUNCTION,
    OT_CLOSURE,
    OT_INSTANCE,
    OT_THREAD,
} ObjType;

typedef struct my_class Class;
typedef struct my_obj_map ObjMap;

typedef struct objHeader {
    ObjType type;
    bool isDark;
    Class* class;
    struct objHeader* next;
} ObjHeader;

typedef enum {
    VT_UNDEFINED,
    VT_NULL,
    VT_FALSE,
    VT_TRUE,
    VT_NUM,
    VT_OBJ,
} ValueType;

typedef struct {
    ValueType type;
    union {
        double num;
        ObjHeader* objHeader;
    };
} Value;

typedef struct {
    TokenType type;
    const char* start;
    uint32 length;
    uint32 lineNo;
    Value value;
} Token;

typedef struct my_vm VM;

#define DECLARE_BUFFER_TYPE(type)                                                   \
typedef struct type##_buffer {                                                      \
    type* data;                                                                     \
    uint32 cnt;                                                                     \
    uint32 cap;                                                                     \
}type##Buffer;                                                                      \
                                                                                    \
void type##BufferInit(type##Buffer* buf);                                           \
void type##BufferFillWrite(VM* vm,type##Buffer* buf,type data,uint32 fillCnt);      \
void type##BufferAdd(VM* vm,type##Buffer* buf,type data);                           \
void type##BufferClear(VM* vm,type##Buffer* buf);                                   \


#define DECLARE_BUFFER_METHOD(type)                                                 \
void type##BufferInit(type##Buffer* buf) {                                          \
    buf->data=nil;                                                                  \
    buf->cnt=buf->cap=0;                                                            \
}                                                                                   \
void type##BufferFillWrite(VM* vm,type##Buffer* buf,type data,uint32 fillCnt){      \
    uint32 newCnt=buf->cnt+fillCnt;                                                 \
    if(newCnt>buf->cap){                                                            \
            uint oldSize=buf->cap * sizeof(type);                                   \
            buf->cap=ceilToPowerOf2(newCnt);                                        \
            uint newSize=buf->cap * sizeof(type);                                   \
            ASSERT(newSize>oldSize,"failed to allocate memory type ")               \
            buf->data=(type*)memManager(vm,buf->data,oldSize,newSize);              \
                                                                                    \
    }                                                                               \
    uint32 cnt=0;                                                                   \
    while(cnt<fillCnt){                                                             \
                                                                                    \
        buf->data[buf->cnt]=data;                                                   \
        buf->cnt++;                                                                 \
        cnt++;                                                                      \
    }                                                                               \
                                                                                    \
}                                                                                   \
void type##BufferAdd(VM* vm,type##Buffer* buf,type data){                           \
    type##BufferFillWrite(vm, buf,data,1);                                          \
}                                                                                   \
void type##BufferClear(VM* vm,type##Buffer* buf){                                   \
    size_t oldSize=buf->cap * sizeof(buf->data[0]);                                 \
    memManager(vm,buf->data,oldSize,0);                                             \
    type##BufferInit(buf);                                                          \
}                                                                                   \

typedef struct _string {
    char* str;
    uint32 length;
} String;

typedef struct _charValue {
    uint32 length;
    char start[0];
} CharValue;

#define SymbolTable StringBuffer

DECLARE_BUFFER_TYPE(String)

typedef struct my_vm {
    uint allocatedByte;
    struct my_parser* curParser;
    ObjHeader* allObj;
    ObjMap* allModules;

//    ObjThread* curThread;
    SymbolTable allMethodName;

    Class* stringClass;
    Class* fnClass;
    Class* listClass;
    Class* rangeClass;
    Class* mapClass;
    Class* nullClass;
    Class* boolClass;
    Class* numClass;
    Class* threadClass;
    Class* objClass;
    Class* classOfClass;

} VM;

typedef struct my_obj_module ObjModule;
typedef struct compileUnit CompileUnit;

typedef struct my_parser {
    const char* file;
    const char* sourceCode;
    const char* nextCharPtr;
    char curChar;
    Token curToken;
    Token preToken;
    ObjModule* curModule;
    CompileUnit* compileUnit;


    int interpolationExpectRightParenNum;
    struct my_parser* parentParser;
    VM* vm;
} Parser;


#endif //SPARROW_GO_VM_PARSER_H
