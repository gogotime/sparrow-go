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
    int symbolIndex = getIndexFromSymbolTable(&objModule->moduleVarName, name, length);
    if (symbolIndex == -1) {
        symbolIndex = addSymbol(vm, &objModule->moduleVarName, name, length);
        ValueBufferAdd(vm, &objModule->moduleVarValue, value);
    } else if (VALUE_IS_NUM(objModule->moduleVarValue.data[symbolIndex])) {
        objModule->moduleVarValue.data[symbolIndex] = value;
    } else {
        symbolIndex = -1;
    }

    return symbolIndex;
}


static ObjModule* getModule(VM* vm, Value moduleName) {
    Value value = mapGet(vm, vm->allModules, moduleName);
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
    IntBufferAdd(cu->curParser->vm, &cu->fn->debug->lineNo,cu->curParser->preToken.lineNo);
#endif

    ByteBufferAdd(cu->curParser->vm, &cu->fn->instrStream, (uint8) byte);
    return (int) cu->fn->instrStream.cnt - 1;
}

static void writeOpCode(CompileUnit* cu, OpCode opCode) {
    writeByte(cu, opCode);
    cu->stackSlotNum += opCodeSlotsUsed[opCode];
    if (cu->stackSlotNum > cu->fn->maxStackSlotUserNum) {
        cu->fn->maxStackSlotUserNum = cu->stackSlotNum;
    }
}

inline static int writeByteOperand(CompileUnit* cu, int operand) {
    return writeByte(cu, operand);
}

// write 2 opCode in big endian
inline static int writeShortOperand(CompileUnit* cu, int operand) {
    writeByte(cu, (operand >> 8) & 0xff);
    return writeByte(cu, operand & 0xff);
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

    printf("there is something to do ");

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

//VMResult executeModule(VM* vm, Value moduleName, const char* moduleCode) {
//    ObjThread* objThread = loadModule(vm, moduleName, moduleCode);
//    return VM_RESULT_ERROR;
//}

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

int ensureSymbolTable(VM* vm, SymbolTable* table, const char* symbol, uint32 length) {
    int symbolIdx = getIndexFromSymbolTable(table, symbol, length);
    if (symbolIdx == -1) {
        return addSymbol(vm, table, symbol, length);
    }
    return symbolIdx;
}

static uint32 signToString(Signature* sign, char* buf) {
    uint32 pos = 0;
    memcpy(buf + pos, sign->name, sign->length);
    pos += sign->length;
    switch (sign->type) {
        case SIGH_METHOD:
            buf[pos++] = '(';
            uint32 idx = 0;
            while (idx < sign->argNum) {
                buf[pos++] = '_';
                buf[pos++] = ',';
                idx++;
            }
            if (idx == 0) {
                buf[pos++] = ')';
            } else {
                buf[pos - 1] = ')';
            }
            break;
        default:
            RUNTIME_ERROR("should not reach here");
    }
    buf[pos] = '\0';
    return pos;
}

static void expression(CompileUnit* cu, BindPower rbp) {
    DenotationFn nud = bindRule[cu->curParser->curToken.type].nud;
    ASSERT(nud != nil, "nud is nil")

    getNextToken(cu->curParser);

    bool canAssign = rbp < BP_ASSIGN;

    nud(cu, canAssign);

    while (rbp < bindRule[cu->curParser->curToken.type].lbp) {
        DenotationFn led = bindRule[cu->curParser->curToken.type].led;
        getNextToken(cu->curParser);
        led(cu, canAssign);
    }

}

static void emitCallBySignature(CompileUnit* cu, Signature* sign, OpCode opCode) {
    char signBuf[MAX_SIGN_LEN];
    uint32 length = signToString(sign, signBuf);
    int symbolIdx = ensureSymbolTable(cu->curParser->vm, &cu->curParser->vm->allMethodName, signBuf, length);
    writeOpCodeShortOperand(cu, opCode + sign->argNum, symbolIdx);
}

static void emitCall(CompileUnit* cu, int argNum, const char* name, int length) {
    int symbolIdx = ensureSymbolTable(cu->curParser->vm, &cu->curParser->vm->allMethodName, name, length);
    writeOpCodeShortOperand(cu, OPCODE_CALL0 + argNum, symbolIdx);
}

static void infixOperator(CompileUnit* cu, bool canAssign UNUSED) {
    SymbolBindRule* rule = &bindRule[cu->curParser->preToken.type];

    BindPower rbp = rule->lbp;
    expression(cu, rbp);

    Signature sign = {SIGH_METHOD, rule->id, strlen(rule->id), 1};
    emitCallBySignature(cu, &sign, OPCODE_CALL0);
}

static void unaryOperator(CompileUnit* cu, bool canAssign UNUSED) {
    SymbolBindRule* rule = &bindRule[cu->curParser->preToken.type];

    expression(cu, BP_UNARY);

    emitCall(cu, 0, rule->id, 1);
}

static uint32 addLocalVar(CompileUnit* cu, const char* name, uint32 length) {
    LocalVar* var = &(cu->localVar[cu->localValNum]);
    var->name = name;
    var->length = length;
    var->scopeDepth = cu->scopeDepth;
    var->isUpValue = false;
    return cu->localValNum++;
}

static int declareLocalVar(CompileUnit* cu, const char* name, uint32 length) {
    if (cu->localValNum > MAX_LOCAL_VAR_NUM) {
        COMPILE_ERROR(cu->curParser, "the number of local var out of limit, scope is %d", cu->scopeDepth);
    }

    int idx = (int) cu->localValNum - 1;
    while (idx >= 0) {
        LocalVar* var = &cu->localVar[idx];
        if (var->scopeDepth < cu->scopeDepth) {
            break;
        }

        if (var->length == length && memcmp(var->name, name, length) == 0) {
            char id[MAX_ID_LEN] = {0};
            memcpy(id, name, length);
            COMPILE_ERROR(cu->curParser, "identifier %s redefinition", id);
        }
        idx--;
    }
    return addLocalVar(cu, name, length);

}

static void unaryMethodSignature(CompileUnit* cu UNUSED, Signature* sign UNUSED) {
    sign->type = SIGH_METHOD;
}

static void infixMethodSignature(CompileUnit* cu, Signature* sign) {
    sign->type = SIGH_METHOD;

    sign->argNum = 1;

    consumeCurToken(cu->curParser, TOKEN_LEFT_PAREN, "expect '(' after infix operator!");
    consumeCurToken(cu->curParser, TOKEN_ID, "expect variable name!");
    declareLocalVar(cu, cu->curParser->preToken.start, cu->curParser->preToken.length);
    consumeCurToken(cu->curParser, TOKEN_RIGHT_PAREN, "expect ')' after parameter!");

}

static void mixMethodSignature(CompileUnit* cu, Signature* sign) {
    sign->type = SIGH_METHOD;

    if (matchToken(cu->curParser, TOKEN_LEFT_PAREN)) {
        sign->type = SIGH_METHOD;
        sign->argNum = 1;
        consumeCurToken(cu->curParser, TOKEN_LEFT_PAREN, "expect '(' after infix operator!");
        declareLocalVar(cu, cu->curParser->preToken.start, cu->curParser->preToken.length);
        consumeCurToken(cu->curParser, TOKEN_RIGHT_PAREN, "expect ')' after parameter!");
    }
}

static int findLocal(CompileUnit* cu, const char* name, uint32 length) {
    int index = (int) cu->localValNum - 1;
    while (index >= 0) {
        if (cu->localVar[index].length == length && memcmp(cu->localVar[index].name, name, length) == 0) {
            return index;
        }
    }
    return -1;
}

static int addUpValue(CompileUnit* cu, bool isEnclosingLocalVar, uint32 index) {
    uint32 idx = 0;
    while (idx < cu->fn->upValueNum) {
        if (cu->upValue[idx].index == index && cu->upValue[idx].isEnclosingLocalVar == isEnclosingLocalVar) {
            return (int) idx;
        }
        idx++;
    }

    cu->upValue[cu->fn->upValueNum].isEnclosingLocalVar = isEnclosingLocalVar;
    cu->upValue[cu->fn->upValueNum].index = index;
    return (int) cu->fn->upValueNum++;
}

static int findUpValue(CompileUnit* cu, const char* name, uint32 length) {
    if (cu->enclosingUnit == nil) {
        return -1;
    }

    if (!strchr(name, ' ') && cu->enclosingUnit->enclosingClassBK != nil) {
        return -1;
    }

    int directOuterLocalIndex = findLocal(cu->enclosingUnit, name, length);

    if (directOuterLocalIndex != -1) {
        cu->enclosingUnit->localVar[directOuterLocalIndex].isUpValue = true;
        return addUpValue(cu, true, (uint32) directOuterLocalIndex);
    }

    int directOuterUpValue = findUpValue(cu->enclosingUnit, name, length);

    if (directOuterUpValue != -1) {
        return addUpValue(cu, false, (uint32) directOuterLocalIndex);
    }
    return -1;

}

static Variable getVarFromLocalOrUpValue(CompileUnit* cu, const char* name, uint32 length) {
    Variable var;
    var.scopeType = VAR_SCOPE_INVALID;

    var.index = findLocal(cu, name, length);
    if (var.index != -1) {
        var.scopeType = VAR_SCOPE_LOCAL;
        return var;
    }

    var.index = findUpValue(cu, name, length);
    if (var.index != -1) {
        var.scopeType = VAR_SCOPE_UPVALUE;
    }
    return var;
}

static void emitLoadVariable(CompileUnit* cu, Variable var) {
    switch (var.scopeType) {
        case VAR_SCOPE_LOCAL:
            writeOpCodeByteOperand(cu, OPCODE_LOAD_LOCAL_VAR, var.index);
            break;
        case VAR_SCOPE_UPVALUE:
            writeOpCodeByteOperand(cu, OPCODE_LOAD_UPVALUE, var.index);
            break;
        case VAR_SCOPE_MODULE:
            writeOpCodeByteOperand(cu, OPCODE_LOAD_MODULE_VAR, var.index);
            break;
        default:
            NOT_REACHED("unknown scope type")
    }
}

static void emitStoreVariable(CompileUnit* cu, Variable var) {
    switch (var.scopeType) {
        case VAR_SCOPE_LOCAL:
            writeOpCodeByteOperand(cu, OPCODE_STORE_LOCAL_VAR, var.index);
            break;
        case VAR_SCOPE_UPVALUE:
            writeOpCodeByteOperand(cu, OPCODE_STORE_UPVALUE, var.index);
            break;
        case VAR_SCOPE_MODULE:
            writeOpCodeByteOperand(cu, OPCODE_STORE_MODULE_VAR, var.index);
            break;
        default:
            NOT_REACHED("unknown scope type")
    }
}

static void emitLoadOrStoreVariable(CompileUnit* cu, bool canAssign, Variable var) {
    if (canAssign && matchToken(cu->curParser, TOKEN_ASSIGN)) {
        expression(cu, BP_LOWEST);
        emitStoreVariable(cu, var);
    } else {
        emitLoadVariable(cu, var);
    }
}

static void compileBlock(CompileUnit* cu) {
    while (!matchToken(cu->curParser, TOKEN_RIGHT_BRACE)) {
        if (PEEK_TOKEN(cu->curParser) == TOKEN_EOF) {
            COMPILE_ERROR(cu->curParser, "expect ')' at the end of the block!");
        }
        compileProgram(cu);
    }
}

static bool isLocalName(const char* name) {
    return (name[0] >= 'a' && name[0] <= 'z');
}

static void compileStatement(CompileUnit* cu) {
    expression(cu, BP_LOWEST);
    writeOpCode(cu, OPCODE_POP);
}



