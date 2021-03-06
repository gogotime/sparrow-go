//
// Created by gogotime on 2022/5/8.
//


#include "class.h"
#include "header_obj.h"
#include "../include/utils.h"
#include "string.h"

DECLARE_BUFFER_METHOD(Method)

bool valueIsEqual(Value a, Value b) {
    if (a.type != b.type) {
        return false;
    }
    if (a.type == VT_NUM) {
        return a.num == b.num;
    }

    if (a.objHeader == b.objHeader) {
        return true;
    }

    if (a.objHeader->type != b.objHeader->type) {
        return false;
    }

    if (a.objHeader->type == OT_STRING) {
        ObjString* strA = VALUE_TO_OBJSTR(a);
        ObjString* strB = VALUE_TO_OBJSTR(b);
        return (strA->value.length == strB->value.length &&
                memcmp(strA->value.start, strB->value.start, strA->value.length) == 0);
    }

    if (a.objHeader->type == OT_RANGE) {
        ObjRange* rgA = VALUE_TO_OBJRANGE(a);
        ObjRange* rgB = VALUE_TO_OBJRANGE(b);
        return rgA->from == rgB->from && rgA->to == rgB->to;
    }

    return false;
}

Class* newRawClass(VM* vm, const char* name, uint32 fieldNum) {
    Class* aClass = ALLOCATE(vm, Class);
    initObjHeader(vm, &aClass->objHeader, OT_CLASS, nil);
    aClass->name = newObjString(vm, name, strlen(name));
    aClass->fieldNum = fieldNum;
    aClass->superClass = nil;
    MethodBufferInit(&aClass->method);
    return aClass;
}

inline Class* getClassOfObj(VM* vm, Value value) {
    switch (value.type) {
        case VT_NULL:
            return vm->nullClass;
        case VT_FALSE:
        case VT_TRUE:
            return vm->boolClass;
        case VT_NUM:
            return vm->numClass;
        case VT_OBJ:
            return VALUE_TO_OBJ(value)->class;
        default:
            NOT_REACHED("unknown value type")
    }
    return nil;
}


