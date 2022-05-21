//
// Created by gogotime on 2022/5/21.
//

#ifndef SPARROW_GO_OBJ_THREAD_H
#define SPARROW_GO_OBJ_THREAD_H

#include "class.h"
#include "obj_fn.h"

typedef struct objThread {
    ObjHeader objHeader;
    Value* stack;
    Value* esp;
    uint32 stackCap;
    Frame* frame;
    uint32 frameUsedNum;
    uint32 frameCap;

    ObjUpValue* openUpValue;
    struct objThread* caller;

    Value errorObj;
} ObjThread;

void prepareFrame(ObjThread* objThread, ObjClosure* objClosure, Value* stackStart);

ObjThread* newObjThread(VM* vm, ObjClosure* objClosure);

void resetThread(ObjThread* objThread, ObjClosure* objClosure);

#endif //SPARROW_GO_OBJ_THREAD_H
