//
// Created by gogotime on 2022/5/21.
//

#ifndef SPARROW_GO_OBJ_RANGE_H
#define SPARROW_GO_OBJ_RANGE_H

#include "header_obj.h"

typedef struct {
    ObjHeader objHeader;
    int from;
    int to;
} ObjRange;

ObjRange* newObjRange(VM* vm, int from, int to);

#endif //SPARROW_GO_OBJ_RANGE_H
