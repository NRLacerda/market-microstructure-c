#include <stdio.h>
#include "engine.h"
#include <time.h>

int main() {

    FILE *file = fopen(
        "res/AMZN_2012-06-21_34200000_57600000_message_1.csv",
        "r"
    );

    if (!file) {
        printf("Failed to open file\n");
        return 1;
    }

    clock_t start = clock();
    Engine *engine = engine_create(100000);

    if (!engine) {
        printf("Failed to create engine\n");
        fclose(file);
        return 1;
    }

    printf(
        "\n"
        "=============================================\n"
        "        MARKET MICROSTRUCTURE ENGINE\n"
        "           Limit Order Book Replay\n"
        "---------------------------------------------\n"
        " Project : market-microstructure-c\n"
        " Language: C\n"
        " Engine  : Event-driven LOB simulator\n"
        "=============================================\n\n"
    );

    long event_count = engine_run(engine, file);
  
    engine_destroy(engine);
    
    clock_t end = clock();
    double elapsed = (double)(end - start) / CLOCKS_PER_SEC;

    printf("Processed %ld events\n", event_count);
    printf("Replay time: %.2f seconds\n", elapsed);
    printf("Throughput: %.2f events/sec\n", event_count / elapsed);

    fclose(file);

    return 0;
}