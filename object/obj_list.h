//
// Created by gogotime on 2022/5/20.
//

#ifndef SPARROW_GO_OBJ_LIST_H
#define SPARROW_GO_OBJ_LIST_H

#include "class.h"
#include "../vm/vm.h"

typedef struct {
    ObjHeader objHeader;
    ValueBuffer elements;
}ObjList;

ObjList * newObjList(VM* vm,uint32 elementNum);

Value removeElement(VM* vm,ObjList* objList,uint32 index);

void insertElement(VM* vm,ObjList* objList,uint32 index,Value value);

#endif //SPARROW_GO_OBJ_LIST_H
