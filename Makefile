CC = gcc
CFLAGS = -Wall -Wextra -g

SRC = main.c $(wildcard utils/*/*.c)
OBJ = $(SRC:.c=.o)

TARGET = engine

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET) *.exe