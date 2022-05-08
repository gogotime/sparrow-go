//
// Created by gogotime on 2022/5/8.
//

#ifndef SPARROW_GO_HEADER_OBJ_H
#define SPARROW_GO_HEADER_OBJ_H

#include "../include/common.h"
#include "../include/utils.h"


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

DECLARE_BUFFER_TYPE(Value);

void initObjHeader(VM* vm, ObjHeader* objHeader, ObjType objType, Class* aClass);

#endif //SPARROW_GO_HEADER_OBJ_H
