#include <stdio.h>
#include "event.h"
#include "engine.h"

int engine_read_event(FILE *file, Event *event) 
{
    return fscanf(
        file,
        "%lf,%d,%ld,%d,%ld,%d",
        &event->timestamp,
        &event->type,
        &event->order_id,
        &event->quantity,
        &event->price,
        &event->side
    ) == 6;
}

void engine_run(Engine *engine, FILE *input) 
{
    if (!engine || !input)
       return;

    Event event;

    engine->running = 1;

    while (engine->running && engine_read_event(input, &event)) {
        engine_process_event(engine, &event);
    }
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

void engine_destroy(Engine *engine)
{
    if (!engine)
        return;

    if (engine->book)
        ob_destroy(engine->book);

    free(engine);
}

void engine_stop(Engine *engine)
{
    if (!engine)
        return;

    engine->running = 0;
}