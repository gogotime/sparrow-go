//
// Created by gogotime on 2022/5/8.
//

#ifndef SPARROW_GO_HEADER_OBJ_H
#define SPARROW_GO_HEADER_OBJ_H

#include "../vm/vm.h"
#include "../include/common.h"
#include "../include/utils.h"





void initObjHeader(VM* vm, ObjHeader* objHeader, ObjType objType, Class* aClass);

DECLARE_BUFFER_TYPE(Value)

#endif //SPARROW_GO_HEADER_OBJ_H
