//
// Created by gogotime on 2022/5/21.
//

#include "obj_thread.h"

void prepareFrame(ObjThread* objThread, ObjClosure* objClosure, Value* stackStart) {
    ASSERT(objThread->frameUsedNum < objThread->frameCap, "frame not enough")
    Frame* frame = &(objThread->frame[objThread->frameUsedNum++]);
    frame->stackStart = stackStart;
    frame->closure = objClosure;
    frame->ip = objClosure->fn->instrStream.data;
}

void resetThread(ObjThread* objThread, ObjClosure* objClosure) {
    objThread->esp = objThread->stack;
    objThread->openUpValue = nil;
    objThread->caller = nil;
    objThread->errorObj = VT_TO_VALUE(VT_NULL);
    objThread->frameUsedNum = 0;

    ASSERT(objClosure != nil, "objClosure is nil in resetThread")
    prepareFrame(objThread, objClosure, objThread->stack);
}

ObjThread* newObjThread(VM* vm, ObjClosure* objClosure) {
    ASSERT(objClosure != nil, "objClosure is nil")

    Frame* frame = ALLOCATE_ARRAY(vm, Frame, INITIAL_FRAME_NUM);
    uint32 stackCap = ceilToPowerOf2(objClosure->fn->maxStackSlotUserNum + 1);
    Value* newStack = ALLOCATE_ARRAY(vm, Value, stackCap);

    ObjThread* objThread = ALLOCATE(vm, ObjThread);
    initObjHeader(vm, &objThread->objHeader, OT_THREAD, vm->threadClass);

    objThread->frame = frame;
    objThread->frameCap = INITIAL_FRAME_NUM;
    objThread->stack = newStack;
    objThread->stackCap = stackCap;

    resetThread(objThread, objClosure);
    return objThread;
}

