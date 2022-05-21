//
// Created by gogotime on 2022/5/20.
//

#include "obj_list.h"


ObjList* newObjList(VM* vm, uint32 elementNum) {
    Value* elementArr = nil;
    if (elementNum > 0) {
        elementArr = ALLOCATE_ARRAY(vm, Value, elementNum);
    }
    ObjList* objList = ALLOCATE(vm, ObjList);
    objList->elements.data = elementArr;
    objList->elements.cap = elementNum;
    objList->elements.cnt = elementNum;
    initObjHeader(vm, &objList->objHeader, OT_LIST, vm->listClass);
    return objList;
}

static void shrinkList(VM* vm, ObjList* objList, uint32 newCap) {
    uint32 oldSize = objList->elements.cap * sizeof(Value);
    uint32 newSize = newCap * sizeof(Value);
    memManager(vm, objList->elements.data, oldSize, newSize);
    objList->elements.cap = newCap;
}

Value removeElement(VM* vm, ObjList* objList, uint32 index) {
    Value valueRemoved = objList->elements.data[index];
    uint32 idx = index;
    while (idx < objList->elements.cnt) {
        objList->elements.data[idx] = objList->elements.data[idx + 1];
        idx++;
    }
    uint32 newCap = objList->elements.cap / CAPACITY_GROW_FACTOR;
    if (newCap > objList->elements.cnt) {
        shrinkList(vm, objList, newCap);
    }
    objList->elements.cnt--;
    return valueRemoved;
}

void insertElement(VM* vm, ObjList* objList, uint32 index, Value value) {
    if (index > objList->elements.cnt - 1) {
        RUNTIME_ERROR("index out of bound");
    }

    ValueBufferAdd(vm, &objList->elements, VT_TO_VALUE(VT_NULL));
    uint32 idx = objList->elements.cnt - 1;
    while (idx > index) {
        objList->elements.data[idx] = objList->elements.data[idx - 1];
        idx--;
    }
    objList->elements.data[index] = value;

}