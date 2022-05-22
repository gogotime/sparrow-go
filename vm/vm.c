//
// Created by gogotime on 2022/5/4.
//

#include "vm.h"
#include "core.h"
#include "../include/utils.h"
#include "../object/obj_map.h"
#include <stdlib.h>

void initVM(VM* vm) {
    vm->allocatedByte = 0;
    vm->curParser = nil;
    vm->allObj = nil;
    vm->stringClass = nil;
    StringBufferInit(&vm->allMethodNames);
    vm->allModules = newObjMap(vm);
}

VM* newVM() {
    VM* vm = (VM*) malloc(sizeof(VM));
    if (vm == nil) {
        MEM_ERROR("allocate VM failed");
    }
    initVM(vm);
    buildCore(vm);
    return vm;
}
