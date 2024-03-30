.PHONY: build clean

CC = g++
CFLAGS =

HEADER_FILES = $(wildcard ./AI/*.h) AI.h boop.h colors.h Timer.h
SRCS = main.cc boop.cc

build: a.out

a.out: $(SRCS) $(HEADER_FILES)
	$(CC) $(CFLAGS) $(SRCS)

clean:
	-rm -f a.out