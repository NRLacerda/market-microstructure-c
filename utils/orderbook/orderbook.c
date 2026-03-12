#include <stdlib.h>
#include <limits.h>
#include "orderbook.h"
#include "order/order.h"

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

    Order *order = malloc(sizeof(Order));
    if (!order) return -1;

    order->order_id = order_id;
    order->price = price;
    order->quantity = quantity;
    order->side = side;
    order->next = NULL;
    order->prev = NULL;
    order->level = NULL;

    ht_insert(book->order_table, order_id, order);

    HashTable *levels = (side == 0)
                        ? book->bid_levels
                        : book->ask_levels;

    PriceLevel *level = ht_get(levels, price); // i forgot we put the pricelevels as an hashtable xd

    if (!level) {
        level = pl_create(price);
        ht_insert(levels, price, level);
    }

    pl_add_order(level, order);

    order->level = level;

    /* Update best pointers */
    if (side == 0) { // BID
        if (!book->best_bid || price > book->best_bid->price)
            book->best_bid = level;
    } else { // ASK
        if (!book->best_ask || price < book->best_ask->price)
            book->best_ask = level;
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

        HashTable *levels = (order->side == 0)
                            ? book->bid_levels
                            : book->ask_levels;

        ht_remove(levels, level->price);

        if (book->best_bid == level)
            book->best_bid = NULL;

        if (book->best_ask == level)
            book->best_ask = NULL;

        pl_destroy(level);
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