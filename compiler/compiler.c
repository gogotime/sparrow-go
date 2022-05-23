//
// Created by gogotime on 2022/5/23.
//

#include "compiler.h"
#include <string.h>

struct compileUnit {
    ObjFn* fn;
    LocalVar localVar[MAX_LOCAL_VAR_NUM];
    uint32 localValNum;
    UpValue upValue[MAX_UPVALUE_NUM];
    int scopeDepth;
    uint32 stackSlotNum;
    Loop* curLoop;
    ClassBookKeep* enclosingClassBK;
    struct compileUnit* enclosingUnit;
    Parser* parser;
};

int defineModuleVar(VM* vm, ObjModule* objModule, const char* name, uint32 length, Value value) {
    if (length > MAX_ID_LEN) {
        char id[MAX_ID_LEN] = {0};
        memcpy(id, name, length);
        if (vm->curParser != nil) {
            COMPILE_ERROR(vm->curParser, "length of identifier \"%s\" should be no more than %d", id, MAX_ID_LEN);
        } else {
            MEM_ERROR("length of identifier \"%s\" should be no more than %d", id, MAX_ID_LEN);
        }
    }

    int symbolIndex = getIndexFromSymbolTable(&objModule->moduleVarValue, name, length);
    if (symbolIndex == -1) {
        symbolIndex = addSymbol(vm, &objModule->moduleVarValue, name, length);
        ValueBufferAdd(vm, &objModule->moduleVarValue, value);
    } else if (VALUE_IS_NUM(objModule->moduleVarValue.data[symbolIndex])) {
        objModule->moduleVarValue.data[symbolIndex] = value;
    } else {
        symbolIndex = -1;
    }

    return symbolIndex;
}