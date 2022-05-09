//
// Created by gogotime on 2022/5/9.
//

#ifndef SPARROW_GO_META_OBJ_H
#define SPARROW_GO_META_OBJ_H

#include "obj_string.h"

typedef struct {
    ObjHeader objHeader;
    SymbolTable moduleVarName;
    ValueBuffer moduleVarValue;
    ObjString* name;
} ObjModule;

typedef struct {
    ObjHeader objHeader;
    Value fields[0];
} ObjInstance;

ObjModule* newObjModule(VM* vm, const char* modName);

ObjInstance* newObjInstance(VM* vm, Class* aClass);

#endif //SPARROW_GO_META_OBJ_H
