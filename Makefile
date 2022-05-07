CC:=gcc

CFLAGS= -g -lm -Wall
TARGET=sprgo

DIRS= object include cli compiler parser vm gc
CFILES = $(foreach dir, $(DIRS), $(wildcard $(dir)/*.c))

OBJS=$(patsubst %.c,%.o,$(CFILES))

$(TARGET):$(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(CFLAGS)


.PHONY: clean
clean:
	rm $(TARGET) $(OBJS)