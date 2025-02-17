/*

1. double retraction handle karna hai - done
2. values of variables and numbers - done
3. buffer retraction case - done
4. variable length case - Done
5. add to symbol table using addSymbol function - done

*/
// include statements
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#ifndef __lexerDef_H_INCLUDED__
#define __lexerDef_H_INCLUDED__
#include "lexerDef.h"
#endif

#ifndef __symbolTable_H_INCLUDED__
#define __symbolTable_H_INCLUDED__
#include "symbolTable.h"
#endif
#define BUFFER_SIZE 4096

twinBuffer *buffer;
symTable *symbolTable;
bool bufferChoice = 1;
int flagdr = 0;
int getstreamnull = 0;
int fwdptr = 0;
int varlength = 0;
int lineno = 1;
int funidlength = 0;
bool isnum(char c)
{
    if (c >= '0' && c <= '9')
    {
        return true;
    }
    return false;
}
bool isalphabet(char c)
{
    if (c >= 'a' && c <= 'z')
    {
        return true;
    }
    return false;
}
bool isIgnoreCaseAlphabet(char c)
{
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
    {
        return true;
    }
    return false;
}
bool is2to7(char c)
{
    if ((c >= '2' && c <= '7'))
    {
        return true;
    }
    return false;
}
char *currentBuffer(twinBuffer *buffer)
{
    if (bufferChoice == true)
    {
        return buffer->buffer1;
    }
    return buffer->buffer2;
}
void printBuffer(char *buffer)
{
    // printf("printing buffer : %s\n", buffer);
}
FILE *getStream(FILE *fp)
{
    if (flagdr == 1)
    {
        flagdr = 0;
        bufferChoice = !bufferChoice;
        fwdptr = 0;
        return fp;
    }
    if (bufferChoice == true)
    {
        memset(buffer->buffer2, '\0', BUFFER_SIZE + 1);
    }
    else
    {
        memset(buffer->buffer1, '\0', BUFFER_SIZE + 1);
    }
    // char ch;
    // if (fp != NULL)
    //{
    //     ch = fgetc(fp);
    // }
    // if (ch == -1)
    //{
    //     printf("reached eof\n");
    // }

    if (feof(fp))
    {
        // printf("reached eof\n");
        getstreamnull = 1;
        // bufferChoice = !bufferChoice;
        return NULL;
    }
    else
    {
        if (bufferChoice == true)
        {
            bufferChoice = !bufferChoice;
            int size;
            if ((size = fread(buffer->buffer2, sizeof(char), BUFFER_SIZE, fp)) > 0)
            {
                // printf("buffer 2 : %s\n", buffer->buffer1);

                if (size < BUFFER_SIZE)
                {
                    buffer->buffer1[size] = -1;
                }

                buffer->buffer2[BUFFER_SIZE] = '\0';

                return fp;
            }
            else
            {
                return NULL;
            }
        }
        else
        {
            bufferChoice = !bufferChoice;

            int size;
            if ((size = fread(buffer->buffer1, sizeof(char), BUFFER_SIZE, fp)) > 0)
            {
                // printf("buffer 1 : %s\n", buffer->buffer1);

                if (size < BUFFER_SIZE)
                {
                    buffer->buffer1[size] = -1;
                }
                buffer->buffer1[BUFFER_SIZE] = '\0';

                return fp;
            }
            else
            {
                return NULL;
            }
        }
    }
}

FILE *refillBuffer(int *fwdptr, FILE *fileptr)
{
    if (getstreamnull == 1)
    {
        return NULL;
    }
    // printf("printing current buffer : %c\n", currentBuffer(buffer)[*fwdptr]);
    if (currentBuffer(buffer)[*fwdptr] == '\0')
    {
        *fwdptr = 0;
        return getStream(fileptr);
    }

    else
    {
        return fileptr;
    }
}
void printBuffer1()
{
    printf("printing buffer1-------------\n");
    printf("%s\n", buffer->buffer1);
    printf("-------------\n\n");
}
void printBuffer2()
{
    printf("printing buffer2-------------\n");
    printf("%s\n", buffer->buffer2);
    printf("-------------\n \n");
}
Token *getNextToken(FILE *fileptr)
{

    Token *token = (Token *)malloc(sizeof(Token));
    if (getstreamnull == 1)
    {
        return NULL;
    }
    fileptr = refillBuffer(&fwdptr, fileptr);
    if (fileptr == NULL)
    {
        return NULL;
    }
    printBuffer(currentBuffer(buffer));
    int state = 0;
    char *lexeme = (char *)malloc(sizeof(char) * 100);
    memset(lexeme, '\0', 50);
    int lex_ptr = 0;
    varlength = 0;
    funidlength = 0;
    int errtype = 0;
    char prev;
    while (true)
    {
        switch (state)
        {
        case 0:
        {
            // ab mai kya karu
            // case 1 se likhna start kar
            fileptr = refillBuffer(&fwdptr, fileptr);
            printBuffer(currentBuffer(buffer));
            char ch = currentBuffer(buffer)[fwdptr];
            if (fileptr == NULL)
            {
                return NULL;
            }

            else if (currentBuffer(buffer)[fwdptr] == ' ' || currentBuffer(buffer)[fwdptr] == '\t')
            {
                state = 0;
                fwdptr++;
            }

            else if (currentBuffer(buffer)[fwdptr] == '\n')
            {
                state = 0;
                fwdptr++;
                lineno++;
            }

            else if (currentBuffer(buffer)[fwdptr] == ')')
            {
                state = 21;//1
                fwdptr++;
            }

            else if (currentBuffer(buffer)[fwdptr] == ',')
            {
                state = 16;//2
                fwdptr++;
            }

            else if (currentBuffer(buffer)[fwdptr] == ';')
            {
                state = 17;//3
                fwdptr++;
            }

            else if (currentBuffer(buffer)[fwdptr] == ']')
            {
                state = 15;//4
                fwdptr++;
            }

            else if (currentBuffer(buffer)[fwdptr] == ':')
            {
                state = 18;//5
                fwdptr++;
            }

            else if (currentBuffer(buffer)[fwdptr] == '(')
            {
                state = 20;//6
                fwdptr++;
            }

            else if (currentBuffer(buffer)[fwdptr] == '.')
            {
                state = 19;//7
                fwdptr++;
            }

            else if (currentBuffer(buffer)[fwdptr] == '[')
            {
                state = 14;//8
                fwdptr++;
            }

            else if (currentBuffer(buffer)[fwdptr] == '!')
            {
                state = 60;//9
                fwdptr++;
            }

            else if (currentBuffer(buffer)[fwdptr] == '+')
            {
                state = 22;//11
                fwdptr++;
            }

            else if (currentBuffer(buffer)[fwdptr] == '-')
            {
                state = 23;//12
                fwdptr++;
            }

            else if (currentBuffer(buffer)[fwdptr] == '*')
            {
                state = 24;//13
                fwdptr++;
            }

            else if (currentBuffer(buffer)[fwdptr] == '~')
            {
                state = 28;//14
                fwdptr++;
            }

            else if (currentBuffer(buffer)[fwdptr] == '/')
            {
                state = 25;//15
                fwdptr++;
            }

            else if (currentBuffer(buffer)[fwdptr] == '=')
            {
                state = 61;//16
                fwdptr++;
            }

            else if (currentBuffer(buffer)[fwdptr] == '>')
            {
                state = 54;//18
                fwdptr++;
            }

            else if (currentBuffer(buffer)[fwdptr] == '@')
            {
                state = 58;//21
                fwdptr++;
            }

            else if (currentBuffer(buffer)[fwdptr] == '&')
            {
                state = 56;//24
                prev = '%';
                fwdptr++;
            }

            else if (currentBuffer(buffer)[fwdptr] == '%')
            {
                state = 34;//27
                fwdptr++;
            }

            else if (currentBuffer(buffer)[fwdptr] == '<')
            {
                state = 51;//28
                fwdptr++;
            }

            else if (currentBuffer(buffer)[fwdptr] == '#')
            {
                state = 49;//35
                fwdptr++;
            }

            else if (currentBuffer(buffer)[fwdptr] - '0' <= 9 && currentBuffer(buffer)[fwdptr] - '0' >= 0)
            {
                prev = currentBuffer(buffer)[fwdptr];
                state = 35;//38
                fwdptr++;
            }

            else if (currentBuffer(buffer)[fwdptr] == '_')
            {
                state = 46;//49
                funidlength++;
                fwdptr++;
            }

            else if (currentBuffer(buffer)[fwdptr] == 'b' || currentBuffer(buffer)[fwdptr] == 'c' || currentBuffer(buffer)[fwdptr] == 'd')
            {
                varlength++;
                prev = currentBuffer(buffer)[fwdptr];
                state = 42;//53
                fwdptr++;
            }

            else if (currentBuffer(buffer)[fwdptr] == 'a' || currentBuffer(buffer)[fwdptr] - 'a' <= 25 && currentBuffer(buffer)[fwdptr] - 'a' >= 4)
            {
                prev = currentBuffer(buffer)[fwdptr];
                state = 45;//57
                fwdptr++;
            }
            else if (currentBuffer(buffer)[fwdptr] == -1 && (fileptr == NULL || feof(fileptr)))
            {
                return NULL;
            }

            else
            {
                prev = currentBuffer(buffer)[fwdptr];
                errtype = 3;
                state = 62;
                fwdptr++;
            }
            break;
        }
        case 21://1
        {

            lexeme[lex_ptr] = ')';
            lex_ptr++;
            token->lexeme = lexeme;
            token->name = TK_CL;
            token->lineno = lineno;
return token;
            break;
        }
        case 16://2
        {
            lexeme[lex_ptr] = ',';
            lex_ptr++;
            token->lexeme = lexeme;
            token->name = TK_COMMA;
            token->lineno = lineno;
return token;
            break;
        }
        case 17://3
        {
            lexeme[lex_ptr] = ';';
            lex_ptr++;
            token->lexeme = lexeme;
            token->name = TK_SEM;
            token->lineno = lineno;
return token;
            break;
        }
        case 15://4
        {
            lexeme[lex_ptr] = ']';
            lex_ptr++;
            token->lexeme = lexeme;
            token->name = TK_SQR;
            token->lineno = lineno;
return token;
            break;
        }
        case 18://5
        {
            lexeme[lex_ptr] = ':';
            lex_ptr++;
            token->lexeme = lexeme;
            token->name = TK_COLON;
            token->lineno = lineno;
return token;
            break;
        }
        case 20://6
        {
            lexeme[lex_ptr] = '(';
            lex_ptr++;
            token->lexeme = lexeme;
            token->name = TK_OP;
            token->lineno = lineno;
return token;
            break;
        }
        case 19://7
        {
            lexeme[lex_ptr] = '.';
            lex_ptr++;
            token->lexeme = lexeme;
            token->name = TK_DOT;
            token->lineno = lineno;
return token;
            break;
        }
        case 14://8
        {
            lexeme[lex_ptr] = '[';
            lex_ptr++;
            token->lexeme = lexeme;
            token->name = TK_SQL;
            token->lineno = lineno;
return token;
            break;
        }
        case 60://9
        {
            fileptr = refillBuffer(&fwdptr, fileptr);
            lexeme[lex_ptr] = '!';
            lex_ptr++;
            if (currentBuffer(buffer)[fwdptr] == '=')
            {
                state = 29;//10
                fwdptr++;
            }
            else
            {
                state = 62;
            }
            break;
        }
        case 29://10
        {
            lexeme[lex_ptr] = '=';
            lex_ptr++;
            token->lexeme = lexeme;
            token->name = TK_NE;
            token->lineno = lineno;
return token;
        }
        case 22://11
        {
            lexeme[lex_ptr] = '+';
            lex_ptr++;
            token->lexeme = lexeme;
            token->name = TK_PLUS;
            token->lineno = lineno;
return token;
        }
        case 23://12
        {
            lexeme[lex_ptr] = '-';
            lex_ptr++;
            token->lexeme = lexeme;
            token->name = TK_MINUS;
            token->lineno = lineno;
return token;
        }
        case 24://13
        {
            lexeme[lex_ptr] = '*';
            lex_ptr++;
            token->lexeme = lexeme;
            token->name = TK_MUL;
            token->lineno = lineno;
return token;
        }
        case 28://14
        {
            lexeme[lex_ptr] = '~';
            lex_ptr++;
            token->lexeme = lexeme;
            token->name = TK_NOT;
            token->lineno = lineno;
return token;
        }
        case 25://15
        {
            lexeme[lex_ptr] = '/';
            lex_ptr++;
            token->lexeme = lexeme;
            token->name = TK_DIV;
            token->lineno = lineno;
return token;
        }
        case 61://16
        {
            fileptr = refillBuffer(&fwdptr, fileptr);

            lexeme[lex_ptr] = '=';
            lex_ptr++;
            if (currentBuffer(buffer)[fwdptr] == '=')
            {
                state = 30;//17
                fwdptr++;
            }
            else
            {
                state = 62;
            }
            break;
        }
        case 30://17
        {
            lexeme[lex_ptr] = '=';
            lex_ptr++;
            token->lexeme = lexeme;
            token->name = TK_EQ;
            token->lineno = lineno;
return token;
        }
        case 54://18
        {
            fileptr = refillBuffer(&fwdptr, fileptr);
            lexeme[lex_ptr] = '>';
            lex_ptr++;
            if (currentBuffer(buffer)[fwdptr] == '=')
            {
                state = 12;//20
                fwdptr++;
            }
            else
            {
                state = 13;//19
            }
            break;
        }
        case 13://19
        {
            token->lexeme = lexeme;
            token->name = TK_GT;
            token->lineno = lineno;
return token;
        }
        case 12://20
        {
            lexeme[lex_ptr] = '=';
            lex_ptr++;
            token->lexeme = lexeme;
            token->name = TK_GE;
            token->lineno = lineno;
return token;
        }
        case 58://21
        {

            fileptr = refillBuffer(&fwdptr, fileptr);
            lexeme[lex_ptr] = '@';
            lex_ptr++;
            if (currentBuffer(buffer)[fwdptr] == '@')
            {
                state = 59;//22
                fwdptr++;
            }
            else
            {
                state = 62;
            }
            break;
        }
        case 59://22
        {
            fileptr = refillBuffer(&fwdptr, fileptr);
            lexeme[lex_ptr] = '@';
            lex_ptr++;
            if (currentBuffer(buffer)[fwdptr] == '@')
            {
                state = 27;//23
                fwdptr++;
            }
            else
            {
                state = 62;
            }
            break;
        }
        case 27://23
        {
            lexeme[lex_ptr] = '@';
            lex_ptr++;
            token->lexeme = lexeme;
            token->name = TK_OR;
            token->lineno = lineno;
return token;
        }
        case 56://24
        {

            fileptr = refillBuffer(&fwdptr, fileptr);
            lexeme[lex_ptr] = '&';
            lex_ptr++;
            if (currentBuffer(buffer)[fwdptr] == '&')
            {
                state = 57;//25
                fwdptr++;
            }
            else
            {
                state = 62;
            }
            break;
        }
        case 57://25
        {
            fileptr = refillBuffer(&fwdptr, fileptr);
            lexeme[lex_ptr] = '&';
            lex_ptr++;
            if (currentBuffer(buffer)[fwdptr] == '&')
            {
                state = 26;//26
                fwdptr++;
            }
            else
            {
                state = 62;
            }
            break;
        }
        case 26:
        {
            lexeme[lex_ptr] = '&';
            lex_ptr++;
            token->lexeme = lexeme;
            token->name = TK_AND;
            token->lineno = lineno;
            return token;
        }
        case 34://27
        {

            token->name = TK_COMMENT;

            do
            {
                fileptr = refillBuffer(&fwdptr, fileptr);
                char ch = currentBuffer(buffer)[fwdptr];
                //printBuffer(currentBuffer(buffer));
                if (ch == '\n')
                {
                    lineno++;
                    fwdptr++;
                    break;
                }
                else if (ch == -1)
                {
                    fwdptr++;
                    break;
                }
                else if (ch == '\0')
                {
                    fwdptr++;
                    break;
                }
                fwdptr++;

            } while (true);
            token->lineno = lineno;
return token;
            break;
        }

        case 51://28
        {
            lexeme[lex_ptr] = '<';
            lex_ptr++;

            fileptr = refillBuffer(&fwdptr, fileptr);

            if (fileptr == NULL)
            {
                state = 62;
            }
            else if (currentBuffer(buffer)[fwdptr] == '-')
            {
                state = 52;//30
                fwdptr++;
            }

            else if (currentBuffer(buffer)[fwdptr] == '=')
            {
                state = 8;//34
                fwdptr++;
            }

            else
            {
                state = 11;//29
            }
            break;
        }

        case 11://29
        {
            token->lexeme = lexeme;
            token->name = TK_LT;
            token->lineno = lineno;
return token;
        }

        case 52://30
        {
            lexeme[lex_ptr] = '-';
            lex_ptr++;

            fileptr = refillBuffer(&fwdptr, fileptr);

            if (fileptr == NULL)
            {
                state = 62;
            }

            else if (currentBuffer(buffer)[fwdptr] == '-')
            {
                state = 53;//32
                fwdptr++;
            }
            else
            {
                int l = strlen(lexeme);
                char *double_retracted_string = (char *)malloc(l - 1);
                strncpy(double_retracted_string, lexeme, l - 2);
                double_retracted_string[l - 2] = '\0'; // Null terminate the string
                token->lexeme = double_retracted_string;
                token->name = TK_LT;
                for (int i = 0; i < 2; i++)
                {
                    if (fwdptr > 0)
                    {
                        fwdptr--;
                    }
                    else
                    {
                        fwdptr = BUFFER_SIZE - 1;
                    }
                }
                flagdr = 1;
                token->lineno = lineno;
return token;
            }
            break;
        }

        case 31:
        {
            token->lexeme = lexeme;
            token->name = TK_LT;
            token->lineno = lineno;
return token;
        }

        case 53://32
        {
            lexeme[lex_ptr] = '-';
            lex_ptr++;

            fileptr = refillBuffer(&fwdptr, fileptr);

            if (fileptr == NULL)
            {
                state = 62;
            }

            else if (currentBuffer(buffer)[fwdptr] == '-')
            {
                state = 9;//33
                fwdptr++;
            }

            else
            {
                state = 62;
            }
            break;
        }

        case 9://33
        {
            lexeme[lex_ptr] = '-';
            token->lexeme = lexeme;
            token->name = TK_ASSIGNOP;
            token->lineno = lineno;
return token;
        }

        case 8://34
        {
            lexeme[lex_ptr] = '=';
            token->lexeme = lexeme;
            token->name = TK_LE;
            token->lineno = lineno;
return token;
        }
        case 49://35
        {

            fileptr = refillBuffer(&fwdptr, fileptr);

            lexeme[lex_ptr] = '#';
            lex_ptr++;
            char c = currentBuffer(buffer)[fwdptr];
            if (isalphabet(c))
            {
                prev = c;
                state = 50;//36
                fwdptr++;
            }
            else
            {
                state = 62;
            }
            break;
        }
        case 50://36
        {
            lexeme[lex_ptr] = prev;
            lex_ptr++;
            fileptr = refillBuffer(&fwdptr, fileptr);

            char c = currentBuffer(buffer)[fwdptr];
            if (isalphabet(c))
            {
                state = 50;//36
                prev = c;
                fwdptr++;
            }
            else
            {
                state = 7;//37
                prev = c;
            }
            break;
        }
        case 7://37
        {
            token->lexeme = lexeme;
            token->name = TK_RUID;
            nodeInfo *n = getInfo(symbolTable, lexeme);
            if (n->is_present)
            {
                token->name = n->node->tokentype;
                token->lineno = lineno;
return token;
            }
            else
            {
                addSymbol(symbolTable, token, lineno);
                token->lineno = lineno;
return token;
            }
            break;
        }
        case 35://38
        {
            lexeme[lex_ptr] = prev;
            lex_ptr++;
            fileptr = refillBuffer(&fwdptr, fileptr);
            char c = currentBuffer(buffer)[fwdptr];
            if (isnum(c))
            {
                prev = c;
                fwdptr++;
                state = 35;//38
            }
            else if (c == '.')
            {
                state = 36;//40
                fwdptr++;
            }
            else
            {
                state = 1;//39
            }
            break;
        }
        case 1://39
        {

            token->lexeme = lexeme;
            token->name = TK_NUM;
            token->value = (Value *)malloc(sizeof(Value));

            token->value->num = atoi(lexeme);
            token->isint = 1;
            token->lineno = lineno;
return token;
        }
        case 36://40
        {
            lexeme[lex_ptr] = '.';
            lex_ptr++;
            fileptr = refillBuffer(&fwdptr, fileptr);
            char c = currentBuffer(buffer)[fwdptr];
            if (isnum(c))
            {
                prev = c;
                state = 37;//42
                fwdptr++;
            }
            else
            {
                state = 63;//41
                prev = c;
                fwdptr++;
            }
            break;
        }
        case 63://41
        {
            lexeme[lex_ptr] = prev;
            int l = strlen(lexeme);
            char *double_retracted_string = (char *)malloc(l - 1);
            strncpy(double_retracted_string, lexeme, l - 2);
            double_retracted_string[l - 2] = '\0'; // Null terminate the string
            token->value = (Value *)malloc(sizeof(Value));
            token->value->num = atoi(lexeme);
            token->isint = 1;
            token->name = TK_NUM;
            for (int i = 0; i < 2; i++)
            {
                if (fwdptr > 0)
                {
                    fwdptr--;
                }
                else
                {
                    fwdptr = BUFFER_SIZE - 1;
                }
            }
            flagdr = 1;
            token->lineno = lineno;
return token;
        }
        case 37://42
        {
            lexeme[lex_ptr] = prev;
            lex_ptr++;
            fileptr = refillBuffer(&fwdptr, fileptr);
            char c = currentBuffer(buffer)[fwdptr];
            if (isnum(c))
            {
                prev = c;
                state = 38;//43
                fwdptr++;
            }
            else
            {
                state = 62;
            }
            break;
        }
        case 38://43
        {
            lexeme[lex_ptr] = prev;
            lex_ptr++;
            fileptr = refillBuffer(&fwdptr, fileptr);
            char c = currentBuffer(buffer)[fwdptr];
            if (c == 'E')
            {
                prev = c;
                state = 39;//45
                fwdptr++;
            }
            else
            {
                state = 2;//44
            }
            break;
        }
        case 2://44
        {

            token->lexeme = lexeme;
            token->name = TK_RNUM;
            token->isint = 0;
            token->value = (Value *)malloc(sizeof(Value));
            token->value->r_num = strtof(lexeme, NULL);
            token->isint = 0;
            token->lineno = lineno;
return token;
            // note
        }
        case 39://45
        {
            lexeme[lex_ptr] = 'E';
            lex_ptr++;
            fileptr = refillBuffer(&fwdptr, fileptr);
            char c = currentBuffer(buffer)[fwdptr];
            prev = c;

            if (c == '+' | c == '-')
            {
                state = 41;//46
                fwdptr++;
            }
            else if (isnum(c))
            {
                state = 40;//47
                fwdptr++;
            }
            else
            {
                state = 62;
            }
            break;
        }
        case 41://46
        {
            lexeme[lex_ptr] = prev;
            lex_ptr++;
            fileptr = refillBuffer(&fwdptr, fileptr);
            char c = currentBuffer(buffer)[fwdptr];
            prev = c;
            if (isnum(c))
            {
                state = 40;//47
                fwdptr++;
            }
            else
            {
                state = 62;
            }
            break;
        }
        case 40://47
        {
            lexeme[lex_ptr] = prev;
            lex_ptr++;
            fileptr = refillBuffer(&fwdptr, fileptr);
            char c = currentBuffer(buffer)[fwdptr];
            prev = c;
            if (isnum(c))
            {
                state = 3;//48
                fwdptr++;
            }
            else
            {
                state = 62;
            }
            break;
        }
        case 3://48
        {
            lexeme[lex_ptr] = prev;
            token->lexeme = lexeme;
            token->name = TK_RNUM;
            token->isint = 0;
            token->value = (Value *)malloc(sizeof(Value));

            token->value->r_num = strtof(lexeme, NULL);
            token->lineno = lineno;
return token;
        }
        case 46://49
        {
            lexeme[lex_ptr] = '_';
            lex_ptr++;
            fileptr = refillBuffer(&fwdptr, fileptr);
            char c = currentBuffer(buffer)[fwdptr];
            prev = c;
            // note
            if (isIgnoreCaseAlphabet(c))
            {
                state = 47;//50
                funidlength++;
                fwdptr++;
            }
            else
            {
                state = 62;
            }
            break;
        }
        case 47://50
        {
            lexeme[lex_ptr] = prev;
            lex_ptr++;
            fileptr = refillBuffer(&fwdptr, fileptr);
            char c = currentBuffer(buffer)[fwdptr];
            prev = c;
            if (isIgnoreCaseAlphabet(c))
            {
                state = 47;//50
                funidlength++;
                fwdptr++;
            }
            else if (isnum(c))
            {
                state = 48;//51
                funidlength++;
                fwdptr++;
            }
            else
            {
                state = 6;//52
            }
            break;
        }
        case 48://51
        {
            lexeme[lex_ptr] = prev;
            lex_ptr++;
            fileptr = refillBuffer(&fwdptr, fileptr);
            char c = currentBuffer(buffer)[fwdptr];
            prev = c;
            if (isnum(c))
            {
                state = 48;//51
                funidlength++;
                fwdptr++;
            }
            else
            {
                state = 6;//52
            }

            break;
        }
        case 6://52
        {

            // lexeme[lex_ptr] = prev;
            // lex_ptr++;
            token->lexeme = lexeme;
            token->name = TK_FUNID;
            if (!(strlen(lexeme) <= 30))
            {
                state = 62;
                errtype = 1;
                break;
            }
            else
            {
                nodeInfo *n = getInfo(symbolTable, lexeme);
                if (n->is_present)
                {
                    token->name = n->node->tokentype;
                    token->lineno = lineno;
return token;
                }
                else
                {
                    addSymbol(symbolTable, token, lineno);
                    token->lineno = lineno;
return token;
                }
            }
            break;
        }
        case 42://53
        {
            lexeme[lex_ptr] = prev;
            lex_ptr++;
            fileptr = refillBuffer(&fwdptr, fileptr);
            char c = currentBuffer(buffer)[fwdptr];
            prev = c;
            // note
            if (is2to7(c))
            {
                state = 43;//54
                varlength++;
                fwdptr++;
            }
            else if (isalphabet(c))
            {
                state = 45;//57
                varlength++;
                fwdptr++;
            }
            else
            {
                state = 62;
            }
            break;
        }
        case 43://54
        {
            lexeme[lex_ptr] = prev;
            lex_ptr++;
            fileptr = refillBuffer(&fwdptr, fileptr);
            char c = currentBuffer(buffer)[fwdptr];
            prev = c;
            if (c == 'b' || c == 'c' || c == 'd')
            {
                state = 43;//54
                varlength++;
                fwdptr++;
            }
            else if (is2to7(c))
            {
                state = 44;//55
                varlength++;
                fwdptr++;
            }
            else
            {
                state = 4;//56
            }
            break;
        }
        case 44://55
        {
            lexeme[lex_ptr] = prev;
            lex_ptr++;
            fileptr = refillBuffer(&fwdptr, fileptr);
            char c = currentBuffer(buffer)[fwdptr];
            prev = c;
            if (is2to7(c))
            {
                state = 44;//55
                fwdptr++;
            }
            else
            {
                state = 4;//56
            }
            break;
        }
        case 4://56
        {
            token->lexeme = lexeme;
            token->name = TK_ID;
            if (varlength >= 2 && varlength <= 20)
            {
                nodeInfo *n = getInfo(symbolTable, lexeme);
                if (n->is_present)
                {
                    token->name = n->node->tokentype;
                    token->lineno = lineno;
                    return token;
                }
                else
                {
                    addSymbol(symbolTable, token, lineno);
                    token->lineno = lineno;
                    return token;
                }
                token->lineno = lineno;
                return token;
            }
            else
            {
                state = 62;
                errtype = 1;
            }
            break;
            // note
        }
        case 45://57
        {

            fileptr = refillBuffer(&fwdptr, fileptr);
            lexeme[lex_ptr] = prev;
            lex_ptr++;
            char c = currentBuffer(buffer)[fwdptr];
            prev = c;
            if (isalphabet(c) && getstreamnull != 1)
            {
                state = 45;//57
                fwdptr++;
            }
            else
            {
                state = 5;//58
                // fwdptr++;
            }
            break;
        }
        case 5://58
        {
            token->lexeme = lexeme;
            token->name = TK_FIELDID;
            nodeInfo *n = getInfo(symbolTable, lexeme);
            if (n->is_present)
            {
                token->name = n->node->tokentype;
                token->lineno = lineno;
return token;
            }
            else
            {
                addSymbol(symbolTable, token, lineno);
                token->lineno = lineno;
return token;
            }
            break;
        }
        // case 59:
        // {
        //     lineno++;
        //     state = 0;
        //     fwdptr++;
        //     break;
        // }
        case 62:
        {
            printf("Error at lineno : %d\n", lineno);
            if (errtype == 0)
            {
                Token *t;
                t = (Token *)malloc(sizeof(Token));
                t->name = TK_ERROR;
                printf("\nThis is not a valid token : %s \n", lexeme);

                return t;
            }
            else if (errtype == 3)
            {

                Token *t;
                t = (Token *)malloc(sizeof(Token));
                t->name = TK_ERROR;
                printf("\nThis is not a valid token : %c \n", prev);
                return t;
            }
            else
            {
                Token *t;
                t = (Token *)malloc(sizeof(Token));
                t->name = TK_ERROR;
                printf("Variable length should be between 2 and 20.\n Length of variable '%s' is %d \n", lexeme, varlength);

                return t;
            }
            // note after error tokensiation
        }
        }
    }
}
void printToken(Token *t)
{
    const char *enumValues[] = {"TK_ASSIGNOP", "TK_COMMENT", "TK_FIELDID", "TK_ID", "TK_NUM", "TK_RNUM", "TK_FUNID", "TK_RUID", "TK_WITH", "TK_PARAMETERS", "TK_END", "TK_WHILE", "TK_UNION", "TK_ENDUNION", "TK_DEFINETYPE", "TK_AS", "TK_TYPE", "TK_MAIN", "TK_GLOBAL", "TK_PARAMETER", "TK_LIST", "TK_SQL", "TK_SQR", "TK_INPUT", "TK_OUTPUT", "TK_INT", "TK_REAL", "TK_COMMA", "TK_SEM", "TK_COLON", "TK_DOT", "TK_ENDWHILE", "TK_OP", "TK_CL", "TK_IF", "TK_THEN", "TK_ENDIF", "TK_READ", "TK_WRITE", "TK_RETURN", "TK_PLUS", "TK_MINUS", "TK_MUL", "TK_DIV", "TK_CALL", "TK_RECORD", "TK_ENDRECORD", "TK_ELSE", "TK_AND", "TK_OR", "TK_NOT", "TK_LT", "TK_LE", "TK_EQ", "TK_GT", "TK_GE", "TK_NE", "EPSILON", "TK_ERROR"};

    printf("------------------- \n");
    if (t->name == TK_NUM)
    {
        printf("Value : %d\n", t->value->num);
    }
    else if (t->name == TK_RNUM)
    {
        printf("Value : %f\n", t->value->r_num);
    }

    printf("Token Type : %s\n", enumValues[t->name]);
    printf("Lexeme : %s\n", t->lexeme);
    printf("Line number : %d\n", lineno);
    printf("------------------- \n");
}

void initializations()
{
    // Allocate memory for buffers
    bufferChoice = 1;
    flagdr = 0;
    getstreamnull = 0;
    fwdptr = 0;
    varlength = 0;
    lineno = 1;
    funidlength = 0;

    symbolTable = createEmptyTable(50);
    addKeywords(symbolTable);

    if(buffer!=NULL){
        if(buffer->buffer1!=NULL){
            free(buffer->buffer1);
        }
        if(buffer->buffer2!=NULL){
            free(buffer->buffer2);
        }
        free(buffer);
    }

    buffer = (twinBuffer *)malloc(sizeof(twinBuffer));
    buffer->buffer1 = (char *)malloc((BUFFER_SIZE + 1) * sizeof(char)); // true
    if (buffer->buffer1 == NULL)
    {
        fprintf(stderr, "Memory allocation failed for buffer1\n");
        return;
    }
    buffer->buffer2 = (char *)malloc((BUFFER_SIZE + 1) * sizeof(char)); // false
    if (buffer->buffer2 == NULL)
    {
        fprintf(stderr, "Memory allocation failed for buffer1\n");
        return;
        // Exit with failure status
    }
    memset(buffer->buffer1, '\0', BUFFER_SIZE + 1);
    memset(buffer->buffer1, '\0', BUFFER_SIZE + 1);

    getstreamnull = 0;
    lineno = 1;

}