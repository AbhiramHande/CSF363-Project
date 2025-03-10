#include "hash_map.h"

static unsigned long hash_function(unsigned int key) {
    unsigned long hash;
    for(hash = 5381; key; key >>= 2)
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

// static void hm_resize(HashMap* hashmap, size_t new_capacity);

hash_map* map_create(int total_entries) {
    hash_map* map = malloc(sizeof(struct HashMap));
    if (!map) 
        return NULL;
    
    map->capacity = total_entries << 1;
    map->entries = calloc(map->capacity, sizeof(struct Entry));
    if (!map->entries) {
        free(map);
        return NULL;
    }

    map->size = 0;

    //atexit(map_cleanup);
    return map;
}

void map_insert(hash_map* map, const unsigned int key, production* value) {
    if (!map) 
        return;

    unsigned long hash = hash_function(key);
    int capacity = map->capacity;

    // Currently using linear probing --> change to Double hashing
    for (int i = 0; i < capacity; i++) {
        int index = (hash + i) % capacity;
        entry* _entry = &map->entries[index];

        if (!_entry->is_occupied) {
            _entry->key = key;
            _entry->value = value;
            _entry->is_occupied = true;
            map->size++;
            return;
        } 
        else if (_entry->key == key) {
            _entry->value = value;
            return;
        }
    }
}

production* map_fetch(hash_map* map, const unsigned int key) {
    if (!map) return NULL;

    unsigned long hash = hash_function(key);
    int capacity = map->capacity;

    for (int i = 0; i < capacity; i++) {
        int index = (hash + i) % capacity;
        entry* _entry = &map->entries[index];

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

void map_cleanup(hash_map* map) {
    if (!map) return;

    free(map->entries);
    map->entries = NULL;

    free(map);
    map = NULL;
}