#include <assert.h>

#include "lexer.h"
#include "symbol_table.h"

twin_buffer* buffer = NULL;
token* get_next_token_helper(FILE* file_ptr);
void buffer_cleanup(void) __attribute__((destructor));

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
    //if (strcmp(string, "SYN") == 0) return SYN;

    // If no match is found, return a default or error value
    return TK_ERROR;  // You could choose to return an error code or handle this case differently
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
        //case SYN: return "SYN";
        default: return "TK_ERROR";
    }
}

void buffer_cleanup(void){
    if(buffer == NULL)
        return;

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

char* retract_and_update(unsigned int no_of_times) {
    int new_pos = buffer->forward_ptr - no_of_times;
    static char* ret_tok = NULL;

    if(new_pos > 0){
        buffer->forward_ptr = new_pos;
        if(buffer->begin_ptr < 0){
            int begin_len = BUFFER_SIZE + buffer->begin_ptr;
            int str_len = buffer->forward_ptr + begin_len - 1;
            if(ret_tok == NULL || strlen(ret_tok) < str_len + 1)
                ret_tok = realloc(ret_tok, (str_len + 1)*sizeof(char));
            strncpy(ret_tok, buffer->load_buffer + begin_len, -(buffer->begin_ptr + 1));
            strncpy(ret_tok + begin_len, buffer->active_buffer, buffer->forward_ptr);
            ret_tok[str_len] = '\0';
        }
        else{
            int str_len = buffer->forward_ptr - buffer->begin_ptr;
            if(ret_tok == NULL || strlen(ret_tok) < str_len + 1)
                ret_tok = realloc(ret_tok, (str_len + 1)*sizeof(char));
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
    buffer->forward_ptr = BUFFER_SIZE - new_pos - 1; // May lead to an error MUST CHECK
        
    int str_len = buffer->forward_ptr - buffer->begin_ptr + 1;
    if(ret_tok == NULL || strlen(ret_tok) < str_len + 1)
        ret_tok = realloc(ret_tok, (str_len + 1)*sizeof(char));
    strncpy(ret_tok, buffer->active_buffer + buffer->begin_ptr, str_len);
    ret_tok[str_len] = '\0';

    buffer->begin_ptr = BUFFER_SIZE - new_pos - 1;
    return ret_tok;
}

void buffer_init(FILE* file_ptr){
    buffer_cleanup();
    buffer = (twin_buffer*)calloc(1, sizeof(twin_buffer));
    if(!buffer){
        fprintf(stderr, "Error: Failed to initialize Twin buffer");
        exit(EXIT_FAILURE);
    }
    
    buffer->active_buffer = (char*)calloc(BUFFER_SIZE, sizeof(char));
    buffer->load_buffer = (char*)calloc(BUFFER_SIZE, sizeof(char));
    if(!buffer->active_buffer || !buffer->load_buffer){
        fprintf(stderr, "Error: Failed to initialize Twin buffer");

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

token* get_next_token(FILE* file_ptr){
    token* ret;
    while(!(ret = get_next_token_helper(file_ptr)));
    return ret;
}

token* get_next_token_helper(FILE* file_ptr){
    static int state = 0;
    static int line_number = 1;
    static entry* comment = NULL;

    if(buffer == NULL)
        buffer_init(file_ptr);
    if(comment == NULL)
        comment = symbol_table_insert("%", TK_COMMENT);

    char buffer_char = buffer->active_buffer[buffer->forward_ptr];
    char* lexeme = NULL;
    token* ret_tok = NULL;
    entry* sym_tab = NULL;
    
    switch(state) {
        case 0:
            if(isdigit(buffer_char))
                state = 35;
            else if(buffer_char == 'b' || buffer_char == 'c' || buffer_char == 'd')
                state = 42;
            else if(islower(buffer_char))
                state = 45;
            else{
                switch (buffer_char) {
                    case '%':
                        state = 34;
                        break;

                    case '_':
                        state = 46;
                        break;
                    
                    case '#':
                        state = 49;
                        break;
                    
                    case '<':
                        state = 51;
                        break;

                    case '>':
                        state = 54;
                        break;

                    case '[':
                        state = 14;
                        break;

                    case ']':
                        state = 15;
                        break;

                    case '\t':
                    case ' ':
                        state = 55;
                        break;

                    case '\n':
                        state = 32;
                        break;

                    case ',':
                        state = 16;
                        break;

                    case ';':
                        state = 17;
                        break;

                    case ':':
                        state = 18;
                        break;

                    case '.':
                        state = 19;
                        break;

                    case '(':
                    case ')':
                        state = buffer_char - 20;
                        break;

                    case '+':
                        state = 22;
                        break;

                    case '-':
                        state = 23;
                        break;

                    case '*':
                        state = 24;
                        break;

                    case '/':
                        state = 25;
                        break;

                    case '&':
                        state = 56;
                        break;

                    case '@':
                        state = 58;
                        break;
                    
                    case '~':
                        state = 28;
                        break;
                    
                    case '!':
                        state = 60;
                        break;

                    case '=':
                        state = 61;
                        break;

                    default:
                        state = 62;
                        break;
                }
            }
            break;
        
        case 1:
            entry* sym_tab = symbol_table_insert(retract_and_update(1), TK_NUM);
            ret_tok = calloc(1, sizeof(token));
            ret_tok->name = TK_NUM;
            ret_tok->lexeme = sym_tab->name;
            ret_tok->line_num = line_number;
            ret_tok->value.num = atoi(ret_tok->lexeme);
            ret_tok->is_value_int = true;
            
            state = 0;
            return ret_tok;
            break;
        
        case 2: case 3:
            sym_tab = symbol_table_insert(state == 2 ? retract_and_update(1) : retract_and_update(0), TK_RNUM);
            ret_tok = calloc(1, sizeof(token));
            ret_tok->name = TK_RNUM;
            ret_tok->lexeme =  (state == 2 ? retract_and_update(1) : retract_and_update(0));
            ret_tok->line_num = line_number;
            ret_tok->value.r_num = strtof(ret_tok->lexeme, NULL);
            ret_tok->is_value_int = false;
            
            state = 0;
            return ret_tok;
            break;

        case 4:
            sym_tab = symbol_table_insert(retract_and_update(1), TK_ID);
            ret_tok = calloc(1, sizeof(token));
            ret_tok->name = TK_ID;
            ret_tok->lexeme = sym_tab->name;
            ret_tok->line_num = line_number;
            
            state = 0;
            return ret_tok;
            break;

        case 5:
            lexeme = retract_and_update(1);
            sym_tab = symbol_table_insert(lexeme, TK_FIELDID);
            ret_tok = calloc(1, sizeof(token));
            ret_tok->name = sym_tab->type;
            ret_tok->lexeme = sym_tab->name;
            ret_tok->line_num = line_number;

            state = 0;
            return ret_tok;
            break;

        case 6:
            lexeme = retract_and_update(1);
            sym_tab = symbol_table_insert(lexeme, TK_FUNID);
            ret_tok = calloc(1, sizeof(token));
            ret_tok->name = sym_tab->type;
            ret_tok->lexeme = sym_tab->name;
            ret_tok->line_num = line_number;
            
            state = 0;
            return ret_tok;
            break;
        
        case 7:
            sym_tab = symbol_table_insert(retract_and_update(1), TK_RUID);
            ret_tok = calloc(1, sizeof(token));
            ret_tok->name = TK_RUID;
            ret_tok->lexeme = sym_tab->name;
            ret_tok->line_num = line_number;

            state = 0;
            return ret_tok;
            break;
        
        case 8:
            sym_tab = symbol_table_insert(retract_and_update(0), TK_LE);
            ret_tok = calloc(1, sizeof(token));
            ret_tok->name = TK_LE;
            ret_tok->lexeme = sym_tab->name;
            ret_tok->line_num = line_number;
            
            state = 0;
            return ret_tok;
            break;

        case 9:
            sym_tab = symbol_table_insert(retract_and_update(0), TK_ASSIGNOP);
            ret_tok = calloc(1, sizeof(token));
            ret_tok->name = TK_ASSIGNOP;
            ret_tok->lexeme = sym_tab->name;
            ret_tok->line_num = line_number;
            
            state = 0;
            return ret_tok;
            break;

        case 10:
            sym_tab = symbol_table_insert(retract_and_update(2), TK_LT);
            ret_tok = calloc(1, sizeof(token));
            ret_tok->name = TK_LT;
            ret_tok->lexeme = sym_tab->name;
            ret_tok->line_num = line_number;
            
            state = 0;
            return ret_tok;
            break;
        
        case 11:
            sym_tab = symbol_table_insert(retract_and_update(1), TK_LT);
            ret_tok = calloc(1, sizeof(token));
            ret_tok->name = sym_tab->type;
            ret_tok->lexeme = retract_and_update(1);
            ret_tok->line_num = line_number;
            
            state = 0;
            return ret_tok;
            break;

        case 12:
            sym_tab = symbol_table_insert(retract_and_update(0), TK_GE);
            ret_tok = calloc(1, sizeof(token));
            ret_tok->name = TK_GE;
            ret_tok->lexeme = sym_tab->name;
            ret_tok->line_num = line_number;
            
            state = 0;
            return ret_tok;
            break;

        case 13:
            sym_tab = symbol_table_insert(retract_and_update(1), TK_GT);
            ret_tok = calloc(1, sizeof(token));
            ret_tok->name = TK_GT;
            ret_tok->lexeme = sym_tab->name;
            ret_tok->line_num = line_number;
            
            state = 0;
            return ret_tok;
            break;

        case 14:
        case 15:
        case 16:
        case 17:
        case 18:
        case 19:
        case 20:
        case 21:
        case 22:
        case 23:
        case 24:
        case 25:
        case 26:
        case 27:
        case 28:
        case 29:
        case 30:
            sym_tab = symbol_table_insert(retract_and_update(0), (token_type)state);
            ret_tok = calloc(1, sizeof(token));
            ret_tok->name = sym_tab->type;
            ret_tok->lexeme = sym_tab->name;
            ret_tok->line_num = line_number;
            
            state = 0;
            return ret_tok;
            break;
        
        case 31:
            buffer->forward_ptr--;
            buffer->begin_ptr = buffer->forward_ptr;

            state = 0;
            return ret_tok;
            break;
        
        case 32:
            buffer->begin_ptr = buffer->forward_ptr;
            line_number++;

            state = 0;
            return NULL;
            break;

        case 33:
            buffer->begin_ptr = buffer->forward_ptr;
            ret_tok = calloc(1, sizeof(token));
            ret_tok->lexeme = comment->name;
            ret_tok->name = TK_COMMENT;
            ret_tok->line_num = line_number++;
            
            state = 0;
            return ret_tok;
            break;

        case 34:
            if(buffer_char != '\n')
                state = 34;
            else 
                state = 33;
            break;
            
        case 35:
            if(!isdigit(buffer_char))
                state = 1;
            else if(buffer_char == '.')
                state = 36;
            break;
        
        case 36:
            if(isdigit(buffer_char))
                state = 37;
            else
                state = 62; //TODO or put the error here itself ig
            break;
        
        case 37:
            if(isdigit(buffer_char))
                state = 38;
            else
                state = 62;
            break;
                
        case 38:
            if(buffer_char == 'E')
                state = 39;
            else
                state = 2; //TODO maybe on digit go to 62?
            break;
        
        case 39:
            if(isdigit(buffer_char))
                state = 40;
            else if(buffer_char == '+' || buffer_char == '-')
                state = 41;
            else
                state = 62;
            break;
        
        case 40:
            if(isdigit(buffer_char))
                state = 3;
            else
                state = 62;
            break;
        
        case 41:
            if(isdigit(buffer_char))
                state = 40;
            else
                state = 62;
            break;
        
        case 42:
            if(islower(buffer_char))
                state = 45;
            else if(buffer_char >= '2' && buffer_char <= '7')
                state = 43;
            else
                state = 62;
            break;
            
        case 43:
            if(buffer_char == 'b' || buffer_char == 'c' || buffer_char == 'd'){
                state = 43;
                break;
            }
        
        case 44:
            if(buffer_char >= '2' && buffer_char <= '7')
                state = 44;
            else 
                state = 4;
            break;
        
        case 45:
            if(islower(buffer_char))
                state = 45;
            else
                state = 5;
            break;

        case 46:
            if(isalpha(buffer_char))
                state = 47;
            else 
                state = 62;
            
        case 47:
            if(isalpha(buffer_char)){
                state = 47;
                break;
            }

        case 48:
            if(isdigit(buffer_char))
                state = 48;
            else 
                state = 6;
            break;
        
        case 49:
            if(islower(buffer_char))
                state = 50;
            else 
                state = 62;
            break;
        
        case 50:
            if(islower(buffer_char))
                state = 50;
            else 
                state = 7;
            break;

        case 51:
            if(buffer_char == '=')
                state = 8;
            else if(buffer_char == '-')
                state = 52;
            else 
                state = 11;
            break;

        case 52:
            if(buffer_char == '-')
                state = 53;
            else 
                state = 10;
            break;

        case 53:
            if(buffer_char == '-')
                state = 9;
            else 
                state = 62;
            break;

        case 54:
            if(buffer_char == '=')
                state = 12;
            else 
                state = 13;
            break;

        case 55:
            if(!isspace(buffer_char))
                state = 31;
            else if(buffer_char == '\n')
                state = 32;
            else
                state = 55;
            break;

        case 56:
            if(buffer_char == '&')
                state = 57;
            else
                state = 62;
            break;

        case 57:
            if(buffer_char == '&')
                state = 26;
            else
                state = 62;
            break; 

        case 58:
            if(buffer_char == '@')
                state = 57;
            else
                state = 62;
            break;

        case 59:
            if(buffer_char == '@')
                state = 27;
            else
                state = 62;
            break;

        case 60: 
        case 61:
            if(buffer_char == '=')
                state -= 31;
            else    
                state = 62;
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