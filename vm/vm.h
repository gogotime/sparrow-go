//
// Created by gogotime on 2022/5/4.
//

#ifndef SPARROW_GO_VM_H
#define SPARROW_GO_VM_H

#include "../include/vm_parser.h"
#include "../include/common.h"


//struct my_vm {
//    uint allocatedByte;
//    Parser* curParser;
//    ObjHeader* allObj;
//    Class* stringClass;
//    Class* fnClass;
//};

VM* newVM();

#endif //SPARROW_GO_VM_H
