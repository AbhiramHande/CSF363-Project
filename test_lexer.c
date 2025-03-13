#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "lexerDef.h"
//#include "symbolTable.h"  // Includes getNextToken(), initializations(), and Token definition
#include "lexer.h"

int main() {
    setbuf(stdout, NULL);
    // Open input file (test case)
    FILE *inputFile = fopen("tests/test_cases/test2.txt", "r");
    if (!inputFile) {
        printf("Error: Could not open t2.txt\n");
        return 1;
    }

    // Open output file to store tokenized results
    FILE* outputFile = fopen("result.txt", "w");
    if (!outputFile) {
        printf("Error: Could not create results.txt\n");
        fclose(inputFile);
        return 1;
    }

        // Read tokens one by one
    token* tok;
    for(int i = 0; i < 1000; i++) {
        tok = get_next_token(inputFile);
        fprintf(outputFile, "Token: %s, Lexeme: %s, Line: %d\n", 
                token_to_string(tok->name),  
                tok->lexeme,
                tok->line_num);

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
