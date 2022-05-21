//
// Created by gogotime on 2022/5/21.
//

#ifndef SPARROW_GO_OBJ_MAP_H
#define SPARROW_GO_OBJ_MAP_H

#include "header_obj.h"

#define MAP_LOAD_PERCENT 0.8

typedef struct {
    Value key;
    Value value;
} Entry;

typedef struct my_obj_map {
    ObjHeader objHeader;
    uint32 cap;
    uint32 cnt;
    Entry* entryArr;
} ObjMap;

ObjMap* newObjMap(VM* vm);

void mapSet(VM* vm, ObjMap* map, Value key, Value value);

Value mapGet(VM* vm, ObjMap* map, Value key);

void mapClear(VM* vm, ObjMap* map);

Value removeKey(VM* vm, ObjMap* map, Value key);

#endif //SPARROW_GO_OBJ_MAP_H
