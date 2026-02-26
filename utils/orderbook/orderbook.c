#include <stdlib.h>
#include <limits.h>
#include "orderbook.h"

OrderBook* ob_create(int hash_size)
{
    OrderBook *book = malloc(sizeof(OrderBook));
    if (!book) return NULL;

    book->order_table = ht_create(hash_size);
    book->bid_levels  = ht_create(hash_size);
    book->ask_levels  = ht_create(hash_size);

    book->best_bid = 0;
    book->best_ask = INT_MAX;

    book->state = TRADING_HALTED;

    return book;
}

void ob_destroy(OrderBook *book)
{
    if (!book) return;

    /* Destroy all orders */
    ht_foreach(book->order_table, free); 
    // assumes your generic ht_foreach applies free(value)

    /* Destroy price levels */
    ht_foreach(book->bid_levels, (void(*)(void*))pl_destroy);
    ht_foreach(book->ask_levels, (void(*)(void*))pl_destroy);

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
    HashTable *levels = (side == 0) ? book->bid_levels
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

    /* Allocate Order */
    Order *order = malloc(sizeof(Order));
    if (!order) return -1;

    order->order_id = order_id;
    order->price = price;
    order->quantity = quantity;
    order->side = side;
    order->next = NULL;
    order->prev = NULL;

    /* Insert into order lookup table */
    ht_insert(book->order_table, order_id, order);

    /* Get correct price level table */
    HashTable *levels = (side == 0)
                        ? book->bid_levels
                        : book->ask_levels;

    /* Find or create price level */
    PriceLevel *level = ht_get(levels, price);

    if (!level) {
        level = pl_create(price);
        ht_insert(levels, price, level);
    }

    /* Add order to FIFO */
    pl_add_order(level, order);

    /* Update best prices */
    if (side == 0) {  // BID
        if (price > book->best_bid)
            book->best_bid = price;
    } else {          // ASK
        if (price < book->best_ask)
            book->best_ask = price;
    }

    return 0;
}

int ob_cancel_order(OrderBook *book, long order_id)
{
    Order *order = ht_remove(book->order_table, order_id);
    if (!order) return -1;

    HashTable *levels = (order->side == 0)
                        ? book->bid_levels
                        : book->ask_levels;

    PriceLevel *level = ht_get(levels, order->price);
    if (!level) return -1;

    pl_remove_order(level, order);

    /* If price level empty → remove it */
    if (level->head == NULL) {

        ht_remove(levels, order->price);
        pl_destroy(level);

        /* best price might need recalculation */
        // For now naive reset
        if (order->side == 0)
            book->best_bid = 0;
        else
            book->best_ask = INT_MAX;
    }

    free(order);
    return 0;
}

void ob_set_state(OrderBook *book, TradingState state)
{
    book->state = state;
}

int ob_get_best_bid(OrderBook *book)
{
    return book->best_bid;
}

int ob_get_best_ask(OrderBook *book)
{
    return book->best_ask;
}