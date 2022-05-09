//
// Created by gogotime on 2022/5/9.
//

#include "obj_string.h"
#include "../vm/vm.h"
#include "string.h"

// fnv-1a
uint32 hashString(char* str, uint32 length) {
    uint32 hashCode = 2166136261;
    uint32 idx = 0;
    while (idx < length) {
        hashCode ^= str[idx];
        hashCode *= 16777619;
        idx++;
    }
    return hashCode;
}

void hashObjString(ObjString* objString) {
    objString->hashCode = hashString(objString->value.start, objString->value.length);
}

ObjString* newObjString(VM* vm, const char* str, uint32 length) {
    ASSERT(length == 0 || str != nil, "string length must >0")
    ObjString* objString = ALLOCATE_EXTRA(vm, ObjString, length + 1);
    if (objString != nil) {
        initObjHeader(vm,&(objString->objHeader),OT_STRING,vm->stringClass);
        objString->value.length=length;
        if(length>0){
            memcpy(objString->value.start, str, length);
        }
        objString->value.start[length] = 0;
        hashObjString(objString);
    } else {
        MEM_ERROR("allocate ObjString failed");
    }
    return objString;
}



