#define DEFAULT_HASH_SIZE 262144  // 2^18 a power of two so we can use bitwise comparations to generate the index

#include "hashtable.h"

typedef struct Order {
    long order_id;
    struct Order *next;
    struct Order *prev;
} Order; 

/* 
   our HashTable is basically an array of pointers to the head of a linked list of orders, 
   so we can have multiple orders with the same hash index, we will use chaining to handle collisions
   this will have in general O(1) lookup time and O(n) in the worst cases, this way we don't have literal collisions and waste of memory
*/

HashTable* ht_create() {
    HashTable *ht = malloc(sizeof(HashTable));
    ht->size = DEFAULT_HASH_SIZE;
    ht->buckets = calloc(DEFAULT_HASH_SIZE, sizeof(Order*));
    return ht;
}

void ht_insert(HashTable *ht, int order_id, Order *order) {
    int index = order_id & (ht->size - 1);
    ht->buckets[index] = order;
}

void ht_remove(HashTable *ht, int order_id){
    int index = order_id & (ht-> size -1);
    Order *order = ht->buckets[index]; // returns the head of the list of orders

    while(order && order->order_id != order_id){
        order = order->next;
    }
    
    if(!order) return;

    if(order->prev){
        order->prev->next = order->next; // change the previous pointer to skip current node, so its deleted from the list
    }
    else{
        ht->buckets[index] = order->next; // in case we dont have an previous
    }

    if(order->next)
    {
        order->next->prev = order->prev; // change the next pointer to skip current node, so its deleted from the list
    }   

    free(order);
}

void ht_destroy(HashTable *ht){
    void ht_destroy(HashTable *ht) {
    for (int i = 0; i < ht->size; i++) { // we need to iterate through all the buckets to free the orders
        Order *current = ht->buckets[i];

        while (current) { // then iterate through all items of the list to free them
            Order *next = current->hash_next;
            free(current);
            current = next;
        }
    }

    free(ht->buckets);
    free(ht);
}