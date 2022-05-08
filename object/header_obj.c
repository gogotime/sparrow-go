//
// Created by gogotime on 2022/5/8.
//

#include "header_obj.h"
#include "../vm/vm.h"

DECLARE_BUFFER_METHOD(Value)

void initObjHeader(VM* vm, ObjHeader* objHeader, ObjType objType, Class* aClass) {
    objHeader->type = objType;
    objHeader->class = aClass;
    objHeader->isDark = false;
    objHeader->next = vm->allObj;
    vm->allObj = objHeader;
}