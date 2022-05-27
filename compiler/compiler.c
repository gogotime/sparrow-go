//
// Created by gogotime on 2022/5/23.
//

#include "compiler.h"
#include <string.h>
#include "../object/obj_map.h"
#include "../object/obj_thread.h"
#include "../vm/core.h"
#include "../parser/parser.h"

#define OPCODE_SLOTS(opcode, effect) effect

static const int opCodeSlotsUsed[] = {
#include "../vm/opcode.inc"
};

#undef OPCODE_SLOTS_USER

typedef enum {
    BP_NONE,
    BP_LOWEST,
    BP_ASSIGN, // =
    BP_LOGIC_OR, // ||
    BP_LOGIC_AND, // &&
    BP_EQUAL, // == !=
    BP_CMP, // < > <= =>
    BP_BIT_OR, // |
    BP_BIT_AND, // &
    BP_BIT_SHIFT, // << >>
    BP_RANGE, // ..
    BP_TERM, // + -
    BP_FACTOR, // * / %
    BP_UNARY, // - ! ~
    BP_CALL, // . () []
    BP_HIGHEST,
} BindPower;

typedef void (* DenotationFn)(CompileUnit* cu, bool canAssign);

typedef void (* MethodSignatureFn)(CompileUnit* cu, Signature* signature);



struct compileUnit {
    ObjFn* fn;
    LocalVar localVar[MAX_LOCAL_VAR_NUM];
    uint32 localValNum;
    UpValue upValue[MAX_UPVALUE_NUM];
    int scopeDepth;
    uint32 stackSlotNum;
    Loop* curLoop;
    ClassBookKeep* enclosingClassBK;
    struct compileUnit* enclosingUnit;
    Parser* curParser;
};


int defineModuleVar(VM* vm, ObjModule* objModule, const char* name, uint32 length, Value value) {
    if (length > MAX_ID_LEN) {
        char id[MAX_ID_LEN] = {0};
        memcpy(id, name, length);
        if (vm->curParser != nil) {
            COMPILE_ERROR(vm->curParser, "length of identifier \"%s\" should be no more than %d", id, MAX_ID_LEN);
        } else {
            MEM_ERROR("length of identifier \"%s\" should be no more than %d", id, MAX_ID_LEN);
        }
    }

    int symbolIndex = getIndexFromSymbolTable(&objModule->moduleVarValue, name, length);
    if (symbolIndex == -1) {
        symbolIndex = addSymbol(vm, &objModule->moduleVarValue, name, length);
        ValueBufferAdd(vm, &objModule->moduleVarValue, value);
    } else if (VALUE_IS_NUM(objModule->moduleVarValue.data[symbolIndex])) {
        objModule->moduleVarValue.data[symbolIndex] = value;
    } else {
        symbolIndex = -1;
    }

    return symbolIndex;
}


static ObjModule* getModule(VM* vm, Value moduleName) {
    Value value = mapGet(vm->allModules, moduleName);
    if (value.type == VT_UNDEFINED) {
        return nil;
    }
    return VALUE_TO_OBJMODULE(value);
}


static void initCompileUnit(Parser* parser, CompileUnit* cu, CompileUnit* enclosingUnit, bool isMethod) {
    parser->compileUnit = cu;
    cu->curParser = parser;
    cu->enclosingUnit = enclosingUnit;
    cu->curLoop = nil;
    cu->enclosingClassBK = nil;

    if (enclosingUnit == nil) {
        cu->scopeDepth = -1;
        cu->localValNum = 0;
    } else {
        if (isMethod) {
            cu->localVar[0].name = "this";
            cu->localVar[0].length = 4;
        } else {
            cu->localVar[0].name = nil;
            cu->localVar[0].length = 0;
        }

        cu->localVar[0].scopeDepth = -1;
        cu->localVar[0].isUpValue = false;
        cu->localValNum = 1;
        cu->scopeDepth = 0;
    }
    cu->stackSlotNum = cu->localValNum;
    cu->fn = newObjFn(cu->curParser->vm, cu->curParser->curModule, cu->localValNum);
}

static int writeByte(CompileUnit* cu, int byte) {
#if DEBUG
    IntBufferAdd(cu->curParser->vm, &cu->fn->debug->lineNo,cu->curParser->preToken.lineNo)
#endif

    ByteBufferAdd(cu->curParser->vm, &cu->fn->instrStream, (uint8) byte);
    return (int) cu->fn->instrStream.cnt - 1;
}

static void writeOpCode(CompileUnit* cu, OpCode opCode) {
    writeByte(cu, opCode);
    cu->stackSlotNum += opCodeSlotsUsed[opCode];
    if (cu->stackSlotNum > cu.fn->maxStackSlotUserNum) {
        cu->fn->maxStackSlotUserNum = cu->stackSlotNum;
    }
}

inline static int writeByteOperand(CompileUnit* cu, int operand) {
    writeByte(cu, operand);
}

// write 2 opCode in big endian
inline static int writeShortOperand(CompileUnit* cu, int operand) {
    writeByte(cu, (operand >> 8) & 0xff);
    writeByte(cu, operand & 0xff);
}

static int writeOpCodeByteOperand(CompileUnit* cu, OpCode opCode, int operand) {
    writeOpCode(cu, opCode);
    return writeByteOperand(cu, operand);
}

static int writeOpCodeShortOperand(CompileUnit* cu, OpCode opCode, int operand) {
    writeOpCode(cu, opCode);
    return writeShortOperand(cu, operand);
}

static void compileProgram(CompileUnit* cu) {
    ;
}

ObjFn* compileModule(VM* vm, ObjModule* objModule, const char* moduleCode) {
    Parser parser;
    parser.parentParser = vm->curParser;
    vm->curParser = &parser;
    if (objModule->name == nil) {
        initParser(vm, &parser, "core.go", moduleCode, objModule);
    } else {
        initParser(vm, &parser, (const char*) objModule->name->value.start, moduleCode, objModule);
    }

    CompileUnit moduleCu;
    initCompileUnit(&parser, &moduleCu, nil, false);

    uint32 moduleVarNumBefore = objModule->moduleVarValue.cnt;

    getNextToken(&parser);

    while (!matchToken(&parser, TOKEN_EOF)) {
        compileProgram(&moduleCu);
    }

    printf("there is something to do ")
    exit(0);
}

static ObjThread* loadModule(VM* vm, Value moduleName, const char* moduleCode) {
    ObjModule* module = getModule(vm, moduleName);
    if (module == nil) {
        ObjString* modeName = VALUE_TO_OBJSTR(moduleName);
        ASSERT(modeName->value.start[modeName->value.length] == '\0', "string.value.start is not 0")
        module = newObjModule(vm, modeName->value.start);
        mapSet(vm, vm->allModules, moduleName, OBJ_TO_VALUE(module));

        ObjModule* coreModule = getModule(vm, CORE_MODULE);
        uint32 idx = 0;
        while (idx < coreModule->moduleVarName.cnt) {
            defineModuleVar(vm, module,
                            coreModule->moduleVarName.data[idx].str,
                            strlen(coreModule->moduleVarName.data[idx].str),
                            coreModule->moduleVarValue.data[idx]);
            idx++;
        }
    }

    ObjFn* fn = compileModule(vm, module, moduleCode);
    ObjClosure* objClosure = newObjClosure(vm, fn);
    ObjThread* objThread = newObjThread(vm, objClosure);

    return objThread;

}

VMResult executeModule(VM* vm, Value moduleName, const char* moduleCode) {
    ObjThread* objThread = loadModule(vm, moduleName, moduleCode);
    return VM_RESULT_ERROR;
}

static uint32 addConstant(CompileUnit* cu, Value constant) {
    ValueBufferAdd(cu->curParser->vm, &cu->fn->constants, constant);
    return cu->fn->constants.cnt - 1;
}

static void emitLoadConstant(CompileUnit* cu, Value constant) {
    int index = addConstant(cu, constant);
    writeOpCodeShortOperand(cu, OPCODE_LOAD_CONSTANT, index);
}

static void literal(CompileUnit* cu, bool canAssign UNUSED) {
    emitLoadConstant(cu, cu->curParser->preToken.value);
}

typedef struct {
    const char* id;
    // left bind power
    BindPower lbp;

    DenotationFn nud;

    DenotationFn led;

    MethodSignatureFn methodSign;

} SymbolBindRule;

#define PREFIX_SYMBOL(nud) {NULL,BP_NONE,nud,NULL,NULL}

#define PREFIX_OPERATOR(id) {id,BP_NONE,unaryOperator,NULL,unaryMethodSignature}

#define INFIX_OPERATOR(id, lbp) {id,lbp,NULL,infixOperator,infixMethodSignature}

#define MIX_OPERATOR(id) {id,BP_TERM,unaryOperator,infixOperator,mixMethodSignature}

#define UNUSED_RULE {NULL,BP_NONE,NULL,NULL,NULL}




SymbolBindRule bindRule[] = {
        UNUSED_RULE,
        PREFIX_SYMBOL(literal),
        PREFIX_SYMBOL(literal)
};