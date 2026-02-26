#ifndef ORDERBOOK_H
#define ORDERBOOK_H

#include <stddef.h>
#include "order/order.h"
#include "pricelevel/pricelevel.h"
#include "hashtable/hashtable.h"

typedef enum {
    TRADING_HALTED = 0,
    QUOTING = 1,
    TRADING = 2
} TradingState;

typedef struct OrderBook {
    HashTable *order_table;    

    // disclaimer, the best thing here would be to have only one tree structure for both sides and to handle level exclusion in O(1)
    // but for simplicity we are going this way in the moment, we will optimize it later
    HashTable *bid_levels;     
    HashTable *ask_levels;     

    int best_bid;
    int best_ask;

    TradingState state;
} OrderBook;

OrderBook* ob_create(int hash_size);
void ob_destroy(OrderBook *book);
int ob_add_order(OrderBook *book,
                 long order_id,
                 int price,
                 int quantity,
                 int side);   // 0 = bid, 1 = ask
int ob_cancel_order(OrderBook *book, long order_id);
Order* ob_get_order(OrderBook *book, long order_id);
PriceLevel* ob_get_price_level(OrderBook *book, int price, int side);
void ob_set_state(OrderBook *book, TradingState state);
int ob_get_best_bid(OrderBook *book);
int ob_get_best_ask(OrderBook *book);

#endif