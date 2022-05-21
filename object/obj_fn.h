//
// Created by gogotime on 2022/5/9.
//

#ifndef SPARROW_GO_OBJ_FN_H
#define SPARROW_GO_OBJ_FN_H


#include "header_obj.h"
#include "obj_string.h"
#include "meta_obj.h"

#define INITIAL_FRAME_NUM 4

typedef struct {
    char* fnName;
    IntBuffer lineNo;
} FnDebug;

typedef struct {
    ObjHeader objHeader;
    ByteBuffer instrStream; // 指令流
    ValueBuffer constants; // 函数内常量表
    ObjModule* module; // 所属模块
    uint32 maxStackSlotUserNum;
    uint32 upValueNum; // 本函数所使用的upValue数量
    uint8 argNum; // 函数期望的参数个数
#if DEBUG
    FnDebug * debug;
#endif
} ObjFn;

typedef struct upValue {
    ObjHeader objHeader;
    // 指向upValue 关联的局部变量
    Value* localVarPtr;
    // 已被关闭的upValue
    Value closedUpValue;
    struct upValue* next;
} ObjUpValue;

typedef struct {
    ObjHeader objHeader;
    ObjFn* fn;
    ObjUpValue* upValue[0];
} ObjClosure;

typedef struct {
    uint8* ip;
    ObjClosure* closure;
    Value* stackStart;
} Frame;

ObjUpValue* newObjUpValue(VM* vm, Value* localVarPtr);

ObjClosure* newObjClosure(VM* vm, ObjFn* objFn);

ObjFn* newObjFn(VM* vm, ObjModule* objModule, uint32 slotNum);

#endif //SPARROW_GO_OBJ_FN_H
