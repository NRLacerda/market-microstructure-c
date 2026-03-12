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
                 OrderSide side)
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
    order->hash_next = NULL;
    order->level = NULL;

    /* Try matching first */
    if (side == SIDE_BID)
        ob_match_asks(book, order);
    else
        ob_match_bids(book, order);

    if (order->quantity > 0) {
        ht_insert(book->order_table, order_id, order);

        ob_insert_order(book, order);

    } else {
        free(order);
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

void ob_match_asks(OrderBook *book, Order *incoming)
{
    while (incoming->quantity > 0 && book->best_ask) {

        PriceLevel *level = book->best_ask;

        if (incoming->price < level->price)
            break;

        Order *resting = level->head;

        int traded = MIN(incoming->quantity, resting->quantity);

        incoming->quantity -= traded;
        resting->quantity -= traded;
        level->total_volume -= traded;

        if (resting->quantity == 0) {

            pl_remove_order(level, resting);

            ht_remove(book->order_table, resting->order_id);

            free(resting);
        }

        if (level->head == NULL) {

            ht_remove(book->ask_levels, level->price);

            if (book->best_ask == level)
                book->best_ask = NULL;

            pl_destroy(level);
        }
    }
}

void ob_match_bids(OrderBook *book, Order *incoming)
{
    while (incoming->quantity > 0 && book->best_bid) {

        PriceLevel *level = book->best_bid;

        if (incoming->price > level->price)
            break;

        Order *resting = level->head;

        int traded = MIN(incoming->quantity, resting->quantity);

        incoming->quantity -= traded;
        resting->quantity -= traded;
        level->total_volume -= traded;

        if (resting->quantity == 0) {

            pl_remove_order(level, resting);

            ht_remove(book->order_table, resting->order_id);

            free(resting);
        }

        if (level->head == NULL) {

            ht_remove(book->bid_levels, level->price);

            if (book->best_bid == level)
                book->best_bid = NULL;

            pl_destroy(level);
        }
    }
}