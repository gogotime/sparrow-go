//
// Created by gogotime on 2022/5/5.
//

#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include "ctype.h"
#include "../include/common.h"
#include "../include/utils.h"
#include "../object/class.h"

KeywordToken keywordTokenMap[] = {
        {"append",      6,  TOKEN_KW_APPEND},
        {"break",       5,  TOKEN_KW_BREAK},
        {"case",        4,  TOKEN_KW_CASE},
        {"chan",        4,  TOKEN_KW_CHAN},
        {"const",       5,  TOKEN_KW_CONST},
        {"continue",    8,  TOKEN_KW_CONTINUE},
        {"default",     7,  TOKEN_KW_DEFAULT},
        {"defer",       5,  TOKEN_KW_DEFER},
        {"else",        4,  TOKEN_KW_ELSE},
        {"fallthrough", 11, TOKEN_KW_FALLTHROUGH},
        {"for",         3,  TOKEN_KW_FOR},
        {"func",        4,  TOKEN_KW_FUNC},
        {"go",          2,  TOKEN_KW_GO},
        {"goto",        4,  TOKEN_KW_GOTO},
        {"if",          2,  TOKEN_KW_IF},
        {"import",      6,  TOKEN_KW_IMPORT},
        {"interface",   9,  TOKEN_KW_INTERFACE},
        {"map",         3,  TOKEN_KW_MAP},
        {"package",     7,  TOKEN_KW_PACKAGE},
        {"range",       5,  TOKEN_KW_RANGE},
        {"return",      6,  TOKEN_KW_RETURN},
        {"select",      6,  TOKEN_KW_SELECT},
        {"struct",      6,  TOKEN_KW_STRUCT},
        {"switch",      6,  TOKEN_KW_SWITCH},
        {"type",        4,  TOKEN_KW_TYPE},
        {"var",         3,  TOKEN_KW_VAR},
        {NULL,          0,  TOKEN_UNKNOWN},
};

char* tokenTypeMap[] = {
        toString(TOKEN_UNKNOWN),
        toString(TOKEN_NUM),            // number
        toString(TOKEN_STRING),         // string
        toString(TOKEN_ID),             // variable
        toString(TOKEN_INTERPOLATION),  // interpolation

        toString(TOKEN_KW_APPEND),      // append
        toString(TOKEN_KW_BREAK),    // break
        toString(TOKEN_KW_CASE),      // case
        toString(TOKEN_KW_CHAN),     // chan
        toString(TOKEN_KW_CONST),    // const
        toString(TOKEN_KW_CONTINUE), // continue
        toString(TOKEN_KW_DEFAULT),      // default
        toString(TOKEN_KW_DEFER),      // defer
        toString(TOKEN_KW_ELSE),     // else
        toString(TOKEN_KW_FALLTHROUGH),      // fallthrough
        toString(TOKEN_KW_FOR),      // for
        toString(TOKEN_KW_FUNC),     // func
        toString(TOKEN_KW_GO),      // go
        toString(TOKEN_KW_GOTO),      // goto
        toString(TOKEN_KW_IF),       // if
        toString(TOKEN_KW_IMPORT),   // import
        toString(TOKEN_KW_INTERFACE),// interface
        toString(TOKEN_KW_MAP),      // map
        toString(TOKEN_KW_PACKAGE),  // package
        toString(TOKEN_KW_RANGE),    // range
        toString(TOKEN_KW_RETURN),   // return
        toString(TOKEN_KW_SELECT),      // select
        toString(TOKEN_KW_STRUCT),   // struct
        toString(TOKEN_KW_SWITCH),      // switch
        toString(TOKEN_KW_TYPE),     // type
        toString(TOKEN_KW_VAR),      // var

        toString(TOKEN_COMMA),         // ),
        toString(TOKEN_COLON),         // :
        toString(TOKEN_LEFT_PAREN),    // (
        toString(TOKEN_RIGHT_PAREN),   // )
        toString(TOKEN_LEFT_BRACKET),  // [
        toString(TOKEN_RIGHT_BRACKET), // ]
        toString(TOKEN_LEFT_BRACE),    // {
        toString(TOKEN_RIGHT_BRACE),   // }
        toString(TOKEN_DOT),           // .
        toString(TOKEN_DOT_DOT),       // ..

        toString(TOKEN_ADD), // +
        toString(TOKEN_SUB), // -
        toString(TOKEN_MUL), // *
        toString(TOKEN_DIV), // /
        toString(TOKEN_MOD), // %

        toString(TOKEN_ASSIGN), // =

        toString(TOKEN_BIT_AND),         // &
        toString(TOKEN_BIT_OR),          // |
        toString(TOKEN_BIT_NOT),         // ~
        toString(TOKEN_BIT_SHIFT_RIGHT), // >>
        toString(TOKEN_BIT_SHIFT_LEFT),  // <<数字>>

        toString(TOKEN_LOGIC_AND), // &&
        toString(TOKEN_LOGIC_OR),  // ||
        toString(TOKEN_LOGIC_NOT), // !

        toString(TOKEN_EQUAL),       // ==
        toString(TOKEN_NOT_EQUAL),   // !=
        toString(TOKEN_GREAT),       // >
        toString(TOKEN_GREAT_EQUAL), // >=
        toString(TOKEN_LESS),        // <
        toString(TOKEN_LESS_EQUAL),  // <=

        toString(TOKEN_QUESTION),    // ?
        toString(TOKEN_EOF) // EOF
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

static void parseHexNum(Parser* parser) {
    while (isxdigit(parser->curChar)) {
        getNextChar(parser);
    }
}

static void parseDecNum(Parser* parser) {
    while (isdigit(parser->curChar)) {
        getNextChar(parser);
    }

    if (parser->curChar == '.' && isdigit(lookAheadChar(parser))) {
        getNextChar(parser);
        while (isdigit(parser->curChar)) {
            getNextChar(parser);
        }
    }
}

static void parseOctNum(Parser* parser) {
    while (parser->curChar >= '0' && parser->curChar <= '8') {
        getNextChar(parser);
    }
}

static void parseNum(Parser* parser) {
    if (parser->curChar == '0' && matchNextChar(parser, 'x')) {
        getNextChar(parser);
        parseHexNum(parser);
        parser->curToken.value = NUM_TO_VALUE(strtol(parser->curToken.start, NULL, 16));
    } else if (parser->curChar == '0' && isdigit(lookAheadChar(parser))) {
        parseOctNum(parser);
        parser->curToken.value = NUM_TO_VALUE(strtol(parser->curToken.start, NULL, 8));
    } else {
        parseDecNum(parser);
        parser->curToken.value = NUM_TO_VALUE(strtod(parser->curToken.start, NULL));
    }

    parser->curToken.length = (uint32) (parser->nextCharPtr - parser->curToken.start - 1);
    parser->curToken.type = TOKEN_NUM;
}

static void parseId(Parser* parser, TokenType type) {
    while (isalnum(parser->curChar) || parser->curChar == '_') {
        getNextChar(parser);
    }
    uint32 length = (uint32) (parser->nextCharPtr - parser->curToken.start - 1);
    if (type != TOKEN_UNKNOWN) {
        parser->curToken.type = type;
    } else {
        parser->curToken.type = idOrKeyword(parser->curToken.start, length);
    }
    parser->curToken.length = length;
}

static void parseUnicodeCodePoint(Parser* parser, ByteBuffer* buf) {
    uint32 idx = 0;
    int value = 0;
    uint8 digit = 0;
    while (idx++ < 4) {
        getNextChar(parser);
        if (parser->curChar == '\0') {
            LEX_ERROR(parser, "unterminated unicode!");
        } else if (parser->curChar >= '0' && parser->curChar <= '9') {
            digit = parser->curChar - '0';
        } else if (parser->curChar >= 'a' && parser->curChar <= 'f') {
            digit = parser->curChar - 'a' + 10;
        } else if (parser->curChar >= 'A' && parser->curChar <= 'F') {
            digit = parser->curChar - 'A' + 10;
        } else {
            LEX_ERROR(parser, "invalid unicode!");
        }
        value = value * 16 | digit;
    }

    uint32 byteNum = getByteNumOfEncodeUtf8(value);
    ASSERT(byteNum != 0, "utf8 encode byte should between 1 and 8")

    ByteBufferFillWrite(parser->vm, buf, 0, byteNum);

    encodeUtf8(buf->data + buf->cnt - byteNum, value);

}

static void parseString(Parser* parser) {
    ByteBuffer str;
    ByteBufferInit(&str);
    while (true) {
        getNextChar(parser);
        if (parser->curChar == '\0') {
            LEX_ERROR(parser, "unterminated string");
        }

        if (parser->curChar == '"') {
            parser->curToken.type = TOKEN_STRING;
            break;
        }
        if (parser->curChar == '%') {
            if (!matchNextChar(parser, '(')) {
                LEX_ERROR(parser, "'%' should followed by '('");
            }
            if (parser->interpolationExpectRightParenNum > 0) {
                COMPILE_ERROR(parser, "don't support inner interpolation expression!");
            }
            parser->interpolationExpectRightParenNum = 1;
            parser->curToken.type = TOKEN_INTERPOLATION;
            break;
        }

        if (parser->curChar == '\\') {
            getNextChar(parser);
            switch (parser->curChar) {
                case '0':
                    ByteBufferAdd(parser->vm, &str, '\0');
                    break;
                case 'a':
                    ByteBufferAdd(parser->vm, &str, '\a');
                    break;
                case 'b':
                    ByteBufferAdd(parser->vm, &str, '\b');
                    break;
                case 'f':
                    ByteBufferAdd(parser->vm, &str, '\f');
                    break;
                case 'n':
                    ByteBufferAdd(parser->vm, &str, '\n');
                    break;
                case 'r':
                    ByteBufferAdd(parser->vm, &str, '\r');
                    break;
                case 't':
                    ByteBufferAdd(parser->vm, &str, '\t');
                    break;
                case 'u':
                    parseUnicodeCodePoint(parser, &str);
                    break;
                case '"':
                    ByteBufferAdd(parser->vm, &str, '"');
                    break;
                case '\\':
                    ByteBufferAdd(parser->vm, &str, '\\');
                    break;
                default:
                    LEX_ERROR(parser, "unsupported char %c", parser->curChar);

            }
        } else {
            ByteBufferAdd(parser->vm, &str, parser->curChar);
        }
    }
    ObjString* objString = newObjString(parser->vm, (const char*) str.data, str.cnt);
    parser->curToken.value = OBJ_TO_VALUE(objString);
    ByteBufferClear(parser->vm, &str);
}

static void skipLine(Parser* parser) {
    getNextChar(parser);
    while (parser->curChar != '\0') {
        if (parser->curChar == '\n') {
            parser->curToken.lineNo++;
            getNextChar(parser);
            return;
        }
        getNextChar(parser);
    }
    LEX_ERROR(parser->vm, "skip a line meet 0");
}

static void skipComment(Parser* parser) {
    char nextChar = lookAheadChar(parser);
    if (parser->curChar == '/') {
        skipLine(parser);
    } else {
        while (nextChar != '*' && nextChar != '0') {
            getNextChar(parser);
            if (parser->curChar == '\n') {
                parser->curToken.lineNo++;
            }
            nextChar = lookAheadChar(parser);
        }
        if (matchNextChar(parser, '*')) {
            if (!matchNextChar(parser, '/')) {
                LEX_ERROR(parser, "expect '/' after \"/* ... *\"");
            }
            getNextChar(parser);
        } else {
            LEX_ERROR(parser, "expect \"*/\" before file end!");
        }
    }
    skipBlanks(parser);

}

void getNextToken(Parser* parser) {
    parser->preToken = parser->curToken;
    skipBlanks(parser);
    parser->curToken.type = TOKEN_UNKNOWN;
    parser->curToken.length = 0;
    parser->curToken.start = parser->nextCharPtr - 1;
    parser->curToken.value = VT_TO_VALUE(VT_UNDEFINED);
    while (parser->curChar != '\0') {
        switch (parser->curChar) {
            case ',':
                parser->curToken.type = TOKEN_COMMA;
                break;
            case ':':
                parser->curToken.type = TOKEN_COLON;
                break;
            case '(':
                if (parser->interpolationExpectRightParenNum > 0) {
                    parser->interpolationExpectRightParenNum++;
                }
                parser->curToken.type = TOKEN_LEFT_PAREN;
                break;
            case ')':
                if (parser->interpolationExpectRightParenNum > 0) {
                    parser->interpolationExpectRightParenNum--;
                    if (parser->interpolationExpectRightParenNum == 0) {
                        parseString(parser);
                        break;
                    }
                }
                parser->curToken.type = TOKEN_RIGHT_PAREN;
                break;
            case '[':
                parser->curToken.type = TOKEN_LEFT_BRACKET;
                break;
            case ']':
                parser->curToken.type = TOKEN_RIGHT_BRACKET;
                break;
            case '{':
                parser->curToken.type = TOKEN_LEFT_BRACE;
                break;
            case '}':
                parser->curToken.type = TOKEN_RIGHT_BRACE;
                break;
            case '.':
                if (matchNextChar(parser, '.')) {
                    parser->curToken.type = TOKEN_DOT_DOT;
                } else {
                    parser->curToken.type = TOKEN_DOT;
                }
                break;
            case '=':
                if (matchNextChar(parser, '=')) {
                    parser->curToken.type = TOKEN_EQUAL;
                } else {
                    parser->curToken.type = TOKEN_ASSIGN;
                }
                break;
            case '+':
                parser->curToken.type = TOKEN_ADD;
                break;
            case '-':
                parser->curToken.type = TOKEN_SUB;
                break;
            case '*':
                parser->curToken.type = TOKEN_MUL;
                break;
            case '/':
                if (matchNextChar(parser, '/') || matchNextChar(parser, '*')) {
                    skipComment(parser);
                    parser->curToken.start = parser->nextCharPtr - 1;
                    continue;
                }
                parser->curToken.type = TOKEN_DIV;
                break;
            case '%':
                parser->curToken.type = TOKEN_MOD;
                break;
            case '&':
                if (matchNextChar(parser, '&')) {
                    parser->curToken.type = TOKEN_LOGIC_AND;
                } else {
                    parser->curToken.type = TOKEN_BIT_AND;
                }
                break;
            case '|':
                if (matchNextChar(parser, '|')) {
                    parser->curToken.type = TOKEN_LOGIC_OR;
                } else {
                    parser->curToken.type = TOKEN_BIT_OR;
                }
                break;
            case '~':
                parser->curToken.type = TOKEN_BIT_NOT;
                break;
            case '?':
                parser->curToken.type = TOKEN_QUESTION;
                break;
            case '>':
                if (matchNextChar(parser, '=')) {
                    parser->curToken.type = TOKEN_GREAT_EQUAL;
                } else if (matchNextChar(parser, '>')) {
                    parser->curToken.type = TOKEN_BIT_SHIFT_RIGHT;
                } else {
                    parser->curToken.type = TOKEN_GREAT;
                }
                break;
            case '<':
                if (matchNextChar(parser, '=')) {
                    parser->curToken.type = TOKEN_LESS_EQUAL;
                } else if (matchNextChar(parser, '<')) {
                    parser->curToken.type = TOKEN_BIT_SHIFT_LEFT;
                } else {
                    parser->curToken.type = TOKEN_LESS;
                }
                break;
            case '!':
                if (matchNextChar(parser, '=')) {
                    parser->curToken.type = TOKEN_NOT_EQUAL;
                } else {
                    parser->curToken.type = TOKEN_LOGIC_NOT;
                }
                break;
            case '"':
                parseString(parser);
                break;
            default:
                if (isalpha(parser->curChar) || parser->curChar == '_') {
                    parseId(parser, TOKEN_UNKNOWN);
                } else if (isdigit(parser->curChar)) {
                    parseNum(parser);
                } else {
                    if (parser->curChar == '#' && matchNextChar(parser, '!')) {
                        skipLine(parser);
                        parser->curToken.start = parser->nextCharPtr - 1;
                        continue;
                    }
                }
                return;
        }
        parser->curToken.length = (uint32) (parser->nextCharPtr - parser->curToken.start);
        getNextChar(parser);
        return;
    }
    parser->curToken.type = TOKEN_EOF;
    parser->curToken.length = 0;
}

bool matchToken(Parser* parser, TokenType expected) {
    if (parser->curToken.type == expected) {
        getNextToken(parser);
        return true;
    }
    return false;
}

void consumeCurToken(Parser* parser, TokenType expected, const char* errMsg) {
    if (parser->curToken.type != expected) {
        COMPILE_ERROR(parser, errMsg);
    }
    getNextToken(parser);
}

void consumeNextToken(Parser* parser, TokenType expected, const char* errMsg) {
    getNextToken(parser);
    if (parser->curToken.type != expected) {
        COMPILE_ERROR(parser, errMsg);
    }
}

static void initParser(VM* vm, Parser* parser, const char* file, const char* sourceCode, ObjModule* curModule) {
    parser->file = file;
    parser->sourceCode = sourceCode;
    parser->curChar = *(parser->sourceCode);
    parser->nextCharPtr = parser->sourceCode + 1;
    parser->curToken.lineNo = 1;
    parser->curToken.type = TOKEN_UNKNOWN;
    parser->curToken.start = parser->sourceCode;
    parser->curToken.length = 0;
    parser->preToken = parser->curToken;
    parser->interpolationExpectRightParenNum = 0;
    parser->vm = vm;
    parser->curModule = curModule;
}

Parser* newParser(VM* vm, const char* file, const char* sourceCode) {
    Parser* parser = malloc(sizeof(parser));
    initParser(vm, parser, file, sourceCode, NULL);
    return parser;
}