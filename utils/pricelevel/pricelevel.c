#include <stdlib.h>
#include "pricelevel.h"

PriceLevel* pl_create(int price)
{
    PriceLevel *level = malloc(sizeof(PriceLevel));
    if (!level) return NULL;

    level->price = price;
    level->total_volume = 0;
    level->head = NULL;
    level->tail = NULL;

    return level;
}

void pl_destroy(PriceLevel *level)
{
    if (!level) return;
    free(level);
}

void pl_add_order(PriceLevel *level, Order *order)
{
    if (!level || !order) return;

    order->next = NULL;
    order->prev = level->tail;

    if (level->tail)
        level->tail->next = order;
    else
        level->head = order;   // first order in level

    level->tail = order;

    level->total_volume += order->quantity;
}

void pl_remove_order(PriceLevel *level, Order *order)
{
    // O(1) removal since the pointer of next/prev lives inside the Order, so we just need update the pointers of the prev/next nodes
    if (!level || !order) return;

    /* Fix previous link */
    if (order->prev)
        order->prev->next = order->next;
    else
        level->head = order->next;   

    /* Fix next link */
    if (order->next)
        order->next->prev = order->prev;
    else
        level->tail = order->prev;   

    level->total_volume -= order->quantity;

    order->next = NULL;
    order->prev = NULL;
}

int pl_is_empty(PriceLevel *level)
{
    return (level->head == NULL);
}