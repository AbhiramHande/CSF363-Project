#include "parser.h"

#include <time.h>

#define MAX_LINE_LENGTH 256
#define SUPER_SYN_SIZE 9

//TODO
static bool error_present = false;

non_terminal** non_terminals = NULL;
int non_terminal_count = 0;
non_terminal* start_symbol = NULL;
production** parse_table = NULL;


void first_and_follow_cleanup(void);
node* create_tree_node_nonterm(non_terminal* nt);
node* create_tree_node_term(token_type tok_name);

node* parse_code(char* grammar_file, FILE* src_code) {
    atexit(first_and_follow_cleanup);

    FILE* file = fopen(grammar_file, "r");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, MAX_LINE_LENGTH, file)) {

        // Parse the production rule to remove the arrow
        char* arrow = strstr(line, "===>");
        if (!arrow) continue;
        *arrow = '\0';
        char* lhs = line;

        // Parse the rule to remove leading and trailing spaces
        while (isspace(*lhs)) 
            lhs++;
        char* end = lhs + strlen(lhs) - 1;
        while (end > lhs && isspace(*end)) 
            end--;
        *(end + 1) = '\0';

        // Check the LHS for non-terminal
        if (*lhs != '<' || lhs[strlen(lhs)-1] != '>') {
            fprintf(stderr, "Invalid non-terminal: %s\n", lhs);
            continue;
        }

        int str_len = strlen(lhs) - 2;
        non_terminal* nt = find_non_terminal(lhs + 1, str_len);
        if (!nt) {
            nt = (non_terminal*)malloc(sizeof(non_terminal));
            nt->name = strndup(lhs + 1, str_len);
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

        // Add start symbol if there is none
        if (!start_symbol)
            start_symbol = nt;
    }

    rewind(file);
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

        int str_len = strlen(lhs) - 2;
        non_terminal* nt = find_non_terminal(lhs + 1, str_len);

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
                int tok_str_len = strlen(token);
                symbol* sym = (symbol*)malloc(sizeof(symbol));
                if (strcmp(token, "EPS") == 0) {
                    sym->type = SYM_EPSILON;
                    sym->value.token_value = EPSILON;
                } 
                else if (token[0] == '<' && token[tok_str_len -1] == '>') {
                    sym->type = SYM_NON_TERMINAL;
                    sym->value.nt = find_non_terminal(token + 1, tok_str_len - 2);
                } 
                else if (strncmp(token, "TK_", 3) == 0) {
                    sym->type = SYM_TERMINAL;
                    sym->value.token_value = string_to_token(token);
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
    file = NULL;

    for(int i = 0; i < non_terminal_count; i++)
        compute_first_set(non_terminals[i]);
    compute_follow_set(start_symbol, NULL, NULL, 0);
    generate_parse_map();

    node* root = generate_parse_tree(src_code);
    return root;
}

non_terminal* find_non_terminal(const char* name, int end) {
    for (int i = 0; i < non_terminal_count; i++) {
        if(end == 0){
            if (strcmp(non_terminals[i]->name, name) == 0)
                return non_terminals[i];
        }
        else{
            if(strlen(non_terminals[i]->name) == end && strncmp(non_terminals[i]->name, name, end) == 0)
                return non_terminals[i];
        }
    }
    return NULL;
}

void add_to_first_set(non_terminal* nt, const token_type element) {
    for (int i = 0; i < nt->first_size; i++)
        if (nt->first_set[i] == element)
            return;

    nt->first_set = (token_type*)realloc(nt->first_set, (nt->first_size + 1)*sizeof(token_type));
    nt->first_set[nt->first_size++] = element;
    if (element == EPSILON)
        nt->has_epsilon_in_first = true;

    return;
}

void add_to_follow_set(non_terminal *nt, const token_type element) {
    for (int i = 0; i < nt->follow_size; i++)
        if (nt->follow_set[i] == element)
            return;

    nt->follow_set = (token_type*)realloc(nt->follow_set, (nt->follow_size + 1)*sizeof(token_type));
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
                non_terminal* prod_nt = sym->value.nt;
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
            token_type* first_seq = compute_first_of_sequence(syms, syms_size, &first_seq_size);

            
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
                    compute_follow_set(sym->value.nt, nt, prod, j);
            }
        }
    }
    return;
}

bool contains_EPS(token_type* set, int size) {
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
                printf(" %s", token_to_string(nt->first_set[j]));
                if (j < nt->first_size - 1) 
                    printf(",");
            }
            printf(" }\n");
        }
        
        if(print_follow){
            printf("Follow(%s): {", nt->name);
            for (int j = 0; j < nt->follow_size; j++) {
                printf(" %s", token_to_string(nt->follow_set[j]));
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
    non_terminal_count = 0;

    free(non_terminals);
    free(parse_table);

    non_terminals = NULL;
    start_symbol = NULL;
    parse_table = NULL;

    return;
}

void add_to_set(token_type** set, int* size, const token_type element) {
    if(element == TK_ERROR)
        return;

    for (int i = 0; i < *size; i++)
        if ((*set)[i] == element)
            return;

    *set = (token_type*)realloc(*set, (*size + 1)*sizeof(token_type));
    (*set)[(*size)++] = element;
}

token_type* compute_first_of_sequence(symbol** sym_seq, int sym_seq_count, int* result_size) {
    token_type* result = NULL;
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
            non_terminal* sym_nt = sym->value.nt;
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
    parse_table = calloc(non_terminal_count * TERMINAL_COUNT, sizeof(production*));
    for(int i = 0; i < non_terminal_count; i++){
        non_terminal* nt = non_terminals[i];
        for(int j = 0; j < nt->prod_count; j++){
            production* prod = nt->productions[j];
            int first_seq_count = 0;
            token_type* tok_set = compute_first_of_sequence(prod->symbols, prod->count, &first_seq_count);
            for(int k = 0; k < first_seq_count; k++){
                token_type tok = tok_set[k];
                if(tok == EPSILON){
                    for(int l = 0; l < nt->follow_size; l++){
                        token_type follow_tok = nt->follow_set[l];
                        parse_table[TERMINAL_COUNT * i + follow_tok] = prod;
                    }

                    break;
                }

                parse_table[TERMINAL_COUNT * i + tok] = prod;
            }

            free(tok_set);
            tok_set = NULL;
            first_seq_count = 0;
        }
    }
}

void print_parse_table(){
    int count = 0;

    for(int i = 0; i < non_terminal_count; i++) {
        for(int j = 0; j < TERMINAL_COUNT; j++) {
            if(parse_table[TERMINAL_COUNT * i + j] == NULL)
                continue;
            
            production* prod = parse_table[TERMINAL_COUNT * i + j];
            printf("Stack symbol: %s, ", non_terminals[i]->name);
            printf("Next token: %s\n", token_to_string(j));
            printf("Production Rule: %s ===>", non_terminals[i]->name);
            for(int k = 0; k < prod->count; k++){
                if(prod->symbols[k]->type == SYM_NON_TERMINAL)
                    printf(" %s", prod->symbols[k]->value.name);
                else
                    printf(" %s", token_to_string(prod->symbols[k]->value.token_value));
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
    for(int i = 0; i < non_terminal_count; i++){
        non_terminal* nt = non_terminals[i];
        for(int j = 0; j < nt->prod_count; j++){
            production* prod = nt->productions[j];
            int first_seq_count = 0;
            token_type* tok_set = compute_first_of_sequence(prod->symbols, prod->count, &first_seq_count);
            for(int k = 0; k < first_seq_count; k++){
                token_type tok = tok_set[k];
                if(tok == EPSILON){
                    for(int l = 0; l < nt->follow_size; l++){
                        token_type follow_tok = nt->follow_set[l];
                        map_insert(TERMINAL_COUNT * i + follow_tok, prod);
                    }
                    break;
                }
                
                map_insert(TERMINAL_COUNT * i + tok, prod);
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
        for(int j = 0; j < TERMINAL_COUNT; j++) {
            if(map_fetch(TERMINAL_COUNT * i + j) == NULL)
                continue;
            
            production* prod = map_fetch(TERMINAL_COUNT * i + j);
            printf("Stack symbol: %s, ", non_terminals[i]->name);
            printf("Next token: %s\n", token_to_string(j));
            printf("Production Rule: %s ===>", non_terminals[i]->name);
            for(int k = 0; k < prod->count; k++){
                if(prod->symbols[k]->type == SYM_NON_TERMINAL)
                    printf(" %s", prod->symbols[k]->value.nt->name);
                else
                    printf(" %s", token_to_string(prod->symbols[k]->value.token_value));
            }
            printf("\n");
            count++;
        }

        printf("\n");
    }

    printf("Total number of entries: %d\n", count);
    printf("Total number of collisions: %d\n", get_collision_count());

}

node* create_tree_node_nonterm(non_terminal* nt){
    node* tree_node = calloc(1, sizeof(node));
    if(!tree_node)
        return NULL;

    tree_node->token_value = NULL;
    tree_node->children = NULL;
    tree_node->children_count = 0;

    symbol* sym = calloc(1, sizeof(symbol));
    if(!sym)
        return NULL;

    sym->type = SYM_NON_TERMINAL;
    sym->value.nt = nt;
    tree_node->stack_symbol = sym;

    return tree_node;
}

node* create_tree_node_term(token_type tok_name){
    node* tree_node = calloc(1, sizeof(node));
    if(!tree_node)
        return NULL;

    tree_node->token_value = NULL;
    tree_node->children = NULL;
    tree_node->children_count = 0;

    symbol* sym = calloc(1, sizeof(symbol));
    if(!sym)
        return NULL;

    if(tok_name == EPSILON)
        sym->type = SYM_EPSILON;
    else
        sym->type = SYM_TERMINAL;
    sym->value.token_value = tok_name;
    tree_node->stack_symbol = sym;

    return tree_node;
}

node* generate_parse_tree(FILE* src_code){
    // error_present = false;
    int times = 0;
    int terms_so_far = 0;
    stack* parser_stack = stack_create();
    node* start = create_tree_node_nonterm(start_symbol);
    stack_push(parser_stack, start);
    setbuf(stdout, NULL);
    token* tok = get_next_token(src_code);
    token_type super_syn[SUPER_SYN_SIZE] = {TK_SEM, TK_ELSE, TK_END, TK_ENDIF, TK_ENDRECORD, TK_ENDUNION, TK_ENDWHILE, TK_CL, TK_SQR};
    while(!stack_empty(parser_stack)){
        times++;
        if(tok->name == TK_EOF){
            fprintf(stderr, "WTF?!\n");
            exit(EXIT_FAILURE);
        }
        while(tok->name == TK_COMMENT) {
            times++;
            free(tok);
            tok = get_next_token(src_code);
            if(tok->name == TK_EOF){
                fprintf(stderr, "WTF?!\n");
                exit(EXIT_FAILURE);
            }
        }
        node* parser_stack_top = stack_top(parser_stack);

        if(parser_stack_top->stack_symbol->type == SYM_TERMINAL){
            terms_so_far++;
            if(parser_stack_top->stack_symbol->value.token_value == tok->name){
                node* term = stack_pop(parser_stack);
                term->token_value = tok;
                tok = get_next_token(src_code);
            }
            else{
                fprintf(stderr, "Line no. %-4d: \033[1;31mError:\033[0m The token %-15s for lexeme %s does not match with the expected token %s\n",
                    tok->line_num, token_to_string(tok->name), tok->lexeme, token_to_string(parser_stack_top->stack_symbol->value.token_value));
                //TODO: Error
                error_present = true;
                stack_pop(parser_stack);
                continue;
            }
        }
        else{
            //assert(parser_stack_top->stack_symbol->type == SYM_NON_TERMINAL);
            int non_term_pos = -1;
            for(int i = 0; i < non_terminal_count; i++){
                if(parser_stack_top->stack_symbol->value.nt == non_terminals[i])
                    non_term_pos = i;
            }
            if(non_term_pos == -1)
                fprintf(stderr, "Nonterm not found");

            int lookup_key = TERMINAL_COUNT * non_term_pos + tok->name; 
            production* rule = map_fetch(lookup_key);
            if(rule){
                node* old_top = stack_pop(parser_stack);
                old_top->children_count = rule->count;

                if(old_top->children_count > 0)
                    old_top->children = (node**)calloc(old_top->children_count, sizeof(node*));

                for(int i = rule->count - 1; i >= 0; i--){
                    node* new_node;

                    if(rule->symbols[i]->type == SYM_NON_TERMINAL)
                        new_node = create_tree_node_nonterm(rule->symbols[i]->value.nt);
                    else if(rule->symbols[i]->type == SYM_TERMINAL)
                        new_node = create_tree_node_term(rule->symbols[i]->value.token_value);
                    else
                        new_node = create_tree_node_term(EPSILON);
                    
                    old_top->children[i] = new_node;

                    if(new_node->stack_symbol->type != SYM_EPSILON)
                        if(!stack_push(parser_stack, new_node))
                            fprintf(stderr, "\033[1;31mError:\033[0m Stack overflow.\n");
                }
            }
            else{
                fprintf(stderr, "Line no. %-4d: \033[1;31mError:\033[0m Invalid token %-15s encountered with value %s stack top %s\n",
                    tok->line_num, token_to_string(tok->name), tok->lexeme, parser_stack_top->stack_symbol->value.nt->name);
                //TODO: Error
                error_present = true;
                token_type* syn = parser_stack_top->stack_symbol->value.nt->follow_set;
                int syn_count = parser_stack_top->stack_symbol->value.nt->follow_size;
                int i = 0;
                for(; i < syn_count; i++){
                    if(syn[i] == tok->name){
                        stack_pop(parser_stack);
                        break;
                    }
                }
                if (i == syn_count){
                    int j = 0;
                    for(; j < SUPER_SYN_SIZE; j++){
                        if(super_syn[j] == tok->name){
                            stack_pop(parser_stack);
                            break;
                        }
                    }
                    if(j == SUPER_SYN_SIZE)
                        tok = get_next_token(src_code);
                }
            }
        }
    }

    if(tok->name != TK_EOF){
        fprintf(stderr, "Error: Expected EOF but got %s\n", token_to_string(tok->name));
        error_present = true;
    }
    free(tok);
    stack_cleanup(&parser_stack);
    return start;
}
void print_parse_tree_inorder(node* root, node* parent, FILE* output_file){
    if(!error_present){
        if(root -> stack_symbol -> type == SYM_TERMINAL){
            
            if(parent != NULL){
                if(root -> token_value -> name == TK_NUM) 
                    fprintf(output_file, "%-30s  | %-25s  | %-8d  | %-25s  | %-11lld  | %-25s  | yes          | %-25s\n", 
                        root->token_value->lexeme, 
                        token_to_string(root->token_value->name), 
                        root -> token_value -> line_num , 
                        token_to_string(root->token_value->name), 
                        root -> token_value ->value.num, 
                        parent->stack_symbol->value.nt->name, 
                        token_to_string(root->token_value->name));
                else if(root -> token_value -> name == TK_RNUM) 
                    fprintf(output_file, "%-30s  | %-25s  | %-8d  | %-25s  | %-11Lf  | %-25s  | yes          | %-25s\n",
                        root->token_value->lexeme, 
                        token_to_string(root->token_value->name), 
                        root -> token_value -> line_num , 
                        token_to_string(root->token_value->name), 
                        root -> token_value ->value.r_num, 
                        parent->stack_symbol->value.nt->name, 
                        token_to_string(root->token_value->name));
                else 
                    fprintf(output_file, "%-30s  | %-25s  | %-8d  | %-25s  | Not a number | %-25s  | yes          | %-25s\n",
                        root->token_value->lexeme, 
                        token_to_string(root->token_value->name), 
                        root -> token_value -> line_num , 
                        token_to_string(root->token_value->name), 
                        parent->stack_symbol->value.nt->name, 
                        token_to_string(root->token_value->name));
                
            }
            else{
                if(root -> token_value -> name == TK_NUM) 
                    fprintf(output_file, "%-30s  | %-25s  | %-8d  | %-25s  | %-11lld  | NULL                       | yes          | %-25s\n",
                        root->token_value->lexeme, 
                        token_to_string(root->token_value->name), 
                        root -> token_value -> line_num , 
                        token_to_string(root->token_value->name), 
                        root -> token_value ->value.num, 
                        token_to_string(root->token_value->name));
                else if(root -> token_value -> name == TK_RNUM) 
                    fprintf(output_file, "%-30s  | %-25s  | %-8d  | %-25s  | %-11Lf  | NULL                       | yes          | %-25s\n",
                        root->token_value->lexeme, 
                        token_to_string(root->token_value->name), 
                        root -> token_value -> line_num , 
                        token_to_string(root->token_value->name), 
                        root -> token_value ->value.r_num, 
                        token_to_string(root->token_value->name));
                else 
                    fprintf(output_file, "%-30s  | %-25s  | %-8d  | %-25s  | Not a number | NULL                       | yes          | %-25s\n",
                        root->token_value->lexeme, 
                        token_to_string(root->token_value->name), 
                        root -> token_value -> line_num , 
                        token_to_string(root->token_value->name), 
                        token_to_string(root->token_value->name));
            }
            return;  
        }
        else if(root -> stack_symbol -> type == SYM_NON_TERMINAL){
            // Go Left
            if(root->children[0]->stack_symbol->type != SYM_EPSILON) print_parse_tree_inorder(root -> children[0], root, output_file);
            
            // Print
            if(parent != NULL){
                fprintf(output_file, "           ----------           | %-25s  | Not Def.  |          ----------        | Not a number | %-25s  | yes          | %-25s\n",
                    root->stack_symbol->value.nt->name, 
                    parent->stack_symbol->value.nt->name, 
                    root->stack_symbol->value.nt->name);
            }
            else{
                fprintf(output_file, "           ----------           | %-25s  | Not Def.  |          ----------        | Not a number | NULL                        | yes          | %-25s\n",
                    root->stack_symbol->value.nt->name, 
                    root->stack_symbol->value.nt->name);
            }
            // Go Right
            for(int i = 1 ; i < root -> children_count ; i++){
                if(root->children[i]->stack_symbol->type != SYM_EPSILON) print_parse_tree_inorder(root -> children[i], root, output_file);
            }
        }
        else 
            fprintf(output_file, "           ----------           | EPSILON                  | Not Def.  |          ----------        | Not a number | %-25s  | yes          | \n",
                parent->stack_symbol->value.nt->name);   
    }
    else printf("Errors Present. Parse tree not printed\n");
    return;
}

void print_parse_tree(node* root){
    if(!error_present){
        if(root->stack_symbol->type == SYM_TERMINAL){
            printf("Parse Tree Leaf: %s, Leaf lexeme: %s\n\n", token_to_string(root->token_value->name), root->token_value->lexeme);
            return;
        }
        else if(root->stack_symbol->type == SYM_NON_TERMINAL){
            printf("Parse Tree Node: %s, Number of Children: %d\n", root->stack_symbol->value.nt->name, root->children_count);
            for(int i = 0; i < root->children_count; i++){
                if(root->children[i]->stack_symbol->type == SYM_TERMINAL)
                    printf("%s \t", token_to_string(root->children[i]->token_value->name));
                else if(root->children[i]->stack_symbol->type == SYM_NON_TERMINAL)
                    printf("%s \t", root->children[i]->stack_symbol->value.nt->name);
                else
                    printf("EPSILON \t");
            }
            printf("\n\n");

            for(int i = 0; i < root->children_count; i++)
                if(root->children[i]->stack_symbol->type != SYM_EPSILON)
                    print_parse_tree(root->children[i]);
        }
    }
    return;
}

void parse_tree_cleanup(node** root){
    if(!root || !*root)
        return;

    for(int i = 0; i < (*root)->children_count; i++)
        parse_tree_cleanup(&((*root)->children[i]));
    
    free((*root)->children);
    free((*root)->stack_symbol);
    free((*root)->token_value);

    (*root)->children = NULL;
    (*root)->stack_symbol = NULL;
    (*root)->token_value = NULL;
    (*root)->children_count = 0;

    free(*root);
    *root = NULL;
    return;
}