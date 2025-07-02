# Simple Makefile for libtiny3d Task 1
CC = gcc
CFLAGS = -Wall -std=c99 -Iinclude

# Build and run demo
demo:
	$(CC) $(CFLAGS) src/canvas.c demo/main.c -o demo -lm
	./demo

# Clean up
clean:
	del demo.exe
	del task_1demo.pgm

.PHONY: demo clean