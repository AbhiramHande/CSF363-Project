#include "parse.h"

#include <time.h>

#define MAX_LINE_LENGTH 256

non_terminal** non_terminals = NULL;
int non_terminal_count = 0;
int terminal_count = 61;
non_terminal* start_symbol = NULL;
hash_map* parse_map = NULL;
production** parse_table = NULL;


void first_and_follow_cleanup(void);

TokenName stringToTokenName(const char* tokenStr) {
    // List of token names corresponding to the enum
    if (strcmp(tokenStr, "TK_ASSIGNOP") == 0) return TK_ASSIGNOP;
    if (strcmp(tokenStr, "TK_COMMENT") == 0) return TK_COMMENT;
    if (strcmp(tokenStr, "TK_FIELDID") == 0) return TK_FIELDID;
    if (strcmp(tokenStr, "TK_ID") == 0) return TK_ID;
    if (strcmp(tokenStr, "TK_NUM") == 0) return TK_NUM;
    if (strcmp(tokenStr, "TK_RNUM") == 0) return TK_RNUM;
    if (strcmp(tokenStr, "TK_FUNID") == 0) return TK_FUNID;
    if (strcmp(tokenStr, "TK_RUID") == 0) return TK_RUID;
    if (strcmp(tokenStr, "TK_WITH") == 0) return TK_WITH;
    if (strcmp(tokenStr, "TK_PARAMETERS") == 0) return TK_PARAMETERS;
    if (strcmp(tokenStr, "TK_END") == 0) return TK_END;
    if (strcmp(tokenStr, "TK_WHILE") == 0) return TK_WHILE;
    if (strcmp(tokenStr, "TK_UNION") == 0) return TK_UNION;
    if (strcmp(tokenStr, "TK_ENDUNION") == 0) return TK_ENDUNION;
    if (strcmp(tokenStr, "TK_DEFINETYPE") == 0) return TK_DEFINETYPE;
    if (strcmp(tokenStr, "TK_AS") == 0) return TK_AS;
    if (strcmp(tokenStr, "TK_TYPE") == 0) return TK_TYPE;
    if (strcmp(tokenStr, "TK_MAIN") == 0) return TK_MAIN;
    if (strcmp(tokenStr, "TK_GLOBAL") == 0) return TK_GLOBAL;
    if (strcmp(tokenStr, "TK_PARAMETER") == 0) return TK_PARAMETER;
    if (strcmp(tokenStr, "TK_LIST") == 0) return TK_LIST;
    if (strcmp(tokenStr, "TK_SQL") == 0) return TK_SQL;
    if (strcmp(tokenStr, "TK_SQR") == 0) return TK_SQR;
    if (strcmp(tokenStr, "TK_INPUT") == 0) return TK_INPUT;
    if (strcmp(tokenStr, "TK_OUTPUT") == 0) return TK_OUTPUT;
    if (strcmp(tokenStr, "TK_INT") == 0) return TK_INT;
    if (strcmp(tokenStr, "TK_REAL") == 0) return TK_REAL;
    if (strcmp(tokenStr, "TK_COMMA") == 0) return TK_COMMA;
    if (strcmp(tokenStr, "TK_SEM") == 0) return TK_SEM;
    if (strcmp(tokenStr, "TK_COLON") == 0) return TK_COLON;
    if (strcmp(tokenStr, "TK_DOT") == 0) return TK_DOT;
    if (strcmp(tokenStr, "TK_ENDWHILE") == 0) return TK_ENDWHILE;
    if (strcmp(tokenStr, "TK_OP") == 0) return TK_OP;
    if (strcmp(tokenStr, "TK_CL") == 0) return TK_CL;
    if (strcmp(tokenStr, "TK_IF") == 0) return TK_IF;
    if (strcmp(tokenStr, "TK_THEN") == 0) return TK_THEN;
    if (strcmp(tokenStr, "TK_ENDIF") == 0) return TK_ENDIF;
    if (strcmp(tokenStr, "TK_READ") == 0) return TK_READ;
    if (strcmp(tokenStr, "TK_WRITE") == 0) return TK_WRITE;
    if (strcmp(tokenStr, "TK_RETURN") == 0) return TK_RETURN;
    if (strcmp(tokenStr, "TK_PLUS") == 0) return TK_PLUS;
    if (strcmp(tokenStr, "TK_MINUS") == 0) return TK_MINUS;
    if (strcmp(tokenStr, "TK_MUL") == 0) return TK_MUL;
    if (strcmp(tokenStr, "TK_DIV") == 0) return TK_DIV;
    if (strcmp(tokenStr, "TK_CALL") == 0) return TK_CALL;
    if (strcmp(tokenStr, "TK_RECORD") == 0) return TK_RECORD;
    if (strcmp(tokenStr, "TK_ENDRECORD") == 0) return TK_ENDRECORD;
    if (strcmp(tokenStr, "TK_ELSE") == 0) return TK_ELSE;
    if (strcmp(tokenStr, "TK_AND") == 0) return TK_AND;
    if (strcmp(tokenStr, "TK_OR") == 0) return TK_OR;
    if (strcmp(tokenStr, "TK_NOT") == 0) return TK_NOT;
    if (strcmp(tokenStr, "TK_LT") == 0) return TK_LT;
    if (strcmp(tokenStr, "TK_LE") == 0) return TK_LE;
    if (strcmp(tokenStr, "TK_EQ") == 0) return TK_EQ;
    if (strcmp(tokenStr, "TK_GT") == 0) return TK_GT;
    if (strcmp(tokenStr, "TK_GE") == 0) return TK_GE;
    if (strcmp(tokenStr, "TK_NE") == 0) return TK_NE;
    if (strcmp(tokenStr, "EPS") == 0) return EPSILON;
    if (strcmp(tokenStr, "TK_ERROR") == 0) return TK_ERROR;
    if (strcmp(tokenStr, "$") == 0) return DOLLAR;
    if (strcmp(tokenStr, "SYN") == 0) return SYN;

    // If no match is found, return a default or error value
    return TK_ERROR;  // You could choose to return an error code or handle this case differently
}

const char* tokenNameToString(TokenName token) {
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
        case SYN: return "SYN";
        default: return "UNKNOWN";
    }
}

int main(int argc, char *argv[]) {
    atexit(first_and_follow_cleanup);

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <grammar_file>\n", argv[0]);
        return 1;
    }

    FILE* file = fopen(argv[1], "r");
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, MAX_LINE_LENGTH, file)) {

        // Parse the production rule to remove the arrow
        char* arrow = strstr(line, "===>");
        if (!arrow) continue;
        *arrow = '\0';
        char* lhs = line;
        char* rhs = arrow + 4;

        // Parse the rule to remove leading and trailing spaces
        while (isspace(*lhs)) 
            lhs++;
        char* end = lhs + strlen(lhs) - 1;
        while (end > lhs && isspace(*end)) 
            end--;
        *(end + 1) = '\0';

        while (isspace(*rhs)) 
            rhs++;
        end = rhs + strlen(rhs) - 1;
        while (end > rhs && isspace(*end)) 
            end--;
        *(end + 1) = '\0';

        // Check the LHS for non-terminal
        if (*lhs != '<' || lhs[strlen(lhs)-1] != '>') {
            fprintf(stderr, "Invalid non-terminal: %s\n", lhs);
            continue;
        }
        char* nt_name = strndup(lhs + 1, strlen(lhs) - 2);

        non_terminal* nt = find_non_terminal(nt_name);
        if (!nt) {
            nt = (non_terminal*)malloc(sizeof(non_terminal));
            nt->name = strdup(nt_name);
            nt->prod_count = 0;
            nt->first_size = 0;
            nt->follow_size = 0;
            nt->productions = NULL;
            nt->first_set = NULL;
            nt->follow_set = NULL;
            nt->has_epsilon_in_first = false;
            
            non_terminals = (non_terminal**)realloc(non_terminals, (non_terminal_count + 1)*sizeof(non_terminal*));
            non_terminals[non_terminal_count++] = nt;
        }
        free(nt_name);
        nt_name = NULL;

        // Add start symbol if there is none
        if (!start_symbol)
            start_symbol = nt;

        // Prase the RHS of the rule
        char* save_rule;
        char* alt = strtok_r(rhs, "|", &save_rule);
        while (alt) {
            production* prod = (production*)malloc(sizeof(production));
            prod->symbols = NULL;
            prod->count = 0;
            
            // Parse each rule for tokens, non-terminals and epsilon
            char* save_token;
            char* token = strtok_r(alt, " \t", &save_token);
            while (token) {
                symbol* sym = (symbol*)malloc(sizeof(symbol));
                if (strcmp(token, "EPS") == 0) {
                    sym->type = SYM_EPSILON;
                    sym->value.token_value = EPSILON;
                } 
                else if (token[0] == '<' && token[strlen(token)-1] == '>') {
                    sym->type = SYM_NON_TERMINAL;
                    sym->value.name = strndup(token + 1, strlen(token) - 2);
                } 
                else if (strncmp(token, "TK_", 3) == 0) {
                    sym->type = SYM_TERMINAL;
                    sym->value.token_value = stringToTokenName(token);
                } 
                else {
                    fprintf(stderr, "Invalid symbol: %s\n", token);
                    free(sym);
                    sym = NULL;
                    token = strtok(NULL, " \t");
                    continue;
                }

                prod->symbols = (symbol**)realloc(prod->symbols, (prod->count + 1) * sizeof(symbol*));
                prod->symbols[prod->count++] = sym;

                token = strtok_r(NULL, " \t", &save_token);
            }

            nt->productions = (production**)realloc(nt->productions, (nt->prod_count + 1)*sizeof(production*));
            nt->productions[nt->prod_count++] = prod;

            alt = strtok_r(NULL, "|", &save_rule);
        }
    }

    fclose(file);

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

        for(int i = 0; i < non_terminal_count; i++)
            compute_first_set(non_terminals[i]);
        compute_follow_set(start_symbol, NULL, NULL, 0);

        //print_first_and_follow_sets(true, true);
        //generate_parse_table();
        generate_parse_map();

    clock_gettime(CLOCK_MONOTONIC, &end);

    //print_parse_tree();
    print_parse_map();
    long long time = (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec);
    printf("\nTotal time taken to create first and follow sets and generate parse table is %lld ns\n", time);
    return 0;
}

non_terminal* find_non_terminal(const char* name) {
    for (int i = 0; i < non_terminal_count; i++) {
        if (strcmp(non_terminals[i]->name, name) == 0) {
            return non_terminals[i];
        }
    }
    return NULL;
}

void add_to_first_set(non_terminal* nt, const TokenName element) {
    for (int i = 0; i < nt->first_size; i++)
        if (nt->first_set[i] == element)
            return;

    nt->first_set = (TokenName*)realloc(nt->first_set, (nt->first_size + 1)*sizeof(TokenName));
    nt->first_set[nt->first_size++] = element;
    if (element == EPSILON)
        nt->has_epsilon_in_first = true;

    return;
}

void add_to_follow_set(non_terminal *nt, const TokenName element) {
    for (int i = 0; i < nt->follow_size; i++)
        if (nt->follow_set[i] == element)
            return;

    nt->follow_set = (TokenName*)realloc(nt->follow_set, (nt->follow_size + 1)*sizeof(TokenName));
    nt->follow_set[nt->follow_size++] = element;
}

void compute_first_set(non_terminal* nt) {
    if(nt->first_size != 0)
        return;
    
    for(int i = 0; i < nt->prod_count; i++){
        production* prod = nt->productions[i];
        for(int j = 0; j < prod->count; j++){
            symbol* sym = prod->symbols[j];
            if(sym->type == SYM_TERMINAL){
                add_to_first_set(nt, sym->value.token_value);
                break;
            }
            else if(sym->type == SYM_EPSILON){
                nt->has_epsilon_in_first = true;
                add_to_first_set(nt, EPSILON);
                break;
            }
            else{
                non_terminal* prod_nt = find_non_terminal(sym->value.name);
                compute_first_set(prod_nt);
                for(int k = 0; k < prod_nt->first_size; k++) {
                    if(prod_nt->first_set[k] != EPSILON)
                        add_to_first_set(nt, prod_nt->first_set[k]);
                }

                if(!prod_nt->has_epsilon_in_first)
                    break;
                else if(j == prod->count - 1)
                    add_to_first_set(nt, EPSILON);

            }

        }
    }

    return;
}

void compute_follow_set(non_terminal* nt, non_terminal* A, production* aToAlpha, int nt_pos){
    int original_size = nt->follow_size;

    if(nt == start_symbol)
        add_to_follow_set(nt, DOLLAR);
    else{
        bool add_follow_A = false;
        if(nt_pos + 1 == aToAlpha->count)
            add_follow_A = true;
        else{
            symbol** syms = aToAlpha->symbols + nt_pos + 1; //Potential error: overflow and access invalid memory
            int syms_size = aToAlpha->count - (nt_pos + 1);
            int first_seq_size = 0;
            TokenName* first_seq = compute_first_of_sequence(syms, syms_size, &first_seq_size);

            
            for(int i = 0; i < first_seq_size; i++){
                if(first_seq[i] != EPSILON)
                    add_to_follow_set(nt, first_seq[i]);
                else
                    add_follow_A = true;
            }

            free(first_seq);
            first_seq = NULL;
            first_seq_size = 0;
        }

        if(add_follow_A)
            for(int i = 0; i < A->follow_size; i++)
                add_to_follow_set(nt, A->follow_set[i]);
    }

    if(nt->follow_size != original_size){
        for(int i = 0; i < nt-> prod_count; i++){
            production* prod = nt->productions[i];
            for(int j = prod->count - 1; j >= 0; j--){
                symbol* sym = prod->symbols[j];
                if(sym->type == SYM_NON_TERMINAL)
                    compute_follow_set(find_non_terminal(sym->value.name), nt, prod, j);
            }
        }
    }
    return;
}

bool contains_EPS(TokenName* set, int size) {
    for (int i = 0; i < size; i++)
        if (set[i] == EPSILON)
            return true;

    return false;
}

void print_first_and_follow_sets(bool print_first, bool print_follow){
    if(!print_first && !print_follow)
        return;

    for (int i = 0; i < non_terminal_count; i++) {
        non_terminal* nt = non_terminals[i];
        if(print_first){
            printf("First(%s): {", nt->name);
            for (int j = 0; j < nt->first_size; j++) {
                printf(" %s", tokenNameToString(nt->first_set[j]));
                if (j < nt->first_size - 1) 
                    printf(",");
            }
            printf(" }\n");
        }
        
        if(print_follow){
            printf("Follow(%s): {", nt->name);
            for (int j = 0; j < nt->follow_size; j++) {
                printf(" %s", tokenNameToString(nt->follow_set[j]));
                if (j < nt->follow_size - 1) 
                    printf(",");
            }
            printf(" }\n");
        }
        
        if(print_first && print_follow)
            printf("\n");
    }
}

void first_and_follow_cleanup(void){
    for (int i = 0; i < non_terminal_count; i++) {
        non_terminal* nt = non_terminals[i];
        for (int j = 0; j < nt->prod_count; j++) {
            production* prod = nt->productions[j];
            for (int k = 0; k < prod->count; k++) {
                if(prod->symbols[k]->type == SYM_NON_TERMINAL){
                    free(prod->symbols[k]->value.name);
                    prod->symbols[k]->value.name= NULL;
                }
                free(prod->symbols[k]);
                prod->symbols[k] = NULL;
            }
            free(prod->symbols);
            prod->symbols = NULL;
            free(prod);
            prod = NULL;
        }

        free(nt->productions);
        free(nt->first_set);
        free(nt->follow_set);
        free(nt->name);

        nt->productions = NULL;
        nt->first_set = NULL;        
        nt->follow_set = NULL;
        nt->name = NULL;

        free(nt);
        nt = NULL;
    }

    free(non_terminals);
    free(parse_table);
    map_cleanup(parse_map);

    non_terminals = NULL;
    start_symbol = NULL;
    parse_table = NULL;
    parse_map = NULL;

    return;
}

void add_to_set(TokenName** set, int* size, const TokenName element) {
    if(element == TK_ERROR)
        return;

    for (int i = 0; i < *size; i++)
        if ((*set)[i] == element)
            return;

    *set = (TokenName*)realloc(*set, (*size + 1)*sizeof(TokenName));
    (*set)[(*size)++] = element;
}

TokenName* compute_first_of_sequence(symbol** sym_seq, int sym_seq_count, int* result_size) {
    TokenName* result = NULL;
    int size = *result_size = 0;
    bool can_derive_epsilon = true;

    for (int i = 0; i < sym_seq_count; i++) {
        symbol* sym = sym_seq[i];
        if (sym->type == SYM_TERMINAL) {
            add_to_set(&result, &size, sym->value.token_value);
            can_derive_epsilon = false;
            break;
        } 
        else if (sym->type == SYM_EPSILON) {
            can_derive_epsilon = true;
            break;
        } 
        else {
            non_terminal* sym_nt = find_non_terminal(sym->value.name);
            for (int j = 0; j < sym_nt->first_size; j++)
                if (sym_nt->first_set[j] != EPSILON)
                    add_to_set(&result, &size, sym_nt->first_set[j]);

            if (!sym_nt->has_epsilon_in_first) {
                can_derive_epsilon = false;
                break;
            }
        }
    }

    if (can_derive_epsilon)
        add_to_set(&result, &size, EPSILON);

    *result_size = size;
    return result;
}

#ifdef NO_HASHMAP
void generate_parse_table(){
    parse_table = calloc(non_terminal_count * terminal_count, sizeof(production*));
    for(int i = 0; i < non_terminal_count; i++){
        non_terminal* nt = non_terminals[i];
        for(int j = 0; j < nt->prod_count; j++){
            production* prod = nt->productions[j];
            int first_seq_count = 0;
            TokenName* tok_set = compute_first_of_sequence(prod->symbols, prod->count, &first_seq_count);
            for(int k = 0; k < first_seq_count; k++){
                TokenName tok = tok_set[k];
                if(tok == EPSILON){
                    for(int l = 0; l < nt->follow_size; l++){
                        TokenName follow_tok = nt->follow_set[l];
                        parse_table[terminal_count * i + follow_tok] = prod;
                    }

                    break;
                }

                parse_table[terminal_count * i + tok] = prod;
            }

            free(tok_set);
            tok_set = NULL;
            first_seq_count = 0;
        }
    }
}

void print_parse_tree(){
    int count = 0;

    for(int i = 0; i < non_terminal_count; i++) {
        for(int j = 0; j < terminal_count; j++) {
            if(parse_table[terminal_count * i + j] == NULL)
                continue;
            
            production* prod = parse_table[terminal_count * i + j];
            printf("Stack symbol: %s, ", non_terminals[i]->name);
            printf("Next token: %s\n", tokenNameToString(j));
            printf("Production Rule: %s ===>", non_terminals[i]->name);
            for(int k = 0; k < prod->count; k++){
                if(prod->symbols[k]->type == SYM_NON_TERMINAL)
                    printf(" %s", prod->symbols[k]->value.name);
                else
                    printf(" %s", tokenNameToString(prod->symbols[k]->value.token_value));
            }
            printf("\n");
            count++;
        }

        printf("\n");
    }

    printf("Total number of entries: %d", count);
}
#endif

void generate_parse_map(){
    parse_map = map_create(200);
    for(int i = 0; i < non_terminal_count; i++){
        non_terminal* nt = non_terminals[i];
        for(int j = 0; j < nt->prod_count; j++){
            production* prod = nt->productions[j];
            int first_seq_count = 0;
            TokenName* tok_set = compute_first_of_sequence(prod->symbols, prod->count, &first_seq_count);
            for(int k = 0; k < first_seq_count; k++){
                TokenName tok = tok_set[k];
                if(tok == EPSILON){
                    for(int l = 0; l < nt->follow_size; l++){
                        TokenName follow_tok = nt->follow_set[l];
                        map_insert(parse_map, terminal_count * i + follow_tok, prod);
                    }
                    break;
                }
                
                map_insert(parse_map, terminal_count * i + tok, prod);
            }

            free(tok_set);
            tok_set = NULL;
            first_seq_count = 0;
        }
    }
}

void print_parse_map(){
    int count = 0;

    for(int i = 0; i < non_terminal_count; i++) {
        for(int j = 0; j < terminal_count; j++) {
            if(map_fetch(parse_map, terminal_count * i + j) == NULL)
                continue;
            
            production* prod = map_fetch(parse_map, terminal_count * i + j);
            printf("Stack symbol: %s, ", non_terminals[i]->name);
            printf("Next token: %s\n", tokenNameToString(j));
            printf("Production Rule: %s ===>", non_terminals[i]->name);
            for(int k = 0; k < prod->count; k++){
                if(prod->symbols[k]->type == SYM_NON_TERMINAL)
                    printf(" %s", prod->symbols[k]->value.name);
                else
                    printf(" %s", tokenNameToString(prod->symbols[k]->value.token_value));
            }
            printf("\n");
            count++;
        }

        printf("\n");
    }

    printf("Total number of entries: %d\n", count);
    printf("Total number of collisions: %d\n", get_collision_count());

}