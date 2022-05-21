//
// Created by gogotime on 2022/5/8.
//

#ifndef SPARROW_GO_VM_PARSER_H
#define SPARROW_GO_VM_PARSER_H

#include "common.h"

typedef enum {
    TOKEN_UNKNOWN,
    TOKEN_NUM,            // number
    TOKEN_STRING,         // string
    TOKEN_ID,             // variable
    TOKEN_INTERPOLATION,  // interpolation

    TOKEN_KW_APPEND,      // append
    TOKEN_KW_BREAK,       // break
    TOKEN_KW_CASE,        // case
    TOKEN_KW_CHAN,        // chan
    TOKEN_KW_CONST,       // const
    TOKEN_KW_CONTINUE,    // continue
    TOKEN_KW_DEFAULT,     // default
    TOKEN_KW_DEFER,       // defer
    TOKEN_KW_ELSE,        // else
    TOKEN_KW_FALLTHROUGH, // fallthrough
    TOKEN_KW_FOR,         // for
    TOKEN_KW_FUNC,        // func
    TOKEN_KW_GO,          // go
    TOKEN_KW_GOTO,        // goto
    TOKEN_KW_IF,          // if
    TOKEN_KW_IMPORT,      // import
    TOKEN_KW_INTERFACE,   // interface
    TOKEN_KW_MAP,         // map
    TOKEN_KW_PACKAGE,     // package
    TOKEN_KW_RANGE,       // range
    TOKEN_KW_RETURN,      // return
    TOKEN_KW_SELECT,      // select
    TOKEN_KW_STRUCT,      // struct
    TOKEN_KW_SWITCH,      // switch
    TOKEN_KW_TYPE,        // type
    TOKEN_KW_VAR,         // var

//    TOKEN_TYPE_INT,      // int
//    TOKEN_TYPE_UINT,      // uint
//    TOKEN_TYPE_INT8,      // int8
//    TOKEN_TYPE_INT16,      // int16
//    TOKEN_TYPE_INT32,      // int32
//    TOKEN_TYPE_INT64,      // int64
//    TOKEN_TYPE_UINT8,      // uint8
//    TOKEN_TYPE_UINT16,      // uint16
//    TOKEN_TYPE_UINT32,      // uint32
//    TOKEN_TYPE_UINT64,      // uint64
//    TOKEN_TYPE_FLOAT32,      // float32
//    TOKEN_TYPE_FLOAT64,      // float64
//    TOKEN_TYPE_BOOL,      // bool
//    TOKEN_TYPE_STRING,      // string

    TOKEN_COMMA,         // ,
    TOKEN_COLON,         // :
    TOKEN_LEFT_PAREN,    // (
    TOKEN_RIGHT_PAREN,   // )
    TOKEN_LEFT_BRACKET,  // [
    TOKEN_RIGHT_BRACKET, // ]
    TOKEN_LEFT_BRACE,    // {
    TOKEN_RIGHT_BRACE,   // }
    TOKEN_DOT,           // .
    TOKEN_DOT_DOT,       // ..

    TOKEN_ADD, // +
    TOKEN_SUB, // -
    TOKEN_MUL, // *
    TOKEN_DIV, // /
    TOKEN_MOD, // %

    TOKEN_ASSIGN, // =

    TOKEN_BIT_AND,         // &
    TOKEN_BIT_OR,          // |
    TOKEN_BIT_NOT,         // ~
    TOKEN_BIT_SHIFT_RIGHT, // >>
    TOKEN_BIT_SHIFT_LEFT,  // <<数字>>

    TOKEN_LOGIC_AND, // &&
    TOKEN_LOGIC_OR,  // ||
    TOKEN_LOGIC_NOT, // !

    TOKEN_EQUAL,       // ==
    TOKEN_NOT_EQUAL,   // !=
    TOKEN_GREAT,       // >
    TOKEN_GREAT_EQUAL, // >=
    TOKEN_LESS,        // <
    TOKEN_LESS_EQUAL,  // <=

    TOKEN_QUESTION,    // ?
    TOKEN_EOF // EOF

} TokenType;

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
    SymbolTable allMethodNames;

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

typedef struct my_parser {
    const char* file;
    const char* sourceCode;
    const char* nextCharPtr;
    char curChar;
    Token curToken;
    Token preToken;
    ObjModule* curModule;

    int interpolationExpectRightParenNum;
    VM* vm;
} Parser;


#endif //SPARROW_GO_VM_PARSER_H
