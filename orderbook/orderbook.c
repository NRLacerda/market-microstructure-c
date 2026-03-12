#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include "orderbook.h"
#include "order.h"

#define MAX_ORDERS 1000000

static inline Order *ob_alloc_order(OrderBook *book)
{
    if (book->pool_index >= book->pool_size)
        exit(1);
    
    return &book->order_pool[book->pool_index++];
}

OrderBook* ob_create(int hash_size)
{
    OrderBook *book = malloc(sizeof(OrderBook));
    if (!book) return NULL;

    book->order_pool = malloc(sizeof(Order) * MAX_ORDERS);
    if (!book->order_pool) return NULL;

    book->pool_size = MAX_ORDERS;
    book->pool_index = 0;

    book->order_table = ht_create(hash_size);
    book->bid_levels  = ht_create(hash_size);
    book->ask_levels  = ht_create(hash_size);

    book->best_bid = NULL;
    book->best_ask = NULL;

    book->state = TRADING;

    return book;
}

void ob_destroy(OrderBook *book)
{
    if (!book) return;

    free(book->order_pool);

    ht_destroy(book->order_table);
    ht_destroy(book->bid_levels);
    ht_destroy(book->ask_levels);

    free(book);
}

Order* ob_get_order(OrderBook *book, long order_id)
{
    return (Order*) ht_get(book->order_table, order_id);
}

PriceLevel* ob_get_price_level(OrderBook *book, int price, int side)
{
    HashTable *levels = (side == SIDE_BID) ? book->bid_levels
                                    : book->ask_levels;

    return (PriceLevel*) ht_get(levels, price);
}

int ob_add_order(OrderBook *book,
                 long order_id,
                 int price,
                 int quantity,
                 int side)
{
    if (book->state != TRADING)
        return -1;

    Order *order = ob_alloc_order(book);
    if (!order)
        return -1;

    order->order_id = order_id;
    order->price    = price;
    order->quantity = quantity;
    order->side     = side;

    order->next      = NULL;
    order->prev      = NULL;
    order->hash_next = NULL;
    order->level     = NULL;

    if (side == SIDE_BID)
        ob_match_asks(book, order);
    else
        ob_match_bids(book, order);

    if (order->quantity > 0) {
        ht_insert(book->order_table, order_id, order);

        HashTable *levels =
            (side == SIDE_BID) ? book->bid_levels : book->ask_levels;

        PriceLevel *level = ht_get(levels, price);
        if (!level) {
            level = pl_create(price);
            ht_insert(levels, price, level);
        }

        pl_add_order(level, order);
        order->level = level;

        if (side == SIDE_BID) {
            if (!book->best_bid || price > book->best_bid->price)
                book->best_bid = level;
        } else {
            if (!book->best_ask || price < book->best_ask->price)
                book->best_ask = level;
        }
    }

    return 0;
}

int ob_cancel_order(OrderBook *book, long order_id)
{
    Order *order = ht_remove(book->order_table, order_id);
    if (!order) return -1;

    PriceLevel *level = order->level;
    if (!level) return -1;

    pl_remove_order(level, order);

    level->total_volume -= order->quantity;

    if (level->head == NULL) {

        HashTable *levels = (order->side == SIDE_BID)
                            ? book->bid_levels
                            : book->ask_levels;

        ht_remove(levels, level->price);

        if (book->best_bid == level)
            book->best_bid = NULL;

        if (book->best_ask == level)
            book->best_ask = NULL;

        pl_destroy(level);
    }

    // "freeing" the order, since we use a pool now
    order->quantity = 0;
    order->level    = NULL;
    order->next     = NULL;
    order->prev     = NULL;
    return 0;
}
void ob_set_state(OrderBook *book, TradingState state)
{
    book->state = state;
}

PriceLevel* ob_get_best_bid(OrderBook *book)
{
    return book->best_bid;
}

PriceLevel* ob_get_best_ask(OrderBook *book)
{
    return book->best_ask;
}

void ob_match_asks(OrderBook *book, Order *incoming)
{
    while (incoming->quantity > 0 &&
           book->best_ask &&
           incoming->price >= book->best_ask->price) {

        PriceLevel *level = book->best_ask;
        Order *resting = level->head;

        int trade = incoming->quantity < resting->quantity
                    ? incoming->quantity
                    : resting->quantity;

        incoming->quantity -= trade;
        resting->quantity -= trade;

        if (resting->quantity == 0)
            ob_cancel_order(book, resting->order_id);
    }
}

void ob_match_bids(OrderBook *book, Order *incoming)
{
    while (incoming->quantity > 0 &&
           book->best_bid &&
           incoming->price <= book->best_bid->price) {

        PriceLevel *level = book->best_bid;
        Order *resting = level->head;

        int trade = incoming->quantity < resting->quantity
                    ? incoming->quantity
                    : resting->quantity;

        incoming->quantity -= trade;
        resting->quantity -= trade;

        if (resting->quantity == 0)
            ob_cancel_order(book, resting->order_id);
    }
}

void ob_print_book(OrderBook *book)
{
    printf("\n============= ORDER BOOK =============\n");

    if (book->best_bid)
        printf("\nBest Bid: %d (%ld)\n",
               book->best_bid->price,
               book->best_bid->total_volume);

    if (book->best_ask)
        printf("Best Ask: %d (%ld)\n",
               book->best_ask->price,
               book->best_ask->total_volume);

    printf("======================================\n\n");
}