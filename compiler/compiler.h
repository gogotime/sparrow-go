//
// Created by gogotime on 2022/5/23.
//

#ifndef SPARROW_GO_COMPILER_H
#define SPARROW_GO_COMPILER_H

#include "../object/class.h"
#include "../object/obj_fn.h"

#define MAX_LOCAL_VAR_NUM 128
#define MAX_UPVALUE_NUM 128
#define MAX_ID_LEN 128

#define MAX_METHOD_NAME_LEN MAX_ID_LEN
#define MAX_ARG_NUM 16

#define MAX_SIGN_LEN MAX_METHOD_NAME_LEN+MAX_ARG_NUM*2+1

#define MAX_FIELD_NUM 128

typedef struct {
    bool isEnclosingLocalVar;
    uint32 index;
} UpValue;

typedef struct {
    const char* name;
    uint32 length;
    int scopeDepth;
    bool isUpValue;
} LocalVar;

typedef enum {
    SIGH_METHOD
} SignatureType;

typedef struct {
    SignatureType type;
    const char* name;
    uint32 length;
    uint32 argNum;
} Signature;

typedef struct loop {
    int condStartIndex;
    int bodyStartIndex;
    int scopeDepth;
    int exitIndex;
    struct loop* enclosingLoop;
} Loop;

typedef struct {
    ObjString* name;
    SymbolTable fields;
    bool inStatic;
    IntBuffer instantMethod;
    IntBuffer staticMethod;
    Signature* signature;
} ClassBookKeep;

typedef enum {
    VAR_SCOPE_INVALID,
    VAR_SCOPE_LOCAL,
    VAR_SCOPE_UPVALUE,
    VAR_SCOPE_MODULE
} VarScopeType;

typedef struct {
    VarScopeType scopeType;
    int index;
} Variable;


int defineModuleVar(VM* vm, ObjModule* objModule, const char* name, uint32 length, Value value);



#endif //SPARROW_GO_COMPILER_H
