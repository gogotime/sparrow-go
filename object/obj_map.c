//
// Created by gogotime on 2022/5/21.
//

#include "obj_map.h"
#include "class.h"

ObjMap* newObjMap(VM* vm) {
    ObjMap* objMap = ALLOCATE(vm, ObjMap);
    initObjHeader(vm, &objMap->objHeader, OT_MAP, vm->mapClass);
    objMap->cap = 0;
    objMap->cnt = 0;
    objMap->entryArr = nil;
    return objMap;
}

static uint32 hashNum(double num) {
    Bits64 bits64;
    bits64.num = num;
    return bits64.bits32[0] ^ bits64.bits32[1];
}

static uint32 hashObj(ObjHeader* objHeader) {
    switch (objHeader->type) {
        case OT_CLASS:
            return hashString(((Class*) objHeader)->name->value.start, ((Class*) objHeader)->name->value.length);
        case OT_RANGE: {
            ObjRange* objRange = (ObjRange*) objHeader;
            return hashNum(objRange->from) ^ hashNum(objRange->to);
        }
        case OT_STRING:
            return ((ObjString*) objHeader)->hashCode;
        default:
            RUNTIME_ERROR("can't not hash the objHeader.type=%d", objHeader->type);
    }
    return 0;
}

static uint32 hashValue(Value value) {
    switch (value.type) {
        case VT_FALSE:
            return 0;
        case VT_TRUE:
            return 1;
        case VT_NUM:
            return hashNum(value.num);
        case VT_NULL:
            return -1;
        case VT_OBJ:
            return hashObj(value.objHeader);
        default:
            RUNTIME_ERROR("unsupported type to hash");
    }
    return 0;
}

static bool addEntry(Entry* entryArr, uint32 cap, Value key, Value value) {
    uint32 idx = hashValue(key) % cap;
    uint32 cnt = 0;
    while (cnt < cap) {
        if (entryArr[idx].key.type == VT_UNDEFINED) {
            entryArr[idx].key = key;
            entryArr[idx].value = value;
            return true;
        } else if (valueIsEqual(entryArr[idx].key, key)) {
            entryArr[idx].value = value;
            return false;
        }
        idx = (idx + 1) % cap;
        cnt++;
    }
    RUNTIME_ERROR("can't add element in map");
    return false;
}

static void resizeMap(VM* vm, ObjMap* objMap, uint32 newCap) {
    Entry* newEntryArr = ALLOCATE_ARRAY(vm, Entry, newCap);
    uint32 idx = 0;
    while (idx < newCap) {
        newEntryArr[idx].key = VT_TO_VALUE(VT_UNDEFINED);
        newEntryArr[idx].value = VT_TO_VALUE(VT_FALSE);
        idx++;
    }

    if (objMap->cap > 0) {
        Entry* entryArr = objMap->entryArr;
        idx = 0;
        while (idx < objMap->cap) {
            if (entryArr[idx].key.type != VT_UNDEFINED) {
                addEntry(newEntryArr, newCap, entryArr[idx].key, entryArr[idx].value);
            }
            idx++;
        }
    }

    printf("addr: %p \n", objMap->entryArr);
    DEALLOCATE_ARRAY(vm, objMap->entryArr, objMap->cap);
    objMap->entryArr = newEntryArr;
    objMap->cap = newCap;
}

static Entry* findEntry(ObjMap* objMap, Value key) {
    if (objMap->cap == 0) {
        return nil;
    }
    uint32 idx = hashValue(key) % objMap->cap;
    while (true) {
        Entry* entry;
        entry = &objMap->entryArr[idx];
        if (valueIsEqual(entry->key, key)) {
            return entry;
        } else if (VALUE_IS_UNDEFINED(entry->key) && VALUE_IS_FALSE(entry->value)) {
            return nil;
        }
        idx = (idx + 1) % objMap->cap;
    }
}

void mapSet(VM* vm, ObjMap* objMap, Value key, Value value) {
    if (objMap->cnt + 1 > objMap->cap * MAP_LOAD_PERCENT) {
        uint32 newCap = objMap->cap * CAPACITY_GROW_FACTOR;
        if (newCap < MIN_CAPACITY) {
            newCap = MIN_CAPACITY;
        }
        resizeMap(vm, objMap, newCap);
    }

    if (addEntry(objMap->entryArr, objMap->cap, key, value)) {
        objMap->cnt++;
    }
}

Value mapGet(VM* vm, ObjMap* objMap, Value key) {
    Entry* entry = findEntry(objMap, key);
    if (entry == NULL) {
        return VT_TO_VALUE(VT_UNDEFINED);
    }
    return entry->value;
}

void mapClear(VM* vm, ObjMap* objMap) {
    DEALLOCATE_ARRAY(vm, objMap->entryArr, objMap->cap);
    objMap->entryArr = nil;
    objMap->cnt = 0;
    objMap->cap = 0;
}

Value removeKey(VM* vm, ObjMap* objMap, Value key) {
    Entry* entry = findEntry(objMap, key);
    if (entry == nil) {
        return VT_TO_VALUE(VT_UNDEFINED);
    }

    Value value = entry->value;
    entry->key = VT_TO_VALUE(VT_UNDEFINED);
    entry->value = VT_TO_VALUE(VT_TRUE);

    objMap->cnt--;
    if (objMap->cnt == 0) {
        mapClear(vm, objMap);
    } else if (objMap->cnt > MIN_CAPACITY &&
               objMap->cnt < ((objMap->cap * MAP_LOAD_PERCENT) / (CAPACITY_GROW_FACTOR))) {
        uint32 newCap = objMap->cap / CAPACITY_GROW_FACTOR;
        if (newCap < MIN_CAPACITY) {
            newCap = MIN_CAPACITY;
        }
        resizeMap(vm, objMap, newCap);
    }
    return value;
}