#ifndef HASHTABLE_H
#define HASHTABLE_H
typedef struct HashNode {
    long key;
    void *value;
    struct HashNode *next;
} HashNode;

typedef struct HashTable {
    HashNode **buckets;
    int size;
} HashTable;

HashTable* ht_create(int size);

void ht_insert(HashTable *ht, long key, void *value);

void* ht_get(HashTable *ht, long key);

void* ht_remove(HashTable *ht, long key);

void ht_destroy(HashTable *ht);


#endif