CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c11
INCLUDES = -Iinclude

SRCS = src/lasm.c src/parser.c src/semantic.c
OBJS = $(SRCS:.c=.o)
TARGET = lasm

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)

.PHONY: all cleans
