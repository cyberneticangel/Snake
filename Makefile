CC      = clang
CFLAGS  = -Wall -Wextra -g
LDLIBS  = -lncurses
TARGET  = bin/snake

$(TARGET): snake.c snake.h
	mkdir -p bin
	$(CC) $(CFLAGS) -o $(TARGET) snake.c $(LDLIBS)

clean:
	rm -f $(TARGET)

.PHONY: clean
