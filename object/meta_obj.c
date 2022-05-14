//
// Created by gogotime on 2022/5/9.
//

#include "meta_obj.h"
#include "string.h"
#include "class.h"

ObjModule* newObjModule(VM* vm, const char* modName) {
    ObjModule* objModule = ALLOCATE(vm, ObjModule);
    if (objModule == nil) {
        MEM_ERROR("failed to allocate ObjModule");
    }
    initObjHeader(vm, &objModule->objHeader, OT_MODULE, NULL);
    StringBufferInit(&objModule->moduleVarName);
    ValueBufferInit(&objModule->moduleVarValue);
    objModule->name = nil;
    if (modName == nil) {
        objModule->name = newObjString(vm, modName, strlen(modName));
    }
    return objModule;
}

ObjInstance* newObjInstance(VM* vm, Class* aClass) {
    ObjInstance* objInstance = ALLOCATE_EXTRA(vm, ObjInstance, sizeof(Value) * aClass->fieldNum);
    if (objInstance == nil) {
        MEM_ERROR("failed to allocate ObjInstance");
    }
    initObjHeader(vm, &objInstance->objHeader, OT_INSTANCE, aClass);
    uint32 idx = 0;
    while (idx < aClass->fieldNum) {
        objInstance->fields[idx] = VT_TO_VALUE(VT_NULL);
        idx++;
    }
    return objInstance;
}