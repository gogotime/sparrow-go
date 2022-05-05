CC:=gcc
all:
	${CC} main.c -o main
	./main

.PHONY: clean
clean:
