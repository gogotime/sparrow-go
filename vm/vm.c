//
// Created by Galamo on 2022/5/4.
//

#include "vm.h"
#include "../include/utils.h"
#include <stdlib.h>

void initVM(VM* vm) {
    vm->allocatedByte = 0;
    vm->curParser = nil;
}

VM* newVM() {
    VM* vm = (VM*) malloc(sizeof(VM));
    if (vm == nil) {
        MEM_ERROR("allocate VM failed");
    }
    initVM(vm);
    return vm;
}
