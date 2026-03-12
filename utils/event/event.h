#ifndef EVENT_H
#define EVENT_H

typedef struct Event {
    double timestamp;
    EventType type;
    long order_id;
    int quantity;
    int price;
    OrderSide side;
} Event;

typedef enum {
    EVENT_ADD = 1,
    EVENT_CANCEL = 2
} EventType;

typedef enum {
    SIDE_BID = 0,
    SIDE_ASK = 1
} OrderSide;

#endif