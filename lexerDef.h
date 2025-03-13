#ifndef LEXERDEF_H
#define LEXERDEF_H
#define BUFFER_SIZE 512

#include <stdio.h>
#include <stdlib.h>

typedef struct TwinBuffer twin_buffer;
typedef struct Token token;

typedef enum {
    TK_ASSIGNOP,
    TK_COMMENT,
    TK_FIELDID,
    TK_ID,
    TK_NUM,
    TK_RNUM,
    TK_FUNID,
    TK_RUID,
    TK_WITH,
    TK_PARAMETERS,
    TK_END,
    TK_WHILE,
    TK_UNION,
    TK_ENDUNION,
    TK_SQL,
    TK_SQR,
    TK_COMMA,
    TK_SEM,
    TK_COLON,
    TK_DOT,
    TK_OP,
    TK_CL,
    TK_PLUS,
    TK_MINUS,
    TK_MUL,
    TK_DIV,
    TK_AND,
    TK_OR,
    TK_NOT,
    TK_NE,
    TK_EQ,
    TK_DEFINETYPE,
    TK_AS,
    TK_TYPE,
    TK_MAIN,
    TK_GLOBAL,
    TK_PARAMETER,
    TK_LIST,
    TK_INPUT,
    TK_OUTPUT,
    TK_INT,
    TK_REAL,
    TK_ENDWHILE,
    TK_IF,
    TK_THEN,
    TK_ENDIF,
    TK_READ,
    TK_WRITE,
    TK_RETURN,
    TK_CALL,
    TK_RECORD,
    TK_ENDRECORD,
    TK_ELSE,
    TK_LT,
    TK_LE,
    TK_GT,
    TK_GE,
    TK_ERROR,
    EPSILON,
    DOLLAR
} token_type;

//int and float enough?
typedef union {
    int num;
    float r_num;
} value;

struct TwinBuffer {
    char* active_buffer;
    char* load_buffer;
    int forward_ptr;
    int begin_ptr;
    bool buffer_loaded;
};

struct Token {
    token_type name;
    char* lexeme;
    value value;
    bool value_is_int;
    int line_num;
};
#endif