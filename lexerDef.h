/*
Group Number : 14
Name : Avyakth Krishna Kumar   ID : 2021B3A71111P
Name : Suchit Chebolu          ID : 2021B1A72281P
Name : Abhiram H               ID : 2021B4A71134P
Name : Ankur Renduchintala     ID : 2021B5A71159P
Name : Vikram Hariharan        ID : 2022A7PS0013P
*/
#ifndef LEXERDEF_H
#define LEXERDEF_H

#define BUFFER_SIZE 2048
#define TERMINAL_COUNT 60
#define KEYWORD_END TK_ELSE
#define KEYWORD_START TK_WITH

#include <stdio.h>
#include <stdlib.h>

typedef enum TokenType token_type;
typedef struct TwinBuffer twin_buffer;
typedef struct Token token;

// Defining an enum data structure for all different tokens
enum TokenType{
    TK_ASSIGNOP,
    TK_COMMENT,
    TK_FIELDID,
    TK_ID,
    TK_NUM,
    TK_RNUM,
    TK_FUNID,
    TK_RUID,
    TK_LT,
    TK_LE,
    TK_GT,
    TK_GE,
    EPSILON,
    DOLLAR,
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
    TK_WITH,
    TK_PARAMETERS,
    TK_END,
    TK_WHILE,
    TK_UNION,
    TK_ENDUNION,
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
    TK_ERROR,
    TK_EOF
};

// Defining the structure for the twin buffer
struct TwinBuffer {
    char* active_buffer;
    char* load_buffer;
    int forward_ptr;
    int begin_ptr;
    bool buffer_loaded;
};

// Defining the structure of a token
struct Token {
    token_type name;
    char* lexeme;
    int line_num;
    bool is_value_int;

    union {
        long long num;
        long double r_num;
    } value;
};

#endif