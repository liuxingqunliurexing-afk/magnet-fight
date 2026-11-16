CC = gcc
CFLAGS = -Wall -Wextra -O2 -Iinclude
LDFLAGS = -lSDL2 -lm

TARGET = magnet_app

SRCS = src/main.c \
       src/server.c \
       src/client.c \
       src/game.c \
       src/magnet.c \
       src/player.c \
       src/ui.c \
       src/network.c

OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean