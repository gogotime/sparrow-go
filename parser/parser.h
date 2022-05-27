//
// Created by gogotime on 2022/5/5.
//

#ifndef SPARROW_GO_PARSER_H
#define SPARROW_GO_PARSER_H

#include "../include/common.h"
#include "../include/vm_parser.h"
#include "../vm/vm.h"
#include "../object/meta_obj.h"

#define toString(x) #x


typedef struct keywordToken {
    char* keyword;
    uint8 length;
    TokenType type;
} KeywordToken;


#define PEEK_TOKEN(parserPtr) parserPtr->curToken.type

void initParser(VM* vm, Parser* parser, const char* file, const char* sourceCode, ObjModule* curModule);

char lookAheadChar(Parser* parser);

void getNextToken(Parser* parser);

bool matchToken(Parser* parser, TokenType expected);

void consumeCurToken(Parser* parser, TokenType expected, const char* errMsg);

void consumeNextToken(Parser* parser, TokenType expected, const char* errMsg);

uint32 getByteNumOfEncodeUtf8(int value);

uint8 encodeUtf8(uint8* buf, int value);

Parser* newParser(VM* vm, const char* file, const char* sourceCode);

#endif //SPARROW_GO_PARSER_H
