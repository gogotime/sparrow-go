CC:=clang

CFLAGS= -fsanitize=address -fno-omit-frame-pointer -g -lm -Wall -DDEBUG -O1
TARGET=sprgo

DIRS= cli  include  compiler parser vm object gc
CFILES = $(foreach dir, $(DIRS), $(wildcard $(dir)/*.c))

OBJS=$(patsubst %.c,%.o,$(CFILES))

$(TARGET):$(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(CFLAGS)


.PHONY: clean
clean:
	-rm $(TARGET) $(OBJS)

all: clean $(TARGET)
	./sprgo ./test/test.go