//
// Created by gogotime on 2022/5/29.
//

#ifndef SPARROW_GO_TOKEN_H
#define SPARROW_GO_TOKEN_H

typedef enum {
    TOKEN_UNKNOWN,
    TOKEN_NUM,            // number
    TOKEN_STRING,         // string
    TOKEN_ID,             // variable
    TOKEN_INTERPOLATION,  // interpolation

    TOKEN_KW_APPEND,      // append
    TOKEN_KW_BREAK,       // break
    TOKEN_KW_CASE,        // case
    TOKEN_KW_CHAN,        // chan
    TOKEN_KW_CONST,       // const
    TOKEN_KW_CONTINUE,    // continue
    TOKEN_KW_DEFAULT,     // default
    TOKEN_KW_DEFER,       // defer
    TOKEN_KW_ELSE,        // else
    TOKEN_KW_FALLTHROUGH, // fallthrough
    TOKEN_KW_FOR,         // for
    TOKEN_KW_FUNC,        // func
    TOKEN_KW_GO,          // go
    TOKEN_KW_GOTO,        // goto
    TOKEN_KW_IF,          // if
    TOKEN_KW_IMPORT,      // import
    TOKEN_KW_INTERFACE,   // interface
    TOKEN_KW_MAP,         // map
    TOKEN_KW_PACKAGE,     // package
    TOKEN_KW_RANGE,       // range
    TOKEN_KW_RETURN,      // return
    TOKEN_KW_SELECT,      // select
    TOKEN_KW_STRUCT,      // struct
    TOKEN_KW_SWITCH,      // switch
    TOKEN_KW_TYPE,        // type
    TOKEN_KW_VAR,         // var

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

#endif //SPARROW_GO_TOKEN_H
