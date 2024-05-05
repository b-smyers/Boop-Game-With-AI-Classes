.PHONY: build clean

CC = g++
CFLAGS = -O2

HEADER_FILES = $(wildcard ./AI/*.h) AI.h boop.h colors.h Timer.h
SRCS = $(wildcard ./*.cc)

build: a.out

a.out: $(SRCS) $(HEADER_FILES)
	$(CC) $(CFLAGS) $(SRCS)

clean:
	-rm -f a.out
