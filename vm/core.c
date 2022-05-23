//
// Created by gogotime on 2022/5/4.
//

#include <sys/stat.h>

#include "core.h"
#include "vm.h"
#include "../include/utils.h"
#include "../object/obj_map.h"
#include "../object/header_obj.h"
#include "../object/class.h"
#include "../compiler/compiler.h"
#include <string.h>


char* rootDir = NULL;

#define RET_VALUE(value) \
do{                      \
    args[0]=value;       \
    return true;         \
}while(0)

#define RET_OBJ(objPtr) RET_VALUE(OBJ_TO_VALUE(objPtr))
#define RET_BOOL(boolean) RET_VALUE(BOOL_TO_VALUE(boolean))
#define RET_NUM(num) RET_VALUE(NUM_TO_VALUE(num))
#define RET_NULL RET_VALUE(VT_TO_VALUE(VT_NULL))
#define RET_TRUE RET_VALUE(VT_TO_VALUE(VT_TRUE))
#define RET_FALSE RET_VALUE(VT_TO_VALUE(VT_FALSE))

#define SET_ERROR_FALSE(vmPtr, errMsg) \
do{                                    \
    (vmPtr)->curThread->errorObj=OBJ_TO_VALUE(newObjString(vmPtr,errMsg,strlen(errMsg))); \
    return false;                      \
                                       \
}while(0);


#define PRIM_METHOD_BIND(classPtr, methodName, func){ \
    uint32 length=strlen(methodName);                 \
    int globalIdx=getIndexFromSymbolTable(&vm->allMethodNames,methodName,length); \
    if(globalIdx==-1){                                \
         globalIdx=addSymbol(vm, &vm->allMethodNames,methodName,length);          \
    }                                                 \
    Method method;                                    \
    method.Type=MT_PRIMITIVE;                         \
    method.PrimFn=func;                               \
    bindMethod(vm,classPtr,(uint32)globalIdx,method); \
                                                      \
}

static bool primObjectNot(VM* vm UNUSED, Value* args) {
    RET_VALUE(VT_TO_VALUE(VT_FALSE));
}

static bool primObjectEqual(VM* vm UNUSED, Value* args) {
    Value boolValue = BOOL_TO_VALUE(valueIsEqual(args[0], args[1]));
    RET_VALUE(boolValue);
}

static bool primObjectNotEqual(VM* vm UNUSED, Value* args) {
    Value boolValue = BOOL_TO_VALUE(!valueIsEqual(args[0], args[1]));
    RET_VALUE(boolValue);
}

static bool primObjectIs(VM* vm, Value* args) {
    if (!VALUE_IS_CLASS(args[1])) {
        RUNTIME_ERROR("argument must be class!");
    }

    Class* thisClass = getClassOfObj(vm, args[0]);
    Class* baseClass = (Class*) (args[1].objHeader);

    while (baseClass != nil) {
        if (thisClass == baseClass) {
            RET_VALUE(VT_TO_VALUE(VT_TRUE));
        }
        baseClass = baseClass->superClass;
    }
    RET_VALUE(VT_TO_VALUE(VT_FALSE));
}

static bool primObjectToString(VM* vm UNUSED, Value* args) {
    Class* aClass = args[0].objHeader->class;
    Value nameValue = OBJ_TO_VALUE(aClass->name);
    RET_VALUE(nameValue);
}

static bool primObjectType(VM* vm, Value* args) {
    Class* aClass = getClassOfObj(vm, args[0]);
    RET_OBJ(aClass);
}

static bool primObjectName(VM* vm, Value* args) {
    RET_OBJ(VALUE_TO_CLASS(args[0])->name);
}

static bool primClassSupertype(VM* vm, Value* args) {
    Class* aClass = VALUE_TO_CLASS(args[0]);
    if (aClass->superClass != nil) {
        RET_OBJ(aClass->superClass);
    }
    RET_VALUE(VT_TO_VALUE(VT_NULL));
}

static bool primClassToString(VM* vm, Value* args) {
    RET_OBJ(VALUE_TO_CLASS(args[0])->name);
}

static bool primObjectMetaSame(VM* vm, Value* args) {
    Value boolValue = BOOL_TO_VALUE(valueIsEqual(args[0], args[1]));
    RET_VALUE(boolValue);
}


static Class* defineClass(VM* vm, ObjModule* objModule, const char* name) {
    Class* aClass = newRawClass(vm, name, 0);
    defineModuleVar(vm, objModule, name, strlen(name), OBJ_TO_VALUE(aClass));
    return aClass;
}

int getIndexFromSymbolTable(SymbolTable* table, const char* symbol, uint32 length) {
    ASSERT(length != 0, "length of symbol is 0");
    uint32 index = 0;
    while (index < table->cnt) {
        if (length == table->data[index].length && memcmp(table->data[index].str, symbol, length) == 0) {
            return index;
        }
    }
    return -1;
}

int addSymbol(VM* vm, SymbolTable* table, const char* symbol, uint32 length) {
    ASSERT(length != 0, "length of symbol is 0");
    String string;
    string.str = ALLOCATE_ARRAY(vm, char, length + 1);
    memcpy(string.str, symbol, length);
    string.str[length] = 0;
    string.length = length;
    StringBufferAdd(vm, table, string);
    return (int) table->cnt - 1;

}

VMResult executeModule(VM* vm, Value moduleName, const char* moduleCode) {
    return VM_RESULT_ERROR;
}


void bindMethod(VM* vm, Class* aClass, uint32 index, Method method) {
    if (index >= aClass->method.cnt) {
        Method emptyPad = {MT_NONE, {0}};
        MethodBufferFillWrite(vm, &aClass->method, emptyPad, index - aClass->method.cnt + 1);
    }
    aClass->method.data[index] = method;
}

void bindSuperClass(VM* vm, Class* subClass, Class* superClass) {
    subClass->superClass = superClass;
    subClass->fieldNum += superClass->fieldNum;
    uint32 idx = 0;
    while (idx < superClass->method.cnt) {
        bindMethod(vm, subClass, idx, subClass->method.data[idx]);
        idx++;
    }
}

void buildCore(VM* vm) {
    ObjModule* coreModule = newObjModule(vm, nil);
    mapSet(vm, vm->allModules, CORE_MODULE, OBJ_TO_VALUE(coreModule));

}

char* readFile(const char* path) {
    FILE* file = fopen(path, "r");
    if (file == nil) {
        IO_ERROR("couldn't open file \"%s\"", path);
    }

    struct stat fileStat;
    stat(path, &fileStat);
    size_t fileSize = fileStat.st_size;
    char* fileContent = (char*) malloc(fileSize + 1);
    if (fileContent == nil) {
        MEM_ERROR("failed to malloc memory for file \"%s\"", path);
    }
    size_t numRead = fread(fileContent, sizeof(char), fileSize, file);
    if (numRead < fileSize) {
        IO_ERROR("read file \"%s\" failed", path);
    }
    fileContent[fileSize] = '\0';
    fclose(file);
    return fileContent;
}
