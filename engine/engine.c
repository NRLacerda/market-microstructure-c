#include <stdio.h>
#include <stdlib.h>
#include "event.h"
#include "engine.h"


int engine_read_event(FILE *file, Event *event)
{
    char line[128];

    if (!fgets(line, sizeof(line), file))
        return 0;

    char *p = line;

    event->timestamp = strtod(p, &p);
    if (*p == ',') p++;

    event->type = (int)strtol(p, &p, 10);
    if (*p == ',') p++;

    event->order_id = strtol(p, &p, 10);
    if (*p == ',') p++;

    event->quantity = (int)strtol(p, &p, 10);
    if (*p == ',') p++;

    event->price = (int)strtol(p, &p, 10);
    if (*p == ',') p++;

    event->side = (int)strtol(p, &p, 10);

    return 1;
}

long engine_run(Engine *engine, FILE *input) 
{
    if (!engine || !input)
       return 0;

    Event event;

    engine->running = 1;
    long event_count = 0;

    while (engine->running && engine_read_event(input, &event)) {
        engine_process_event(engine, &event);
        event_count++;
    }

    return event_count;
}

Engine* engine_create(int hash_size)
{
    Engine *engine = malloc(sizeof(Engine));
    if (!engine)
        return NULL;

    engine->book = ob_create(hash_size);
    engine->running = 0;

    return engine;
}

void engine_process_event(Engine *engine, Event *event)
{
    switch (event->type)
    {
        case EVENT_ADD:
            ob_add_order(
                engine->book,
                event->order_id,
                event->price,
                event->quantity,
                event->side
            );
            break;

        case EVENT_CANCEL:
            ob_cancel_order(engine->book, event->order_id);
            break;

        default:
            break;
    }
}

void engine_destroy(Engine *engine)
{
    if (!engine)
        return;

    if (engine->book){
        ob_print_book(engine->book);
        ob_destroy(engine->book);
    }
    
    free(engine);
}

void engine_stop(Engine *engine)
{
    if (!engine)
        return;

    engine->running = 0;
}