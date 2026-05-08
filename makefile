CC = gcc
CFLAGS = -Wall -Wextra -g -Iinclude -lpthread

SRC = main.c threadpool.c
OBJ = $(SRC:.c=.o)
TARGET = thread

all: $(TARGET)

run: $(TARGET)
	./$(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean
