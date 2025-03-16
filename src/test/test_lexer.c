#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "../../include/lexer.h"

int main(int argc, char** argv) {
    setbuf(stdout, NULL);
    if(argc < 2){
        printf("Atleast 2 inputs expected.\n");
        exit(EXIT_FAILURE);
    }
    FILE *inputFile = fopen(argv[1], "r");
    if (!inputFile) {
        printf("Error: Could not open %s\n", argv[1]);
        return 1;
    }

    FILE* outputFile = stdout;
    if (!outputFile) {
        printf("Error: Could not create results.txt\n");
        fclose(inputFile);
        return 1;
    }

    token* tok;
    while(tok = get_next_token(inputFile)) {
        if(tok->name != TK_EOF)
            fprintf(outputFile, "Line no. %d \tLexeme %s \tToken %s\n", 
                    tok->line_num,  
                    tok->lexeme,
                    token_to_string(tok->name));
        else    
            break;

        free(tok);
        tok = NULL;
    }
    free(tok);

    fclose(inputFile);
    return 0;
}
