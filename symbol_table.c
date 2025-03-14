#include "symbol_table.h"

#define INITIAL_TABLE_SIZE 100

typedef struct SymbolTable symbol_table;
typedef struct SymbolTableNode symbol_table_node;
typedef struct SymbolTableEntry entry;

struct SymbolTable {
    symbol_table_node* entries;
    int capacity;
    int occupancy;
};

struct SymbolTableNode {
    entry* value;
    bool is_occupied;
};

static int collision_count = 0;
symbol_table* table = NULL;

static void symbol_table_create(void) __attribute__((constructor));
static void symbol_table_cleanup(void) __attribute__((destructor));

static unsigned long hash_function(const char* key) {
    unsigned long hash;
    for(hash = 5381; *key != '\0'; key++)
        hash = ((hash << 5) + hash) ^ *key;
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

//add all keywords here
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
            assert(strcmp(node.value->name, _entry->name) != 0);
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

    return;
}

// should add token_type
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