#include <stdio.h>
#include "event.h"
#include "engine.h"

int engine_read_event(FILE *file, Event *event) {
    return fscanf(
        file,
        "%lf,%d,%ld,%d,%ld,%d",
        &event->time,
        &event->type,
        &event->order_id,
        &event->size,
        &event->price,
        &event->direction
    ) == 6;
}

void engine_run(Engine *engine, FILE *input) {
    Event event;

    engine->running = 1;

    while (engine->running && engine_read_event(input, &event)) {
        engine_process_event(engine, &event);
    }
}