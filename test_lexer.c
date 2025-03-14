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
    FILE* outputFile = fopen("result.dump", "w");
    if (!outputFile) {
        printf("Error: Could not create results.txt\n");
        fclose(inputFile);
        return 1;
    }

        // Read tokens one by one
    token* tok;
    for(int i = 0; i < 1000; i++) {
        tok = get_next_token(inputFile);
        fprintf(outputFile, "Line No. %d \tLexeme: %s \tToken: %s\n", 
                tok->line_num,  
                tok->lexeme,
                token_to_string(tok->name));

        free(tok);
        tok = NULL;
    }

    // Close files
    fclose(inputFile);
    fclose(outputFile);

    printf("Lexical analysis complete. Check results.txt for output.\n");
    return 0;
}
