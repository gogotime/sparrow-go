//
// Created by Galamo on 2022/5/5.
//

#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include "ctype.h"
#include "../include/common.h"
#include "../include/utils.h"

KeywordToken keywordTokenMap[] = {
        {"var",      3, TOKEN_VAR},
        {"func",     4, TOKEN_FUNC},
        {"if",       3, TOKEN_IF},
        {"else",     3, TOKEN_ELSE},
        {"true",     3, TOKEN_TRUE},
        {"false",    3, TOKEN_FALSE},
        {"for",      3, TOKEN_FOR},
        {"break",    3, TOKEN_BREAK},
        {"continue", 3, TOKEN_CONTINUE},
        {"return",   3, TOKEN_RETURN},
        {"nil",      3, TOKEN_NIL},
        {"struct",   3, TOKEN_STRUCT},
        {"import",   3, TOKEN_IMPORT},
        {NULL,       0, TOKEN_UNKNOWN},
};

static TokenType idOrKeyword(const char* start, uint32 length) {
    uint32 idx = 0;
    while (keywordTokenMap[idx].keyword != NULL) {
        if (keywordTokenMap[idx].length == length &&
            memcmp(keywordTokenMap[idx].keyword, start, length) == 0) {
            return keywordTokenMap[idx].type;
        }
        idx++;
    }
    return TOKEN_ID;
}

char lookAheadChar(Parser* parser) {
    return *(parser->nextCharPtr);
}

static void getNextChar(Parser* parser) {
    parser->curChar = *(parser->nextCharPtr);
    parser->nextCharPtr++;
}

static bool matchNextChar(Parser* parser, char expectedChar) {
    if (lookAheadChar(parser) == expectedChar) {
        getNextChar(parser);
        return true;
    }
    return false;
}

static void skipBlanks(Parser* parser) {
    while (isspace(parser->curChar)) {
        if (parser->curChar == '\n') {
            parser->curToken.lineNo++;
        }
        getNextChar(parser);
    }
}

static void parseId(Parser* parser, TokenType type) {
    while (isalnum(parser->curChar) || parser->curChar == '_') {
        getNextChar(parser);
    }
    uint32 length = (uint32)(parser->nextCharPtr - parser->curToken.start - 1);
    if (type != TOKEN_UNKNOWN) {
        parser->curToken.type = type;
    } else {
        parser->curToken.type = idOrKeyword(parser->curToken.start, length);
    }
    parser->curToken.length = length;
}

static void parseUnicodeCodePoint(Parser* parser, ByteBuffer buf) {
    uint32 idx = 0;
    int value = 0;
    uint8 digit = 0;
    while (idx++ < 4) {
        getNextChar(parser);
        if (parser->curChar == '\0') {
            LEX_ERROR(parser,"unterminated unicode!");
        }
    }

}







