//
// Created by gogotime on 2022/5/4.
//

#ifndef SPARROW_GO_CORE_H
#define SPARROW_GO_CORE_H

#include "../include/vm_parser.h"
#include "../object/meta_obj.h"
#include "vm.h"

#define CORE_MODULE VT_TO_VALUE(VT_NULL)

extern char* rootDir;

int getIndexFromSymbolTable(SymbolTable* table, const char* symbol, uint32 length);

int addSymbol(VM* vm, SymbolTable* table, const char* symbol, uint32 length);

void buildCore(VM* vm);

VMResult executeModule(VM* vm, Value moduleName, const char* moduleCode);

char* readFile(const char* path);

#endif //SPARROW_GO_CORE_H
