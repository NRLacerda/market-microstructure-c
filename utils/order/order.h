#ifndef ORDER_H
#define ORDER_H

typedef struct Order {
    long order_id;
    int price;
    int quantity;
    int side;

    struct Order *next;
    struct Order *prev;

    struct Order *hash_next;  
};

#endif