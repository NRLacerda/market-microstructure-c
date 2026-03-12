CC = gcc
CFLAGS = -Wall -Wextra -g \
         -Iengine \
         -Ievent \
         -Ihashtable \
         -Iorder \
         -Iorderbook \
         -Ipricelevel

TARGET = market_microstructure

SRC = main.c \
      engine/engine.c \
      hashtable/hashtable.c \
      orderbook/orderbook.c \
      pricelevel/pricelevel.c

OBJ = $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)