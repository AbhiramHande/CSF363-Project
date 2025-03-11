#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "lexerDef.h"
#include "symbolTable.h"  // Includes getNextToken(), initializations(), and Token definition
#include "lexer.h"



// Function to map TokenName enum to a string for output
const char *getTokenName(TokenName token) {
    static const char *tokenNames[] = {
        "TK_ASSIGNOP", "TK_COMMENT", "TK_FIELDID", "TK_ID", "TK_NUM", "TK_RNUM",
        "TK_FUNID", "TK_RUID", "TK_WITH", "TK_PARAMETERS", "TK_END", "TK_WHILE",
        "TK_UNION", "TK_ENDUNION", "TK_DEFINETYPE", "TK_AS", "TK_TYPE", "TK_MAIN",
        "TK_GLOBAL", "TK_PARAMETER", "TK_LIST", "TK_SQL", "TK_SQR", "TK_INPUT",
        "TK_OUTPUT", "TK_INT", "TK_REAL", "TK_COMMA", "TK_SEM", "TK_COLON", "TK_DOT",
        "TK_ENDWHILE", "TK_OP", "TK_CL", "TK_IF", "TK_THEN", "TK_ENDIF", "TK_READ",
        "TK_WRITE", "TK_RETURN", "TK_PLUS", "TK_MINUS", "TK_MUL", "TK_DIV", "TK_CALL",
        "TK_RECORD", "TK_ENDRECORD", "TK_ELSE", "TK_AND", "TK_OR", "TK_NOT", "TK_LT",
        "TK_LE", "TK_EQ", "TK_GT", "TK_GE", "TK_NE", "EPSILON", "TK_ERROR", "DOLLAR", "SYN"
    };
    return tokenNames[token];
}

int main() {
    setbuf(stdout, NULL);
    // Open input file (test case)
    FILE *inputFile = fopen("t3.txt", "r");
    if (!inputFile) {
        printf("Error: Could not open t1.txt\n");
        return 1;
    }

    // Open output file to store tokenized results
    FILE* outputFile = stdout;
    if (!outputFile) {
        printf("Error: Could not create results.txt\n");
        fclose(inputFile);
        return 1;
    }

    // Initialize the lexer
    initializations();

    // Read tokens one by one
    Token *token;
    while ((token = getNextToken(inputFile)) != NULL) {
        fprintf(outputFile, "Token: %s, Lexeme: %s, Line: %d\n", 
                getTokenName(token->name),  
                token->lexeme,
                token->lineno);

        // free(token->lexeme);  // Free dynamically allocated lexeme
        // free(token->value);    // Free value storage
        // free(token);           // Free token itself
    }

    // Close files
    fclose(inputFile);
    fclose(outputFile);

    printf("Lexical analysis complete. Check results.txt for output.\n");
    return 0;
}
