//
// Created by Galamo on 2022/5/5.
//

#ifndef SPARROW_GO_PARSER_H
#define SPARROW_GO_PARSER_H

#include "../include/common.h"
#include "../vm/vm.h"

typedef enum {
    TOKEN_UNKNOWN,
    TOKEN_NUM,            // number
    TOKEN_STRING,         // string
    TOKEN_ID,             // variable
    TOKEN_INTERPOLATION,  // interpolation

    TOKEN_VAR,      // var
    TOKEN_FUNC,     // func
    TOKEN_IF,       // if
    TOKEN_ELSE,     // else
    TOKEN_TRUE,     // true
    TOKEN_FALSE,    // false
    TOKEN_FOR,      // for
    TOKEN_BREAK,    // break
    TOKEN_CONTINUE, // continue
    TOKEN_RETURN,   // return
    TOKEN_NIL,     // nil

    TOKEN_STRUCT,  // struct
    TOKEN_IMPORT, // import

    TOKEN_COMMA,         // ,
    TOKEN_COLON,         // :
    TOKEN_LEFT_PAREN,    // (
    TOKEN_RIGHT_PAREN,   // )
    TOKEN_LEFT_BRACKET,  // [
    TOKEN_RIGHT_BRACKET, // ]
    TOKEN_LEFT_BRACE,    // {
    TOKEN_RIGHT_BRACE,   // }
    TOKEN_DOT,           // .
    TOKEN_DOT_DOT,       // ..

    TOKEN_ADD, // +
    TOKEN_SUB, // -
    TOKEN_MUL, // *
    TOKEN_DIV, // /
    TOKEN_MOD, // %

    TOKEN_ASSIGN, // =

    TOKEN_BIT_AND,         // &
    TOKEN_BIT_OR,          // |
    TOKEN_BIT_NOT,         // ~
    TOKEN_BIT_SHIFT_RIGHT, // >>
    TOKEN_BIT_SHIFT_LEFT,  // <<数字>>

    TOKEN_LOGIC_AND, // &&
    TOKEN_LOGIC_OR,  // ||
    TOKEN_LOGIC_NOT, // !

    TOKEN_EQUAL,       // ==
    TOKEN_NOT_EQUAL,   // !=
    TOKEN_GREAT,       // >
    TOKEN_GREAT_EQUAL, // >=
    TOKEN_LESS,        // <
    TOKEN_LESS_EQUAL,  // <=

    TOKEN_QUESTION,    // ?
    TOKEN_EOF // EOF

} TokenType;

typedef struct {
    TokenType type;
    const char* start;
    uint32 length;
    uint32 lineNo;
} Token;

typedef struct _parser {
    const char* file;
    const char* sourceCode;
    const char* nextCharPtr;
    char curChar;
    Token curToken;
    Token preToken;
    int interpolationExpectRightParenNum;
    VM* vm;
} Parser;

typedef struct _keywordToken {
    char* keyword;
    uint8 length;
    TokenType type;
} KeywordToken;




#define PEEK_TOKEN(parserPtr) parserPtr->curToken.type

char lookAheadChar(Parser* parser);

void getNextToken(Parser* parser);

bool matchToken(Parser* parser, TokenType expected);

void consumeCurToken(Parser* parser, TokenType expected, const char* errMsg);

void consumeNextToken(Parser* parser, TokenType expected, const char* errMsg);

uint32 getByteNumOfEncodeUtf8(int value);

uint8 encodeUtf8(uint8* buf, int value);

void initParser(VM* vm, Parser* parser, const char* file, const char* sourceCode);

#endif //SPARROW_GO_PARSER_H
