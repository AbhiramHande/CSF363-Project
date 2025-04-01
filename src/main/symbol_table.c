#include "../../include/symbol_table.h"

/**
 * @brief Defines the initial capacity of the symbol table.
 * 
 * @note The symbol table starts with this size and dynamically resizes if needed.
 */
#define INITIAL_TABLE_SIZE 1024


/**
 * @brief Data structure representing a symbol table.
 * 
 * @details 
 * - The symbol table is implemented as a **hash map** for efficient storage 
 *   and retrieval of symbols (identifiers, keywords, etc.).
 * - Symbols are indexed by their **lexeme** and mapped to their respective **token type**.
 * - Uses **open addressing with linear probing** for collision resolution.
 * 
 * @note
 * - The symbol table is dynamically allocated and must be initialized using `symbol_table_create`.
 * - It supports **automatic resizing** when the hash map is full.
 * - Deallocated using `symbol_table_cleanup` to prevent memory leaks.
 * 
 * @warning
 * - Lexemes must be **null-terminated strings**.
 * 
 * @attention
 * - **Lookup Complexity:** Average **@f$ \mathcal{O}(1) @f$**, Worst-case **@f$ \mathcal{O}(n) @f$** (if hash function performs poorly).
 * - **Insertion Complexity:** Average **O(1)**, but rehashing and/or resizing can be expensive.
 */
typedef struct SymbolTable symbol_table;

/**
 * @brief Represents a node in the symbol table, storing an individual entry.
 * 
 * @details 
 * - Each node contains an entry (a pair of lexeme and token type) and a flag indicating its validity.
 * - Used within the symbol table's hash map for storage and retrieval.
 */
typedef struct SymbolTableNode symbol_table_node;

struct SymbolTable {
    symbol_table_node* entries; /**< Pointer to an array of symbol table entries. */
    int capacity; /**< Maximum capacity of the symbol table (resizable). */
    int occupancy; /**< Current number of occupied slots in the symbol table. */
};

/**
 * @struct SymbolTableNode
 * @brief Structure defining a single node in the symbol table.
 */
struct SymbolTableNode {
    entry* value; /**< Pointer to an entry in the symbol table. */
    bool is_occupied; /**< Indicates whether the entry is valid or uninitialized. */
};


/*****************************************************************************
 *                                                                           *
 *                      GLOBAL VARIABLES IN THIS FILE                        *
 *                                                                           *
 *****************************************************************************/

/**
 * @brief Tracks the number of collisions encountered in the symbol table.
 * 
 * @note 
 * - This value is primarily used for debugging and performance analysis of the hashing mechanism.
 * - It is automatically reset to zero when `symbol_table_create` is called.
 */
static int collision_count = 0;

/**
 * @brief A pointer to the global symbol table.
 * 
 * @details The symbol table is implemented as a HashMap for efficient storage 
 *          and retrieval of identifiers, keywords, and other symbols used in parsing.
 * 
 * @note 
 * - Defined as a static global variable, making it inaccessible outside this file.
 * - Ensures a singleton-like behavior, preventing multiple instances.
 * - Can only be accessed and modified via `symbol_table_insert` and `symbol_table_fetch`.
 * - Effectively enforces encapsulation.
 */
static symbol_table* table = NULL;


/*****************************************************************************
 *                                                                           *
 *                     FUNCTIONS INTERNAL TO THE FILE                        *
 *                                                                           *
 *****************************************************************************/

 /**
 * @ingroup Symbol table internal
 * 
 * @brief Initializes the symbol table (automatically called on startup, uses a hash map).
 * 
 * @pre `table` should be uninitialized or `NULL`.
 *
 * @post `table` is set to a newly created symbol table.
 * @post `collision_count` is reset to `0`.
 * 
 * @note Adds all keywords to the symbol table.
 * 
 * @warning Modifies the global variables `table` and `collision_count`. 
 */
static void symbol_table_create(void) __attribute__((constructor));

/**
 * @ingroup Symbol table internal
 * 
 * @brief Deallocates the symbol table (automatically called at program termination).
 * 
 * @post `table` and all associated memory are freed and set to `NULL`.
 * 
 * @details Frees the memory allocated for the hash table, its entries, and stored lexemes.
 *          The caller must **NOT** free any values (pointers) inserted or retrieved from the symbol table.
 */
static void symbol_table_cleanup(void) __attribute__((destructor));

/**
 * @ingroup Symbol table internal
 * 
 * @brief Computes a hash value for a given key.
 * 
 * @details Implements the **djb2** hash function:
 *          - Starts with an initial hash value of `5381`.
 *          - Iterates through each character in the key:
 *              - Multiplies the current hash by `33`.
 *              - Adds the ASCII value of the character.
 *          - Continues until the end of the string.
 * 
 * @param key The string key for which the hash is computed.
 * 
 * @return The computed hash as an `unsigned long`.
 */
static unsigned long hash_function(const char* key);

/**
 * @ingroup Symbol table internal
 * 
 * @brief Determines whether a given number is prime in \(O(\sqrt{n})\) time complexity.
 * 
 * @param n The number to check for primality.
 * 
 * @return `true` if the number is prime, otherwise `false`.
 */
static bool is_prime(unsigned int n);

/**
 * @ingroup Symbol table internal
 * 
 * @brief Finds the next prime number greater than a given value.
 * 
 * @param n The number after which the next prime is sought.
 * 
 * @return The next prime number as an `int`.
 */
static int next_prime(int n);

/**
 * @ingroup Symbol table internal
 * 
 * @brief Inserts an existing entry into a newly created symbol table.
 * 
 * @note This function is used internally to transfer entries during resizing.
 * 
 * @param _entry A pointer to an entry in the old symbol table. 
 * 
 * @warning Modifies the global variables `table` and `collision_count`.
 */
static void symbol_table_insert_entry(entry* _entry);

/**
 * @ingroup Symbol table internal
 * 
 * @brief Resizes the symbol table when it reaches capacity.
 * 
 * @pre `table` must point to the current symbol table.
 * @post `table` will point to the newly allocated larger symbol table.
 * 
 * @note This function is automatically triggered when the table becomes full.
 * 
 * @warning Modifies the global variables `table` and `collision_count`.
 */
static void symbol_table_resize(void);

static unsigned long hash_function(const char* key) {
    unsigned long hash;
    for(hash = 5381; *key != '\0'; key++)
        hash = ((hash << 5) + hash) + *key;
    return hash;
}

static bool is_prime(unsigned int n) {
    if (n <= 1) 
        return false;
    if (n <= 3) 
        return true;
    if (n % 2 == 0 || n % 3 == 0) 
        return false;
    for (size_t i = 5; i * i <= n; i += 6)
        if (n % i == 0 || n % (i + 2) == 0) 
            return false;

    return true;
}

static int next_prime(int n) {
    while (!is_prime(n)) 
        n++;
    return n;
}

static void symbol_table_create(void) {
    symbol_table* map = malloc(sizeof(struct SymbolTable));
    if (!map){
        perror("Unable to create hash map for symbol table");
        exit(EXIT_FAILURE);
    }
        
    map->capacity = next_prime(INITIAL_TABLE_SIZE);
    map->entries = calloc(map->capacity, sizeof(struct SymbolTableNode));
    if (!map){
        free(map);
        perror("Unable to create hash map for parse table");
        exit(EXIT_FAILURE);
    }

    map->occupancy = 0;
    collision_count = 0;
    table = map;

    symbol_table_insert("with", TK_WITH);
    symbol_table_insert("parameters", TK_PARAMETERS);
    symbol_table_insert("end", TK_END);
    symbol_table_insert("while", TK_WHILE);
    symbol_table_insert("union", TK_UNION);
    symbol_table_insert("endunion", TK_ENDUNION);
    symbol_table_insert("definetype", TK_DEFINETYPE);
    symbol_table_insert("as", TK_AS);
    symbol_table_insert("type", TK_TYPE);
    symbol_table_insert("main", TK_MAIN);
    symbol_table_insert("global", TK_GLOBAL);
    symbol_table_insert("parameter", TK_PARAMETER);
    symbol_table_insert("list", TK_LIST);
    symbol_table_insert("input", TK_INPUT);
    symbol_table_insert("output", TK_OUTPUT);
    symbol_table_insert("int", TK_INT);
    symbol_table_insert("real", TK_REAL);
    symbol_table_insert("endwhile", TK_ENDWHILE);
    symbol_table_insert("if", TK_IF);
    symbol_table_insert("then", TK_THEN);
    symbol_table_insert("endif", TK_ENDIF);
    symbol_table_insert("read", TK_READ);
    symbol_table_insert("write", TK_WRITE);
    symbol_table_insert("return", TK_RETURN);
    symbol_table_insert("call", TK_CALL);
    symbol_table_insert("record", TK_RECORD);
    symbol_table_insert("endrecord", TK_ENDRECORD);
    symbol_table_insert("else", TK_ELSE);
    
    symbol_table_insert("_main", TK_MAIN);

    return;
}

static void symbol_table_cleanup(void) {
    if (!table) return;

    for(int i = 0; i < table->capacity; i++){
        if(table->entries[i].is_occupied){
            free(table->entries[i].value->name);
            table->entries[i].value->name = NULL;
            free(table->entries[i].value);
            table->entries[i].value = NULL;
        }
    }

    free(table->entries);
    table->entries = NULL;

    free(table);
    table = NULL;
    return;
}

static void symbol_table_insert_entry(entry* _entry){
    if (!table) 
        return;

    unsigned long hash = hash_function(_entry->name);
    int capacity = table->capacity;

    for (int i = 0; i < capacity; i++) {
        int index = (hash + i) % capacity;
        symbol_table_node node = table->entries[index];

        if (!node.is_occupied) {
            table->entries[index].value = _entry;
            table->entries[index].is_occupied = true;
            table->occupancy++;
            return;
        }
        else{
            collision_count++;
        }
    }
}

static void symbol_table_resize(void) {   
    int capacity = next_prime(table->capacity << 2);

    symbol_table_node* entries = calloc(capacity, sizeof(struct SymbolTableNode));
    if (!entries){
        perror("Unable to resize hash map for symbol table");
        exit(EXIT_FAILURE);
    }

    symbol_table* old_table = table;
    table = calloc(1, sizeof(struct SymbolTable));
    table->capacity = capacity;
    table->entries = entries;
    table->occupancy = 0;
    
    for(int i = old_table->capacity - 1; i >= 0; i--){
        if(old_table->entries[i].is_occupied)
            symbol_table_insert_entry(old_table->entries[i].value);
    }

    free(old_table->entries);
    old_table->entries = NULL;

    free(old_table);
    old_table = NULL;

    return;
}

/*****************************************************************************
 *                                                                           *
 *                  FUNCTIONS DEFINED IN THE HEADER FILE                     *
 *                                                                           *
 *****************************************************************************/

entry* symbol_table_insert(const char* lexeme, token_type type) {
    if (!table) 
        return NULL;

    unsigned long hash = hash_function(lexeme);
    int capacity = table->capacity;
    if(capacity == table->occupancy){
        symbol_table_resize();
        capacity = table->capacity;
    }
    
    for (int i = 0; i < capacity; i++) {
        int index = (hash + i) % capacity;
        symbol_table_node node = table->entries[index];

        if (!node.is_occupied) {
            entry* _entry = calloc(1, sizeof(entry));
            _entry->name = strdup(lexeme);
            _entry->type = type;
            table->entries[index].is_occupied = true;
            table->entries[index].value = _entry;
            table->occupancy++;
            return _entry;
        } 
        else if (strcmp(node.value->name, lexeme) == 0) {
            return node.value;
        }
        else
            collision_count++;
    }
}

entry* symbol_table_fetch(const char* key) {
    if (!table){
        perror("Invalid Read");
        exit(EXIT_FAILURE);
    }

    unsigned long hash = hash_function(key);
    int capacity = table->capacity;

    for (int i = 0; i < capacity; i++) {
        int index = (hash + i) % capacity;
        symbol_table_node node = table->entries[index];

        if (node.is_occupied)
            if (strcmp(node.value->name, key))
                return node.value;
            else 
                continue;
        else
            return NULL;
    }

    return NULL;
}

int sym_get_collision_count(){
    return collision_count;
}