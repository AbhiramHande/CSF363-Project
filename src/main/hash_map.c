#include "../../include/hash_map.h"

/**
 * @brief Defines the initial capacity of the hash map.
 * 
 * @warning The hash map starts with this size, and if it is full, no further elements can be added.
 */
#define INITIAL_TABLE_SIZE 400

/*****************************************************************************
 *                                                                           *
 *                     STRUCTURES INTERNAL TO THE FILE                       *
 *                                                                           *
 *****************************************************************************/

/**
 * @brief Data structure representing a hash map.
 * 
 * @details 
 * - The hash map is indexed by a unique **key** and each key maps to a respective **value**.
 * - Collision resolution is handled using **open addressing with linear probing**.
 * 
 * @note
 * - The symbol table is dynamically allocated and must be initialized using `map_create`.
 * - The hash map is deallocated using `map_cleanup` to prevent memory leaks.
 * 
 * @attention
 * - **Lookup Complexity:** Average @f$ \mathcal{O}(1) @f$, Worst-case @f$ \mathcal{O}(n) @f$ (in the case of high collision rates).
 * - **Insertion Complexity:** Average @f$ \mathcal{O}(1) @f$, but rehashing can incur additional overhead.
 */
typedef struct HashMap hash_map;

/**
 * @brief Represents an entry in the hash map.
 * 
 * @details 
 * - Each entry contains a unique key (an unsigned integer) and a value (a pointer to a production rule).
 * - Also includes an `is_occupied` flag to indicate whether the entry is in use.
 * - These entries are used for storage and retrieval within the hash map.
 */
typedef struct Entry map_entry;

struct Entry {
    unsigned int key; /**< The unique key for this entry */
    production* value; /**< Pointer to the value associated with the key (here, a production rule) */
    bool is_occupied; /**< Indicates if the entry is currently occupied by a valid key-value pair */
};

struct HashMap {
    map_entry* entries; /**< Array of entries in the hash map */
    int capacity; /**< Total number of slots available in the hash map */
    int size; /**< Number of currently occupied slots in the hash map */
};


/*****************************************************************************
 *                                                                           *
 *                      GLOBAL VARIABLES IN THIS FILE                        *
 *                                                                           *
 *****************************************************************************/

/**
 * @brief Tracks the number of collisions encountered in the hash map.
 * 
 * @note 
 * - This value is primarily used for debugging and performance analysis of the hashing mechanism.
 * - It is automatically reset to zero when `map_create` is called.
 */
static int collision_count = 0;

/**
 * @brief A pointer to the global hash map representing the parse table.
 * 
 * @details The parse table is implemented as a hash map to optimize lookup 
 *          operations during parsing, improving efficiency compared to a 2D array.
 * 
 * @note 
 * - Defined as a static global variable, restricting access to this file.
 * - Enforces a singleton-like behavior, ensuring only one instance exists.
 * - Can only be accessed and modified via `map_insert` and `map_fetch`.
 * - Prevents unintended modifications by enforcing encapsulation.
 */
static hash_map* _table = NULL;


/*****************************************************************************
 *                                                                           *
 *                     FUNCTIONS INTERNAL TO THE FILE                        *
 *                                                                           *
 *****************************************************************************/

/**
 * @ingroup Hash map internal
 * 
 * @brief Initializes the hash map (automatically called on startup).
 * 
 * @pre `_table` should be uninitialized or `NULL`.
 *
 * @post `_table` is set to a newly created empty hash table.
 * @post `collision_count` is reset to `0`.
 * 
 * @warning Modifies the global variables `_table` and `collision_count`. 
 */
static void map_create(void) __attribute__((constructor));

/**
 * @ingroup Hash map internal
 * 
 * @brief Deallocates the hash map (automatically called at program termination).
 * 
 * @post `_table` is freed and set to `NULL`.
 * 
 * @details Frees the memory allocated for the hash table and its entries.
 *          However, the caller is responsible for freeing any values (pointers) 
 *          that were inserted into the table.
 */
static void map_cleanup(void) __attribute__((destructor));

/**
 * @ingroup Hash map internal
 * 
 * @brief Computes a hash value for a given key.
 * 
 * @details Implements a variation of the **djb2** hash function. 
 *          - Starts with a prime number `5381`.
 *          - Multiplies by `33`, XORs with the key, and shifts the key left by `8` bits.
 *          - Repeats until the key becomes `0`.
 * 
 * @param key The unsigned integer key for which the hash is computed.
 * 
 * @return The computed hash as an `unsigned long`.
 */
static unsigned long hash_function(unsigned int key);

/**
 * @ingroup Hash map internal
 * 
 * @brief Determines whether a given number is prime in \(O(\sqrt{n})\).
 * 
 * @param n The number to check for primality.
 * 
 * @return `true` if the number is prime, otherwise `false`.
 */
static bool is_prime(unsigned int n);

/**
 * @ingroup Hash map internal
 * 
 * @brief Finds the next prime number greater than a given value.
 * 
 * @param n The number after which the next prime is sought.
 * 
 * @return The next prime number as an `int`.
 */
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


/*****************************************************************************
 *                                                                           *
 *                  FUNCTIONS DEFINED IN THE HEADER FILE                     *
 *                                                                           *
 *****************************************************************************/

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