CC:=clang

CFLAGS= -g -lm -Wall -DDEBUG -O3
TARGET=sprgo

DIRS= cli object include  compiler parser vm gc
CFILES = $(foreach dir, $(DIRS), $(wildcard $(dir)/*.c))

OBJS=$(patsubst %.c,%.o,$(CFILES))

$(TARGET):$(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(CFLAGS)


.PHONY: clean
clean:
	-rm $(TARGET) $(OBJS)

all: clean $(TARGET)
	./sprgo ./test/test.go