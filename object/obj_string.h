//
// Created by gogotime on 2022/5/9.
//

#ifndef SPARROW_GO_OBJ_STRING_H
#define SPARROW_GO_OBJ_STRING_H

#include "../include/common.h"
#include "header_obj.h"

typedef struct {
    ObjHeader objHeader;
    uint32 hashCode;
    CharValue value;
} ObjString;

uint32 hashString(char* str, uint32 length);

void hashObjString(ObjString* objString);

ObjString* newObjString(VM* vm, const char* str, uint32 length);


#endif //SPARROW_GO_OBJ_STRING_H


