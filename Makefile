CC = gcc
CFLAGS = -Wall -Wextra -O2
TARGET = reversi
SRCS = main.c board.c ai.c
OBJS = $(SRCS:.c=.o)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c reversi.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: clean
