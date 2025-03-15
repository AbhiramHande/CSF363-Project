#include "hash_map.h"

#define INITIAL_TABLE_SIZE 400

typedef struct Entry map_entry;
typedef struct HashMap hash_map;

struct Entry {
    unsigned int key;
    production* value;
    bool is_occupied;
};

struct HashMap {
    map_entry* entries;
    int capacity;
    int size;
};

static int collision_count = 0;
static hash_map* _table = NULL;

static void map_create(void) __attribute__((constructor));
static void map_cleanup(void) __attribute__((destructor));

static unsigned long hash_function(unsigned int key);
static bool is_prime(unsigned int n);
static int next_prime(int n);

static unsigned long hash_function(unsigned int key) {
    unsigned long hash;
    for(hash = 5381; key; key >>= 8)
        hash = ((hash << 5) + hash) ^ key;
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

static void map_create(void) {
    hash_map* map = malloc(sizeof(struct HashMap));
    if (!map){
        perror("Unable to create hash map for parse table");
        exit(EXIT_FAILURE);
    }
        
    
    map->capacity = next_prime(INITIAL_TABLE_SIZE);
    map->entries = calloc(map->capacity, sizeof(struct Entry));
    if (!map){
        free(map);
        perror("Unable to create hash map for parse table");
        exit(EXIT_FAILURE);
    }

    map->size = 0;
    collision_count = 0;
    _table = map;
    return;
}

static void map_cleanup(void) {
    if (!_table) return;

    free(_table->entries);
    _table->entries = NULL;

    free(_table);
    _table = NULL;
}

void map_insert(const unsigned int key, production* value) {
    if (!_table) 
        return;

    unsigned long hash = hash_function(key);
    int capacity = _table->capacity;

    // Currently using linear probing --> change to Double hashing
    for (int i = 0; i < capacity; i++) {
        int index = (hash + i) % capacity;
        map_entry* _entry = &_table->entries[index];

        if (!_entry->is_occupied) {
            _entry->key = key;
            _entry->value = value;
            _entry->is_occupied = true;
            _table->size++;
            return;
        } 
        else if (_entry->key == key) {
            _entry->value = value;
            return;
        }
        else
            collision_count++;
    }
}

production* map_fetch(const unsigned int key) {
    if (!_table){
        perror("Invalid Read");
        exit(EXIT_FAILURE);
    }

    unsigned long hash = hash_function(key);
    int capacity = _table->capacity;

    for (int i = 0; i < capacity; i++) {
        int index = (hash + i) % capacity;
        map_entry* _entry = &_table->entries[index];

        if (_entry->is_occupied)
            if (_entry->key == key)
                return _entry->value;
            else 
                continue;
        else
            return NULL;
    }

    return NULL;
}

int get_collision_count(){
    return collision_count;
}