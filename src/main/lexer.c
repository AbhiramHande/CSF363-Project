#include <assert.h>

#include "../../include/lexer.h"
#include "../../include/symbol_table.h"

#define STATE_START             0
#define STATE_FIN_NUM           1
#define STATE_FIN_RNUM_NO_EXP   2
#define STATE_FIN_RNUM_EXP      3
#define STATE_FIN_ID            4
#define STATE_FIN_KEY_OR_FIELD  5
#define STATE_FIN_FUNCTION      6
#define STATE_FIN_RUID          7
#define STATE_FIN_LE            8
#define STATE_FIN_ASSIGNOP      9
#define STATE_FIN_LT_MIUNS      10
#define STATE_FIN_LT            11
#define STATE_FIN_GE            12
#define STATE_FIN_GT            13
#define STATE_FIN_SQR_OPEN      TK_SQL
#define STATE_FIN_SQR_CLOSE     TK_SQR
#define STATE_FIN_COMMA         TK_COMMA
#define STATE_FIN_SEMI_COLON    TK_SEM
#define STATE_FIN_COLON         TK_COLON
#define STATE_FIN_DOT           TK_DOT
#define STATE_FIN_PAREN_OPEN    TK_OP
#define STATE_FIN_PAREN_CLOSE   TK_CL
#define STATE_FIN_PLUS          TK_PLUS
#define STATE_FIN_MINUS         TK_MINUS
#define STATE_FIN_MUL           TK_MUL
#define STATE_FIN_DIV           TK_DIV
#define STATE_FIN_AND           26
#define STATE_FIN_OR            27
#define STATE_FIN_NOT           28
#define STATE_FIN_NOT_EQUAL     29
#define STATE_FIN_EQUAL         30
#define STATE_FIN_WHITESPACE    31
#define STATE_FIN_NEWLINE       32
#define STATE_FIN_COMMENT       33                 

#define STATE_INTR_COMMENT                  34
#define STATE_INTR_NUM                      35
#define STATE_INTR_RNUM_NO_EXP_NO_DIGIT     36
#define STATE_INTR_RNUM_NO_EXP_ONE_DIGIT    37
#define STATE_INTR_RNUM_NO_EXP_TWO_DIGIT    38
#define STATE_INTR_RNUM_EXP_NO_DIGIT        39
#define STATE_INTR_RNUM_EXP_ONE_DIGIT       40
#define STATE_INTR_RNUM_EXP_NO_DIGIT_SIGNED 41
#define STATE_INTR_ID_ONE_CHAR              42
#define STATE_INTR_ID_MORE_CHARS            43
#define STATE_INTR_ID_MORE_DIGITS           44
#define STATE_INTR_KEY_OR_FIELD             45
#define STATE_INTR_FUNCTION_                46
#define STATE_INTR_FUNCTION_CHARS           47
#define STATE_INTR_FUNCTION_DIGITS          48
#define STATE_INTR_RUID_HASH                49
#define STATE_INTR_RUID_CHARS               50
#define STATE_INTR_ANGULAR_OPEN             51
#define STATE_INTR_ANGULAR_OPEN_MINUS       52
#define STATE_INTR_ALMOST_ASSIGN            53
#define STATE_INTR_ANGULAR_CLOSE            54
#define STATE_INTR_WHITESPACE               55
#define STATE_INTR_AND_ONE                  56
#define STATE_INTR_AND_TWO                  57
#define STATE_INTR_AT_ONE                   58
#define STATE_INTR_AT_TWO                   59
#define STATE_INTR_BANG                     60
#define STATE_INTR_EQUAL                    61

#define STATE_TRAP_SYMBOL_START             62      
#define STATE_TRAP_PATTERN                  63
#define STATE_TRAP_SYMBOL_INTR              65

static twin_buffer* buffer = NULL;
token* get_next_token_helper(FILE* file_ptr);
void buffer_cleanup(void) __attribute__((destructor));
char* retract_and_update(int no_of_times);

token_type string_to_token(const char* string) {
    if (strcmp(string, "TK_ASSIGNOP") == 0) return TK_ASSIGNOP;
    if (strcmp(string, "TK_COMMENT") == 0) return TK_COMMENT;
    if (strcmp(string, "TK_FIELDID") == 0) return TK_FIELDID;
    if (strcmp(string, "TK_ID") == 0) return TK_ID;
    if (strcmp(string, "TK_NUM") == 0) return TK_NUM;
    if (strcmp(string, "TK_RNUM") == 0) return TK_RNUM;
    if (strcmp(string, "TK_FUNID") == 0) return TK_FUNID;
    if (strcmp(string, "TK_RUID") == 0) return TK_RUID;
    if (strcmp(string, "TK_WITH") == 0) return TK_WITH;
    if (strcmp(string, "TK_PARAMETERS") == 0) return TK_PARAMETERS;
    if (strcmp(string, "TK_END") == 0) return TK_END;
    if (strcmp(string, "TK_WHILE") == 0) return TK_WHILE;
    if (strcmp(string, "TK_UNION") == 0) return TK_UNION;
    if (strcmp(string, "TK_ENDUNION") == 0) return TK_ENDUNION;
    if (strcmp(string, "TK_DEFINETYPE") == 0) return TK_DEFINETYPE;
    if (strcmp(string, "TK_AS") == 0) return TK_AS;
    if (strcmp(string, "TK_TYPE") == 0) return TK_TYPE;
    if (strcmp(string, "TK_MAIN") == 0) return TK_MAIN;
    if (strcmp(string, "TK_GLOBAL") == 0) return TK_GLOBAL;
    if (strcmp(string, "TK_PARAMETER") == 0) return TK_PARAMETER;
    if (strcmp(string, "TK_LIST") == 0) return TK_LIST;
    if (strcmp(string, "TK_SQL") == 0) return TK_SQL;
    if (strcmp(string, "TK_SQR") == 0) return TK_SQR;
    if (strcmp(string, "TK_INPUT") == 0) return TK_INPUT;
    if (strcmp(string, "TK_OUTPUT") == 0) return TK_OUTPUT;
    if (strcmp(string, "TK_INT") == 0) return TK_INT;
    if (strcmp(string, "TK_REAL") == 0) return TK_REAL;
    if (strcmp(string, "TK_COMMA") == 0) return TK_COMMA;
    if (strcmp(string, "TK_SEM") == 0) return TK_SEM;
    if (strcmp(string, "TK_COLON") == 0) return TK_COLON;
    if (strcmp(string, "TK_DOT") == 0) return TK_DOT;
    if (strcmp(string, "TK_ENDWHILE") == 0) return TK_ENDWHILE;
    if (strcmp(string, "TK_OP") == 0) return TK_OP;
    if (strcmp(string, "TK_CL") == 0) return TK_CL;
    if (strcmp(string, "TK_IF") == 0) return TK_IF;
    if (strcmp(string, "TK_THEN") == 0) return TK_THEN;
    if (strcmp(string, "TK_ENDIF") == 0) return TK_ENDIF;
    if (strcmp(string, "TK_READ") == 0) return TK_READ;
    if (strcmp(string, "TK_WRITE") == 0) return TK_WRITE;
    if (strcmp(string, "TK_RETURN") == 0) return TK_RETURN;
    if (strcmp(string, "TK_PLUS") == 0) return TK_PLUS;
    if (strcmp(string, "TK_MINUS") == 0) return TK_MINUS;
    if (strcmp(string, "TK_MUL") == 0) return TK_MUL;
    if (strcmp(string, "TK_DIV") == 0) return TK_DIV;
    if (strcmp(string, "TK_CALL") == 0) return TK_CALL;
    if (strcmp(string, "TK_RECORD") == 0) return TK_RECORD;
    if (strcmp(string, "TK_ENDRECORD") == 0) return TK_ENDRECORD;
    if (strcmp(string, "TK_ELSE") == 0) return TK_ELSE;
    if (strcmp(string, "TK_AND") == 0) return TK_AND;
    if (strcmp(string, "TK_OR") == 0) return TK_OR;
    if (strcmp(string, "TK_NOT") == 0) return TK_NOT;
    if (strcmp(string, "TK_LT") == 0) return TK_LT;
    if (strcmp(string, "TK_LE") == 0) return TK_LE;
    if (strcmp(string, "TK_EQ") == 0) return TK_EQ;
    if (strcmp(string, "TK_GT") == 0) return TK_GT;
    if (strcmp(string, "TK_GE") == 0) return TK_GE;
    if (strcmp(string, "TK_NE") == 0) return TK_NE;
    if (strcmp(string, "EPS") == 0) return EPSILON;
    if (strcmp(string, "TK_ERROR") == 0) return TK_ERROR;
    if (strcmp(string, "$") == 0) return DOLLAR;

    return TK_ERROR;
}

const char* token_to_string(token_type token) {
    switch (token) {
        case TK_ASSIGNOP: return "TK_ASSIGNOP";
        case TK_COMMENT: return "TK_COMMENT";
        case TK_FIELDID: return "TK_FIELDID";
        case TK_ID: return "TK_ID";
        case TK_NUM: return "TK_NUM";
        case TK_RNUM: return "TK_RNUM";
        case TK_FUNID: return "TK_FUNID";
        case TK_RUID: return "TK_RUID";
        case TK_WITH: return "TK_WITH";
        case TK_PARAMETERS: return "TK_PARAMETERS";
        case TK_END: return "TK_END";
        case TK_WHILE: return "TK_WHILE";
        case TK_UNION: return "TK_UNION";
        case TK_ENDUNION: return "TK_ENDUNION";
        case TK_DEFINETYPE: return "TK_DEFINETYPE";
        case TK_AS: return "TK_AS";
        case TK_TYPE: return "TK_TYPE";
        case TK_MAIN: return "TK_MAIN";
        case TK_GLOBAL: return "TK_GLOBAL";
        case TK_PARAMETER: return "TK_PARAMETER";
        case TK_LIST: return "TK_LIST";
        case TK_SQL: return "TK_SQL";
        case TK_SQR: return "TK_SQR";
        case TK_INPUT: return "TK_INPUT";
        case TK_OUTPUT: return "TK_OUTPUT";
        case TK_INT: return "TK_INT";
        case TK_REAL: return "TK_REAL";
        case TK_COMMA: return "TK_COMMA";
        case TK_SEM: return "TK_SEM";
        case TK_COLON: return "TK_COLON";
        case TK_DOT: return "TK_DOT";
        case TK_ENDWHILE: return "TK_ENDWHILE";
        case TK_OP: return "TK_OP";
        case TK_CL: return "TK_CL";
        case TK_IF: return "TK_IF";
        case TK_THEN: return "TK_THEN";
        case TK_ENDIF: return "TK_ENDIF";
        case TK_READ: return "TK_READ";
        case TK_WRITE: return "TK_WRITE";
        case TK_RETURN: return "TK_RETURN";
        case TK_PLUS: return "TK_PLUS";
        case TK_MINUS: return "TK_MINUS";
        case TK_MUL: return "TK_MUL";
        case TK_DIV: return "TK_DIV";
        case TK_CALL: return "TK_CALL";
        case TK_RECORD: return "TK_RECORD";
        case TK_ENDRECORD: return "TK_ENDRECORD";
        case TK_ELSE: return "TK_ELSE";
        case TK_AND: return "TK_AND";
        case TK_OR: return "TK_OR";
        case TK_NOT: return "TK_NOT";
        case TK_LT: return "TK_LT";
        case TK_LE: return "TK_LE";
        case TK_EQ: return "TK_EQ";
        case TK_GT: return "TK_GT";
        case TK_GE: return "TK_GE";
        case TK_NE: return "TK_NE";
        case EPSILON: return "EPS";
        case TK_ERROR: return "TK_ERROR";
        case DOLLAR: return "$";
        case TK_EOF: return "EOF";
        default: return "TK_ERROR";
    }
}

void buffer_cleanup(void){
    if(buffer == NULL)
        return;
    
    retract_and_update(-1);
    free(buffer->active_buffer);
    free(buffer->load_buffer);

    buffer->active_buffer = NULL;
    buffer->load_buffer = NULL;
    buffer->begin_ptr = 0;
    buffer->forward_ptr = 0;
    buffer->buffer_loaded = false;

    free(buffer);
    buffer = NULL;
    return;
}

void swap_buffer(FILE* file_ptr){
    assert(buffer->forward_ptr == BUFFER_SIZE - 1);
    if(!buffer->buffer_loaded){
        size_t char_read = fread(buffer->load_buffer, sizeof(char), BUFFER_SIZE - 1, file_ptr);
        if(char_read > 0)
            buffer->load_buffer[char_read] = '\0';
    }
    
    char* temp = buffer->load_buffer;
    buffer->load_buffer = buffer->active_buffer;
    buffer->active_buffer = temp;

    if(buffer->forward_ptr == buffer->begin_ptr)
        buffer->begin_ptr = 0;
    else 
        buffer->begin_ptr -= BUFFER_SIZE;
    
    buffer->forward_ptr = 0;
    buffer->buffer_loaded = false;
    return;
}

char* retract_and_update( int no_of_times) {
    static char* ret_tok = NULL;
    static int ret_tok_size = 0;
    int new_pos = buffer->forward_ptr - no_of_times;

    if(no_of_times < 0){
        free(ret_tok);
        ret_tok = NULL;
        ret_tok_size = 0;
        return NULL;
    }

    if(new_pos >= 0){
        buffer->forward_ptr = new_pos;
        if(buffer->begin_ptr < 0){
            int begin_len = -buffer->begin_ptr;
            int str_len = buffer->forward_ptr + begin_len - 1;
            if(ret_tok_size < str_len + 1){
                ret_tok = realloc(ret_tok, (str_len + 1)*sizeof(char));
                ret_tok_size = str_len + 1;
            }
            strncpy(ret_tok, buffer->load_buffer + BUFFER_SIZE + buffer->begin_ptr, -(buffer->begin_ptr + 1));
            strncpy(ret_tok - (buffer->begin_ptr + 1), buffer->active_buffer, buffer->forward_ptr);
            ret_tok[str_len] = '\0';
        }
        else{
            int str_len = buffer->forward_ptr - buffer->begin_ptr;
            if(ret_tok_size < str_len + 1){
                ret_tok = realloc(ret_tok, (str_len + 1)*sizeof(char));
                ret_tok_size = str_len + 1;
            }
            strncpy(ret_tok, buffer->active_buffer + buffer->begin_ptr, str_len);
            ret_tok[str_len] = '\0';
        }

        buffer->begin_ptr = new_pos;
        return ret_tok;
    }

    char* temp = buffer->load_buffer;
    buffer->load_buffer = buffer->active_buffer;
    buffer->active_buffer = temp;

    buffer->buffer_loaded = true;
    buffer->forward_ptr = BUFFER_SIZE + new_pos - 1; // May lead to an error MUST CHECK
    buffer->begin_ptr += BUFFER_SIZE;

    int str_len = buffer->forward_ptr - buffer->begin_ptr;
    if(ret_tok_size < str_len + 1){
        ret_tok = realloc(ret_tok, (str_len + 1)*sizeof(char));
        ret_tok_size = str_len + 1;
    }
    strncpy(ret_tok, buffer->active_buffer + buffer->begin_ptr, str_len);
    ret_tok[str_len] = '\0';

    buffer->begin_ptr = buffer->forward_ptr;
    return ret_tok;
}

void buffer_init(FILE* file_ptr){
    buffer_cleanup();
    get_next_token_helper(NULL);
    buffer = (twin_buffer*)calloc(1, sizeof(twin_buffer));
    if(!buffer){
        fprintf(stderr, "\033[1;31mError:\033[0m Failed to initialize Twin buffer");
        exit(EXIT_FAILURE);
    }
    
    buffer->active_buffer = (char*)calloc(BUFFER_SIZE, sizeof(char));
    buffer->load_buffer = (char*)calloc(BUFFER_SIZE, sizeof(char));
    if(!buffer->active_buffer || !buffer->load_buffer){
        fprintf(stderr, "\033[1;31mError:\033[0m Failed to initialize Twin buffer");

        free(buffer->active_buffer);
        free(buffer->load_buffer);
        free(buffer);

        exit(EXIT_FAILURE);
    }

    size_t char_read = fread(buffer->active_buffer, sizeof(char), BUFFER_SIZE - 1, file_ptr);
    if(char_read > 0)
        buffer->active_buffer[char_read] = '\0';

    buffer->begin_ptr = 0;
    buffer->forward_ptr = 0;
    buffer->buffer_loaded = false; 
}

token* create_token(char* lexeme, token_type type, int line){
    token* ret_tok = calloc(1, sizeof(token));
    entry* sym_tab = symbol_table_insert(lexeme, type);

    ret_tok->name = sym_tab->type;
    ret_tok->lexeme = sym_tab->name;
    ret_tok->line_num = line;

    return ret_tok;
}

token* get_next_token(FILE* file_ptr){
    if(file_ptr == NULL)
        fprintf(stderr, "\033[1;31mError:\033[0m Invalid file passed.\n");
    token* ret;
    while(!(ret = get_next_token_helper(file_ptr)));
    return ret;
}

token* get_next_token_helper(FILE* file_ptr){
    static int state = STATE_START;
    static int line_number = 1;
    static entry* comment = NULL;

    if(file_ptr == NULL){
        state = STATE_START;
        line_number = 1;
        return NULL;
    }
    if(buffer == NULL)
        buffer_init(file_ptr);
    if(comment == NULL)
        comment = symbol_table_insert("%", TK_COMMENT);

    char buffer_char = buffer->active_buffer[buffer->forward_ptr];
    char* lexeme = NULL;
    token* ret_tok = NULL;
    entry* sym_tab = NULL; 
    static int error_retract = 1;      //TODO: might be depracated --must remove

    if(state == 0 && buffer_char == '\0' && buffer->forward_ptr != BUFFER_SIZE - 1){
        ret_tok = calloc(1, sizeof(struct Token));
        ret_tok->name = TK_EOF;
        return ret_tok;
    }
    
    switch(state) {
        case STATE_START:
            if(isdigit(buffer_char))
                state = STATE_INTR_NUM;
            else if(buffer_char == 'b' || buffer_char == 'c' || buffer_char == 'd')
                state = STATE_INTR_ID_ONE_CHAR;
            else if(islower(buffer_char))
                state = STATE_INTR_KEY_OR_FIELD;
            else{
                switch (buffer_char) {
                    case '%':
                        state = STATE_INTR_COMMENT;
                        break;

                    case '_':
                        state = STATE_INTR_FUNCTION_;
                        break;
                    
                    case '#':
                        state = STATE_INTR_RUID_HASH;
                        break;
                    
                    case '<':
                        state = STATE_INTR_ANGULAR_OPEN;
                        break;

                    case '>':
                        state = STATE_INTR_ANGULAR_CLOSE;
                        break;

                    case '[':
                        state = STATE_FIN_SQR_OPEN;
                        break;

                    case ']':
                        state = STATE_FIN_SQR_CLOSE;
                        break;

                    case '\t':
                    case ' ':
                        state = STATE_INTR_WHITESPACE;
                        break;

                    case '\n':
                        state = STATE_FIN_NEWLINE;
                        break;

                    case ',':
                        state = STATE_FIN_COMMA;
                        break;

                    case ';':
                        state = STATE_FIN_SEMI_COLON;
                        break;

                    case ':':
                        state = STATE_FIN_COLON;
                        break;

                    case '.':
                        state = STATE_FIN_DOT;
                        break;

                    case '(':
                        state = STATE_FIN_PAREN_OPEN;
                        break;

                    case ')':
                        state = STATE_FIN_PAREN_CLOSE;
                        break;

                    case '+':
                        state = STATE_FIN_PLUS;
                        break;

                    case '-':
                        state = STATE_FIN_MINUS;
                        break;

                    case '*':
                        state = STATE_FIN_MUL;
                        break;

                    case '/':
                        state = STATE_FIN_DIV;
                        break;

                    case '&':
                        state = STATE_INTR_AND_ONE;
                        break;

                    case '@':
                        state = STATE_INTR_AT_ONE;
                        break;
                    
                    case '~':
                        state = STATE_FIN_NOT;
                        break;
                    
                    case '!':
                        state = STATE_INTR_BANG;
                        break;

                    case '=':
                        state = STATE_INTR_EQUAL;
                        break;                        

                    default:
                        state = STATE_TRAP_SYMBOL_START;
                        break;
                }
            }
            break;
        
        case STATE_FIN_NUM:
            char* lexeme = retract_and_update(1);
            ret_tok = create_token(lexeme, TK_NUM, line_number);
            ret_tok->value.num = strtoll(ret_tok->lexeme, NULL, 10);
            ret_tok->is_value_int = true;
            
            state = STATE_START;
            return ret_tok;
            break;
        
        case STATE_FIN_RNUM_NO_EXP: case STATE_FIN_RNUM_EXP:
            lexeme = (state == 2 ? retract_and_update(1) : retract_and_update(0));
            ret_tok = create_token(lexeme, TK_RNUM, line_number);
            ret_tok->value.r_num = strtold(ret_tok->lexeme, NULL);
            ret_tok->is_value_int = false;
            
            state = STATE_START;
            return ret_tok;
            break;

        case STATE_FIN_ID:
            lexeme = retract_and_update(1);
            if(strlen(lexeme) > 20){
                fprintf(stderr, "Line no. %-4d: \033[1;31mError:\033[0m Variable identifier is longer than the prescribed length of \033[1m20\033[0m characters.\n", line_number);
                state = STATE_START;
                return NULL;
            }
            ret_tok = create_token(lexeme, TK_ID, line_number);
            
            state = STATE_START;
            return ret_tok;
            break;

        case STATE_FIN_KEY_OR_FIELD: 
            lexeme = retract_and_update(1);
            ret_tok = create_token(lexeme, TK_FIELDID, line_number);

            state = STATE_START;
            return ret_tok;
            break;

        case STATE_FIN_FUNCTION:
            lexeme = retract_and_update(1);
            if(strlen(lexeme) > 30){
                fprintf(stderr, "Line no. %-4d: \033[1;31mError:\033[0m Function identifier is longer than the prescribed length of \033[1m30\033[0m characters.\n", line_number);
                state = STATE_START;
                return NULL;
            }
            ret_tok = create_token(lexeme, TK_FUNID, line_number);
            
            state = STATE_START;
            return ret_tok;
            break;
        
        case STATE_FIN_RUID:
            lexeme = retract_and_update(1);
            ret_tok = create_token(lexeme, TK_RUID, line_number);

            state = STATE_START;
            return ret_tok;
            break;
        
        case STATE_FIN_LE:
            lexeme = retract_and_update(0);
            ret_tok = create_token(lexeme, TK_LE, line_number);
            
            state = STATE_START;
            return ret_tok;
            break;

        case STATE_FIN_ASSIGNOP:
            lexeme = retract_and_update(0);
            ret_tok = create_token(lexeme, TK_ASSIGNOP, line_number);
            
            state = STATE_START;
            return ret_tok;
            break;

        case STATE_FIN_LT_MIUNS:
            lexeme = retract_and_update(2);
            ret_tok = create_token(lexeme, TK_LT, line_number);
            
            state = STATE_START;
            return ret_tok;
            break;
        
        case STATE_FIN_LT:
            lexeme = retract_and_update(1);
            ret_tok = create_token(lexeme, TK_LT, line_number);
            
            state = STATE_START;
            return ret_tok;
            break;

        case STATE_FIN_GE:
            lexeme = retract_and_update(0);
            ret_tok = create_token(lexeme, TK_GE, line_number);
            
            state = STATE_START;
            return ret_tok;
            break;

        case STATE_FIN_GT:
            lexeme = retract_and_update(1);
            ret_tok = create_token(lexeme, TK_GT, line_number);
            
            state = STATE_START;
            return ret_tok;
            break;

        case STATE_FIN_SQR_OPEN:
        case STATE_FIN_SQR_CLOSE:
        case STATE_FIN_COMMA:
        case STATE_FIN_SEMI_COLON:
        case STATE_FIN_COLON:
        case STATE_FIN_DOT:
        case STATE_FIN_PAREN_OPEN:
        case STATE_FIN_PAREN_CLOSE:
        case STATE_FIN_PLUS:
        case STATE_FIN_MINUS:
        case STATE_FIN_MUL:
        case STATE_FIN_DIV:
        case STATE_FIN_AND:
        case STATE_FIN_OR:
        case STATE_FIN_NOT:
        case STATE_FIN_NOT_EQUAL:
        case STATE_FIN_EQUAL:
            lexeme = retract_and_update(0);
            ret_tok = create_token(lexeme, (token_type)state, line_number);
            
            state = STATE_START;
            return ret_tok;
            break;
        
        case STATE_FIN_WHITESPACE:
            if(buffer->forward_ptr == 0){
                buffer->forward_ptr = BUFFER_SIZE - 1;
                swap_buffer(file_ptr);
            }
            else
                buffer->forward_ptr--;
            buffer->begin_ptr = buffer->forward_ptr;
            if(buffer->buffer_loaded)
                buffer->buffer_loaded = false;

            state = STATE_START;
            return ret_tok;
            break;
        
        case STATE_FIN_NEWLINE:
            buffer->begin_ptr = buffer->forward_ptr;
            if(buffer->buffer_loaded)
                buffer->buffer_loaded = false;
            line_number++;

            state = STATE_START;
            return NULL;
            break;

        case STATE_FIN_COMMENT:
            buffer->begin_ptr = buffer->forward_ptr;
            if(buffer->buffer_loaded)
                buffer->buffer_loaded = false;
            ret_tok = calloc(1, sizeof(token));
            ret_tok->lexeme = comment->name;
            ret_tok->name = TK_COMMENT;
            ret_tok->line_num = line_number++;
            
            state = STATE_START;
            return ret_tok;
            break;

        case STATE_INTR_COMMENT:
            if(buffer_char != '\n')
                state = STATE_INTR_COMMENT;
            else 
                state = STATE_FIN_COMMENT;
            break;
            
        case STATE_INTR_NUM:
            if(buffer_char == '.')
                state = STATE_INTR_RNUM_NO_EXP_NO_DIGIT;
            else if(!isdigit(buffer_char))
                state = STATE_FIN_NUM;
            break;
        
        case STATE_INTR_RNUM_NO_EXP_NO_DIGIT:
            if(isdigit(buffer_char))
                state = STATE_INTR_RNUM_NO_EXP_ONE_DIGIT;
            else
                state = STATE_TRAP_PATTERN; 
            break;
        
        case STATE_INTR_RNUM_NO_EXP_ONE_DIGIT:
            if(isdigit(buffer_char))
                state = STATE_INTR_RNUM_NO_EXP_TWO_DIGIT;
            else
                state = STATE_TRAP_PATTERN;
            break;
                
        case STATE_INTR_RNUM_NO_EXP_TWO_DIGIT:
            if(buffer_char == 'E')
                state = STATE_INTR_RNUM_EXP_NO_DIGIT;
            else
                state = STATE_FIN_RNUM_NO_EXP;
            break;
        
        case STATE_INTR_RNUM_EXP_NO_DIGIT:
            if(isdigit(buffer_char))
                state = STATE_INTR_RNUM_EXP_ONE_DIGIT;
            else if(buffer_char == '+' || buffer_char == '-')
                state = STATE_INTR_RNUM_EXP_NO_DIGIT_SIGNED;
            else
                state = STATE_TRAP_PATTERN;
            break;
        
        case STATE_INTR_RNUM_EXP_ONE_DIGIT:
            if(isdigit(buffer_char))
                state = STATE_FIN_RNUM_EXP;
            else
                state = STATE_TRAP_PATTERN;
            break;
        
        case STATE_INTR_RNUM_EXP_NO_DIGIT_SIGNED:
            if(isdigit(buffer_char))
                state = STATE_INTR_RNUM_EXP_ONE_DIGIT;
            else
                state = STATE_TRAP_PATTERN;
            break;
        
        case STATE_INTR_ID_ONE_CHAR:
            if(islower(buffer_char))
                state = STATE_INTR_KEY_OR_FIELD;
            else if(buffer_char >= '2' && buffer_char <= '7')
                state = STATE_INTR_ID_MORE_CHARS;
            else
                state = STATE_FIN_KEY_OR_FIELD;
            break;
            
        case STATE_INTR_ID_MORE_CHARS:
            if(buffer_char == 'b' || buffer_char == 'c' || buffer_char == 'd'){
                state = STATE_INTR_ID_MORE_CHARS;
                break;
            }
        
        case STATE_INTR_ID_MORE_DIGITS:
            if(buffer_char >= '2' && buffer_char <= '7')
                state = STATE_INTR_ID_MORE_DIGITS;
            else 
                state = STATE_FIN_ID;
            break;
        
        case STATE_INTR_KEY_OR_FIELD:
            if(islower(buffer_char))
                state = STATE_INTR_KEY_OR_FIELD;
            else
                state = STATE_FIN_KEY_OR_FIELD;
            break;

        case STATE_INTR_FUNCTION_:
            if(isalpha(buffer_char))
                state = STATE_INTR_FUNCTION_CHARS;
            else 
                state = STATE_TRAP_PATTERN;
            
        case STATE_INTR_FUNCTION_CHARS:
            if(isalpha(buffer_char)){
                state = STATE_INTR_FUNCTION_CHARS;
                break;
            }

        case STATE_INTR_FUNCTION_DIGITS:
            if(isdigit(buffer_char))
                state = STATE_INTR_FUNCTION_DIGITS;
            else 
                state = STATE_FIN_FUNCTION;
            break;
        
        case STATE_INTR_RUID_HASH:
            if(islower(buffer_char))
                state = STATE_INTR_RUID_CHARS;
            else 
                state = STATE_TRAP_PATTERN;
            break;
        
        case STATE_INTR_RUID_CHARS:
            if(islower(buffer_char))
                state = STATE_INTR_RUID_CHARS;
            else 
                state = STATE_FIN_RUID;
            break;

        case STATE_INTR_ANGULAR_OPEN:
            if(buffer_char == '=')
                state = STATE_FIN_LE;
            else if(buffer_char == '-')
                state = STATE_INTR_ANGULAR_OPEN_MINUS;
            else 
                state = STATE_FIN_LT;
            break;

        case STATE_INTR_ANGULAR_OPEN_MINUS:
            if(buffer_char == '-')
                state = STATE_INTR_ALMOST_ASSIGN;
            else 
                state = STATE_FIN_LT_MIUNS;
            break;

        case STATE_INTR_ALMOST_ASSIGN:
            if(buffer_char == '-')
                state = STATE_FIN_ASSIGNOP;
            else 
                state = STATE_TRAP_PATTERN;
            break;

        case STATE_INTR_ANGULAR_CLOSE:
            if(buffer_char == '=')
                state = STATE_FIN_GE;
            else 
                state = STATE_FIN_GT;
            break;

        case STATE_INTR_WHITESPACE:
            if(!isspace(buffer_char))
                state = STATE_FIN_WHITESPACE;
            else if(buffer_char == '\n')
                state = STATE_FIN_NEWLINE;
            else
                state = STATE_INTR_WHITESPACE;
            break;

        case STATE_INTR_AND_ONE:
            if(buffer_char == '&')
                state = STATE_INTR_AND_TWO;
            else
                state = STATE_TRAP_SYMBOL_INTR;
            break;

        case STATE_INTR_AND_TWO:
            if(buffer_char == '&')
                state = STATE_FIN_AND;
            else
                state = STATE_TRAP_PATTERN;
            break; 

        case STATE_INTR_AT_ONE:
            if(buffer_char == '@')
                state = STATE_INTR_AT_TWO;
            else
                state = STATE_TRAP_SYMBOL_INTR;
            break;

        case STATE_INTR_AT_TWO:
            if(buffer_char == '@')
                state = STATE_FIN_OR;
            else
                state = STATE_TRAP_PATTERN;
            break;

        case STATE_INTR_BANG: 
            if(buffer_char == '=')
                state = STATE_FIN_NOT_EQUAL;
            else    
                state = STATE_TRAP_SYMBOL_INTR;
            break;

        case STATE_INTR_EQUAL:
            if(buffer_char == '=')
                state = STATE_FIN_EQUAL;
            else    
                state = STATE_TRAP_SYMBOL_INTR;
            break;
            
        case STATE_TRAP_SYMBOL_START:
            lexeme = retract_and_update(0);
            printf("Line no. %-4d: \033[1;31mError:\033[0m Unknown Symbol \"\033[1m%s\033[0m\"\n", line_number, lexeme);
            state = STATE_START;
            return NULL;
            break;
        
        case STATE_TRAP_PATTERN:
            lexeme = retract_and_update(error_retract);
            printf("Line no. %-4d: \033[1;31mError:\033[0m Unknown Pattern \"\033[1m%s\033[0m\"\n", line_number, lexeme);
            state = STATE_START;
            error_retract = 1;
            return NULL;
            break;

        case STATE_TRAP_SYMBOL_INTR:
            lexeme = retract_and_update(1);
            printf("Line no. %-4d: \033[1;31mError:\033[0m Unknown Symbol \"\033[1m%s\033[0m\"\n", line_number, lexeme);
            state = STATE_START;
            return NULL;
            break;

        default: 
            if(buffer_char == '\0')
                exit(EXIT_SUCCESS);
            exit(EXIT_FAILURE);
            break;
    }
    
    (buffer->forward_ptr)++;
    if(buffer->forward_ptr == BUFFER_SIZE - 1)
        swap_buffer(file_ptr);
    
    return ret_tok;
}