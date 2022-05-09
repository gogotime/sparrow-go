//
// Created by gogotime on 2022/5/9.
//

#include "obj_fn.h"
#include "../vm/vm.h"
#include "class.h"

ObjUpValue* newObjUpValue(VM* vm, Value* localVarPtr) {
    ObjUpValue* objUpValue = ALLOCATE(vm, ObjUpValue);
    if (objUpValue == nil) {
        MEM_ERROR("failed to allocate ObjUpValue");
    }
    initObjHeader(vm, &objUpValue->objHeader, OT_UPVALUE, nil);
    objUpValue->localVarPtr = localVarPtr;
    objUpValue->closedUpValue = VT_TO_VALUE(VT_NULL);
    objUpValue->next = nil;
    return objUpValue;
}

ObjClosure* newObjClosure(VM* vm, ObjFn* objFn) {
    ObjClosure* objClosure = ALLOCATE_EXTRA(vm, ObjClosure, sizeof(ObjUpValue*) * objFn->upValueNum);
    if (objClosure == nil) {
        MEM_ERROR("failed to allocate ObjClosure");
    }
    initObjHeader(vm, &objClosure->objHeader, OT_CLOSURE, vm->fnClass);
    objClosure->fn = objFn;
    uint32 idx = 0;
    while (idx < objFn->upValueNum) {
        objClosure->upValue[idx] = nil;
        idx++;
    }
    return objClosure;
}

ObjFn* newObjFn(VM* vm, ObjModule* objModule, uint32 slotNum) {
    ObjFn* objFn = ALLOCATE(vm, ObjFn);
    if (objFn == nil) {
        MEM_ERROR("failed to allocate ObjFn");
    }
    initObjHeader(vm, &objFn->objHeader, OT_FUNCTION, vm->fnClass);
    ByteBufferInit(&objFn->instrStream);
    ValueBufferInit(&objFn->constants);
    objFn->module = objModule;
    objFn->maxStackSlotUserNum = slotNum;
    objFn->upValueNum = objFn->argNum = 0;
#ifdef DEBUG
    objFn->debug = ALLOCATE(vm, FnDebug);
    objFn->debug->fnName = nil;
    IntBufferInit(&objFn->debug->lineNo);
#endif
    return objFn;
}