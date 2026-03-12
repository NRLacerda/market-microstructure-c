#define DEFAULT_HASH_SIZE 262144  // 2^18 a power of two so we can use bitwise comparations to generate the index

#include "hashtable.h"
#include <stdlib.h>  
#include "../order/order.h"

/* 
   our HashTable is basically an array of pointers to the head of a linked list of orders, 
   so we can have multiple orders with the same hash index, we will use chaining to handle collisions
   this will have in general O(1) lookup time and O(n) in the worst cases, this way we don't have literal collisions and waste of memory
*/

HashTable* ht_create(int size) {
    HashTable *ht = malloc(sizeof(HashTable));
    ht->size = size;
    ht->buckets = calloc(size, sizeof(HashNode*));
    return ht;
}

void ht_insert(HashTable *ht, long key, void *value) {
    int index = key & (ht->size - 1);

    HashNode *node = malloc(sizeof(HashNode));
    node->key = key;
    node->value = value;

    // in practice the adding is O(1), putting the new item as the previous of the actual head
    node->next = ht->buckets[index];
    ht->buckets[index] = node;
}

void* ht_get(HashTable *ht, long key) {
    int index = key & (ht->size - 1);

    HashNode *current = ht->buckets[index];

    while (current) {
        if (current->key == key)
            return current->value;

        current = current->next;
    }

    return NULL;
}

void* ht_remove(HashTable *ht, long key) {
    int index = key & (ht->size - 1);

    HashNode *current = ht->buckets[index];
    HashNode *prev = NULL;

    while (current) {
        if (current->key == key) {

            if (prev)
                prev->next = current->next;
            else
                ht->buckets[index] = current->next;

            void *value = current->value;
            free(current);  // free node only

            return value;   // return stored pointer
        }

        prev = current;
        current = current->next;
    }

    return NULL;
}

void ht_destroy(HashTable *ht) {
    for (int i = 0; i < ht->size; i++) {
        HashNode *current = ht->buckets[i];

        while (current) {
            HashNode *next = current->next;
            free(current);
            current = next;
        }
    }

    free(ht->buckets);
    free(ht);
}