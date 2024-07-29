CC = gcc
CFLAGS = -Wall -Wextra -O2
LDFLAGS = -lcap

SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)
INCLUDE = -Iinclude

all: container_runtime

container_runtime: $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c -o $@ $<

clean:
	rm -f $(OBJ) container_runtime

.PHONY: all clean