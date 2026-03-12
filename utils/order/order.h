#include "pricelevel/pricelevel.h"

#ifndef ORDER_H
#define ORDER_H

typedef enum {
    SIDE_BID = 0,
    SIDE_ASK = 1
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