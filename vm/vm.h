//
// Created by gogotime on 2022/5/4.
//

#ifndef SPARROW_GO_VM_H
#define SPARROW_GO_VM_H

#include "../include/common.h"


typedef struct _vm {
    uint32 allocatedByte;
    Parser* curParser;
} VM;

VM* newVM();

#endif //SPARROW_GO_VM_H
