#include "../../include/parser.h"

/**
 * @brief The assumed maximum length of each line in the grammar file.
 */
#define MAX_LINE_LENGTH 256

/**
 * @brief The size of the "super" synchronizing set.
 * 
 * @details Certain low-level constructs, such as `TK_SEM` (semicolon) and `TK_CL` (closing brackets),
 *          are included in a special set called the "super" synchronizing set, which helps in error recovery.
 * 
 * @warning Whenever modifications are made to the "super" synchronizing set, this macro must be updated accordingly.
 */
#define SUPER_SYN_SIZE 9

/**
 * @brief A global flag indicating the presence of errors in the source code.
 */
static bool error_present = false;

/**
 * @brief An array of pointers to the various non-terminals in the grammar.
 * 
 * @note This array stores all non-terminals for reference in parsing.
 */
non_terminal** non_terminals = NULL;

/**
 * @brief The number of non-terminals stored in `non_terminals`.
 */
int non_terminal_count = 0;

/**
 * @brief A pointer to the start symbol of the grammar.
 * 
 * @note The start symbol is assumed to be the first non-terminal defined in the grammar.
 */
non_terminal* start_symbol = NULL;

/**
 * @brief A 2D parsing table for an LL(1) predictive parser.
 * 
 * @note The table is indexed by the top of the parsing stack (a non-terminal) and the current input token (a terminal).
 * 
 * @deprecated This 2D array implementation has been replaced with a HashMap for improved memory efficiency.
 */
production** parse_table = NULL;


/*****************************************************************************
 *                                                                           *
 *                         PARSER INTERNAL FUNCTIONS                         *
 *                                                                           *
 *****************************************************************************/

/**
 * @ingroup Parser_Internal
 * 
 * @brief Computes the First set of a given non-terminal.
 * 
 * @details 
 * The First set of a non-terminal @f$ X @f$ is computed as follows:
 * 1. If @f$ X \to Y_1Y_2 \dots Y_k @f$ is a production rule for some @f$ k \geq 1 @f$:
 *      - If @f$ \alpha \neq \epsilon \in First(Y_1) @f$, then add @f$ \alpha @f$ to First(X).
 *      - If @f$ \epsilon \in First(Y_1) @f$, continue checking @f$ Y_2, Y_3, \dots, Y_n @f$.
 *      - Stop when a symbol @f$ Y_n @f$ is found where @f$ \epsilon \notin First(Y_n) @f$ or all @f$ Y_i @f$ are exhausted.
 *      - If @f$ \epsilon \in First(Y_i) @f$ for all @f$ i \leq n @f$, add @f$ \epsilon @f$ to First(X).
 * 2. If @f$ X \to \epsilon @f$ is a production, then @f$ \epsilon @f$ is added to First(X).
 * 3. If a non-terminal has multiple production rules, the First set is the **union** of the First sets of all right-hand sides.
 * 
 * @param nt A pointer to the non-terminal whose First set is to be computed.
 * 
 * @returns void
 * 
 * @warning Modifies the global non-terminal structure by adding the First set and its size.
 *          If @f$ \epsilon @f$ is present in First, this is also reflected in the structure.
 */
void compute_first_set(non_terminal* nt);

/**
 * @ingroup Parser_Internal
 * 
 * @brief Computes the Follow set of a given non-terminal.
 * 
 * @details 
 * The Follow set of a non-terminal @f$ X @f$ is computed as follows:
 * 1. If @f$ X @f$ is the start symbol (in the given grammar, `<program>`), then add `$` to Follow(X).
 * 2. If there is a production rule @f$ A \to \alpha B \beta @f$:
 *      - Add everything in `First(β) - {ε}` to `Follow(B)`.
 *      - If @f$ \epsilon \in First(\beta) @f$, then add `Follow(A)` to `Follow(B)`.
 * 3. If there is a production rule @f$ A \to \alpha B @f$, then add `Follow(A)` to `Follow(B)`.
 * 
 * @param nt A pointer to the non-terminal whose Follow set is to be computed.
 * @param A A pointer to the parent non-terminal from which Follow was called (NULL for the start symbol).
 * @param aToAlpha A pointer to the production rule in which `nt` appears.
 * @param nt_pos The position of `nt` in the production rule.
 * 
 * @returns void
 * 
 * @warning Modifies the global non-terminal structure by adding the Follow set and its size.
 */
void compute_follow_set(non_terminal* nt, non_terminal* A, production* aToAlpha, int nt_pos);

/**
 * @ingroup Parser_Internal
 * 
 * @brief Generates the parsing table for the given grammar using a hash map.
 * 
 * @details 
 * The parsing table is constructed using the following rules:
 * 1. For each production @f$ A \to \alpha @f$:
 *      - For every terminal @f$ a \in First(\alpha) @f$, add @f$ A \to \alpha @f$ to @f$ M[A, a] @f$.
 *      - If @f$ \epsilon \in First(\alpha) @f$, then for every terminal @f$ b \in Follow(A) @f$, add @f$ A \to \alpha @f$ to @f$ M[A, b] @f$.
 * 
 * @returns void
 */
void generate_parse_map();

/**
 * @ingroup Parser_Internal
 * 
 * @brief Generates a parse tree for the given source code.
 * 
 * @note Uses LL(1) parsing process
 * 
 * @todo update details --Ankur
 * @details 
 * The LL(1) parsing process follows these steps:
 * 1. Push the start symbol of the grammar (`<program>`) onto an empty stack.
 * 2. Set `tp` to the first symbol of `w`, the tokenized input string.
 * 3. Use the parsing table `M` to decide parsing actions.
 * 4. While (stack is not empty)
 *      - If top of stack matches current token, pop stack and advance `tp`.
 *      - If top of the stack is a terminal but does not match current token, pop the stack.
 *      - If `M`'s entry corresponding to the stack top and current token is `NULL`, **handle error recovery** (panic-mode).
 *      - If @f$ M[X, a] = X \to Y_1Y_2 \cdots Y_k @f$:
 *          - Output the production @f$ X \to Y_1Y_2 \cdots Y_k @f$.
 *          - Pop `X` from stack.
 *          - Push @f$ Y_k, Y_{k-1}, \cdots, Y_1 @f$ onto the stack in that order.
 * 5. Repeat until the stack is empty.
 * 
 * @param src_code A pointer to an opened file containing the source code.
 * 
 * @return A pointer to the root node of the parse tree.
 * 
 * @warning The caller must deallocate the parse tree after use.
 */
node* generate_parse_tree(FILE* src_code);


/*****************************************************************************
 *                                                                           *
 *                         HELPER INTERNAL FUNCTIONS                         *
 *                                                                           *
 *****************************************************************************/

/**
 * @ingroup Parser_Internal_Helper
 * 
 * @brief Adds a given token type to a set if it does not already exist.
 * 
 * @param set A pointer to a dynamically allocated array representing the set.
 * @param size A pointer to the current size of the set.
 * @param element The token type to be added.
 * 
 * @note If the given token type is not present in the set, the set is resized 
 *       and the token is added. The size is updated accordingly.
 */
void add_to_set(token_type** set, int* size, const token_type element);

/**
 * @ingroup Parser_Internal_Helper
 * 
 * @brief Adds a given token to the first set of a non-terminal.
 * 
 * @param nt A pointer to the non-terminal whose first set is to be updated.
 * @param element The token type to be added to the first set.
 */
void add_to_first_set(non_terminal* nt, const token_type element);

/**
 * @ingroup Parser_Internal_Helper
 * 
 * @brief Adds a given token to the follow set of a non-terminal.
 * 
 * @param nt A pointer to the non-terminal whose follow set is to be updated.
 * @param element The token type to be added to the follow set.
 */
void add_to_follow_set(non_terminal* nt, const token_type element);

/**
 * @ingroup Parser_Internal_Helper
 * 
 * @brief Computes the first set of a sequence of symbols (terminals and non-terminals).
 * 
 * @param sym_seq A pointer to an array of symbols representing the sequence.
 * @param sym_seq_count The number of symbols in the sequence.
 * @param result_size A pointer to an integer that will store the size of the resulting first set.
 * 
 * @returns A pointer to a dynamically allocated array containing the first set of the given sequence.
 * 
 * @warning The caller is responsible for freeing the returned array.
 */
token_type* compute_first_of_sequence(symbol** sym_seq, int sym_seq_count, int* result_size);

/**
 * @ingroup Parser_Internal_Helper
 * 
 * @brief Checks whether the `TK_EPSILON` token is present in a given set.
 * 
 * @param set A pointer to an array of token types.
 * @param size The number of elements in the set.
 * 
 * @returns `true` if the set contains `TK_EPSILON`, otherwise `false`.
 */
bool contains_EPS(token_type* set, int size);

/**
 * @ingroup Parser_Internal_Helper
 * 
 * @brief Finds a non-terminal based on its name.
 * 
 * @param name A string representing the name of the non-terminal.
 * @param end The number of characters to compare (if zero, the entire string is compared).
 * 
 * @returns A pointer to the non-terminal if found, otherwise `NULL`.
 */
non_terminal* find_non_terminal(const char* name, int end);

/**
 * @ingroup Parser_Internal_Helper
 * 
 * @brief Creates a new parse tree node corresponding to a given non-terminal.
 * 
 * @param nt A pointer to the non-terminal.
 * 
 * @returns A pointer to the newly created tree node.
 * 
 * @warning The caller is responsible for freeing the allocated node.
 */
node* create_tree_node_nonterm(non_terminal* nt);

/**
 * @ingroup Parser_Internal_Helper
 * 
 * @brief Creates a new parse tree node corresponding to a given terminal.
 * 
 * @param tok_name The token type representing the terminal.
 * 
 * @returns A pointer to the newly created tree node.
 * 
 * @warning The caller is responsible for freeing the allocated node.
 */
node* create_tree_node_term(token_type tok_name);


/*****************************************************************************
 *                                                                           *
 *               INTERNAL FUNCTIONS FOR PRINTING INTERMEDIATES               *
 *                                                                           *
 *****************************************************************************/

/**
 * @ingroup Parser_Internal_Printing
 * 
 * @brief Prints the first and/or follow sets of all non-terminals.
 * 
 * @param print_first If `true`, prints the first sets of each non-terminal.
 * @param print_follow If `true`, prints the follow sets of each non-terminal.
 */
void print_first_and_follow_sets(bool print_first, bool print_follow);

/**
 * @ingroup Parser_Internal_Printing
 * 
 * @brief Prints the parse tree using pre-order traversal.
 * 
 * @param root A pointer to the root of the parse tree.
 */
void print_parse_tree(node* root);

/**
 * @ingroup Parser_Internal_Printing
 * 
 * @brief Prints the parsing table.
 */
void print_parse_map();


/*****************************************************************************
 *                                                                           *
 *                     CONSTRUCTORS AND DESTRUCTORS                          *
 *                                                                           *
 *****************************************************************************/

/**
 * @ingroup Parser_Internal_Cleanup
 * 
 * @brief Deallocates memory associated with all non-terminals, their first and follow sets, and the parsing table.
 */
void first_and_follow_cleanup(void);

#ifdef NO_HASHMAP
void generate_parse_table();
void print_parse_table();
#endif

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
        fprintf(stderr, "\033[1;31mError:\033[0m Expected EOF but got %s\n", token_to_string(tok->name));
        error_present = true;
    }
    free(tok);
    stack_cleanup(&parser_stack);
    return start;
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

/*****************************************************************************
 *                                                                           *
 *                  FUNCTIONS DEFINED IN THE HEADER FILE                     *
 *                                                                           *
 *****************************************************************************/

 node* parse_code(char* grammar_file, FILE* src_code) {
    atexit(first_and_follow_cleanup);

    FILE* file = fopen(grammar_file, "r");
    if (!file) {
        perror("Error opening grammar file");
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

void print_parse_tree_inorder(node* root, node* parent, FILE* output_file){
    if(!error_present){
        if(root -> stack_symbol -> type == SYM_TERMINAL){
            
            if(parent != NULL){
                if(root -> token_value -> name == TK_NUM) 
                    fprintf(output_file, "| %-30s  | %-25s  | %-8d  | %-25s  | %-11lld  | %-25s  | yes          | %-25s |\n", 
                        root->token_value->lexeme, 
                        token_to_string(root->token_value->name), 
                        root -> token_value -> line_num , 
                        token_to_string(root->token_value->name), 
                        root -> token_value ->value.num, 
                        parent->stack_symbol->value.nt->name, 
                        token_to_string(root->token_value->name));
                else if(root -> token_value -> name == TK_RNUM) 
                    fprintf(output_file, "| %-30s  | %-25s  | %-8d  | %-25s  | %-11Lf  | %-25s  | yes          | %-25s |\n",
                        root->token_value->lexeme, 
                        token_to_string(root->token_value->name), 
                        root -> token_value -> line_num , 
                        token_to_string(root->token_value->name), 
                        root -> token_value ->value.r_num, 
                        parent->stack_symbol->value.nt->name, 
                        token_to_string(root->token_value->name));
                else 
                    fprintf(output_file, "| %-30s  | %-25s  | %-8d  | %-25s  | Not a number | %-25s  | yes          | %-25s |\n",
                        root->token_value->lexeme, 
                        token_to_string(root->token_value->name), 
                        root -> token_value -> line_num , 
                        token_to_string(root->token_value->name), 
                        parent->stack_symbol->value.nt->name, 
                        token_to_string(root->token_value->name));
                
            }
            else{
                if(root -> token_value -> name == TK_NUM) 
                    fprintf(output_file, "| %-30s  | %-25s  | %-8d  | %-25s  | %-11lld  | NULL                       | yes          | %-25s |\n",
                        root->token_value->lexeme, 
                        token_to_string(root->token_value->name), 
                        root -> token_value -> line_num , 
                        token_to_string(root->token_value->name), 
                        root -> token_value ->value.num, 
                        token_to_string(root->token_value->name));
                else if(root -> token_value -> name == TK_RNUM) 
                    fprintf(output_file, "| %-30s  | %-25s  | %-8d  | %-25s  | %-11Lf  | NULL                       | yes          | %-25s |\n",
                        root->token_value->lexeme, 
                        token_to_string(root->token_value->name), 
                        root -> token_value -> line_num , 
                        token_to_string(root->token_value->name), 
                        root -> token_value ->value.r_num, 
                        token_to_string(root->token_value->name));
                else 
                    fprintf(output_file, "| %-30s  | %-25s  | %-8d  | %-25s  | Not a number | NULL                       | yes          | %-25s |\n",
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
                fprintf(output_file, "|            ----------           | %-25s  | Not Def.  |          ----------        | Not a number | %-25s  | no           | %-25s |\n",
                    root->stack_symbol->value.nt->name, 
                    parent->stack_symbol->value.nt->name, 
                    root->stack_symbol->value.nt->name);
            }
            else{
                fprintf(output_file, "|            ----------           | %-25s  | Not Def.  |          ----------        | Not a number | NULL                       | no           | %-25s |\n",
                    root->stack_symbol->value.nt->name, 
                    root->stack_symbol->value.nt->name);
            }
            // Go Right
            for(int i = 1 ; i < root -> children_count ; i++){
                if(root->children[i]->stack_symbol->type != SYM_EPSILON) print_parse_tree_inorder(root -> children[i], root, output_file);
            }
        }
        else 
            fprintf(output_file, "|            ----------           | EPSILON                  | Not Def.  |          ----------        | Not a number | %-25s  | yes          |                           |\n",
                parent->stack_symbol->value.nt->name);   
    }
    else printf("Errors Present. Empty Parse tree printed\n");
    return;
}