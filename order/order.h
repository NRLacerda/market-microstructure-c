#ifndef ORDER_H
#define ORDER_H

typedef struct PriceLevel PriceLevel;   

typedef enum OrderSide{
    SIDE_BID = 1,
    SIDE_ASK = -1
} OrderSide;

typedef struct Order {
    long order_id;
    int price;
    int quantity;
    OrderSide side;

    struct Order *next;
    struct Order *prev;

    struct Order *hash_next;  

    PriceLevel *level;
} Order;
#endif