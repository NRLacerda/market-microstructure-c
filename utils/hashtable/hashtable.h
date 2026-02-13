#ifndef HASHTABLE_H
#define HASHTABLE_H


typedef struct Order Order;

typedef struct HashTable {
    Order **buckets;
    int size;
} HashTable;

HashTable* ht_create();
void ht_insert(HashTable *ht, int order_id, Order *order);
void ht_remove(HashTable *ht, int order_id);
Order* ht_get(HashTable *ht, int order_id);
void ht_destroy(HashTable *ht);


#endif