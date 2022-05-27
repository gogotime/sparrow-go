//
// Created by gogotime on 2022/5/4.
//

#ifndef SPARROW_GO_VM_H
#define SPARROW_GO_VM_H

#include "../include/vm_parser.h"
#include "../include/common.h"

#define OPCODE_SLOTS(opcode, effect) OPCODE_##opcode

typedef enum {

#include "opcode.inc"

    OPCODE_NOP

} OpCode;

#undef OPCODE_SLOTS

typedef enum vm_result {
    VM_RESULT_ERROR,
    VM_RESULT_SUCCESS
} VMResult;

//struct my_vm {
//    uint allocatedByte;
//    Parser* parentParser;
//    ObjHeader* allObj;
//    Class* stringClass;
//    Class* fnClass;
//};

VM* newVM();

#endif //SPARROW_GO_VM_H
