#ifndef ENGINE_H
#define ENGINE_H

#include "orderbook.h"
#include "event.h"
#include <stdio.h>

typedef struct Engine {
    OrderBook *book;
    int running;
} Engine;

Engine* engine_create(int hash_size);
void engine_destroy(Engine *engine);
void engine_process_event(Engine *engine, Event *event);
void engine_run(Engine *engine, FILE *input);
void engine_stop(Engine *engine);
int engine_read_event(FILE *file, Event *event);

#endif