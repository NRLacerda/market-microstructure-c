#ifndef PRICELEVEL_H
#define PRICELEVEL_H

#include "order/order.h"

typedef struct PriceLevel {
    int price;

    long total_volume;

    Order *head;   
    Order *tail;   
} PriceLevel;

PriceLevel* pl_create(int price);
void pl_destroy(PriceLevel *level);
void pl_add_order(PriceLevel *level, Order *order);
void pl_remove_order(PriceLevel *level, Order *order);
int pl_is_empty(PriceLevel *level);

#endif