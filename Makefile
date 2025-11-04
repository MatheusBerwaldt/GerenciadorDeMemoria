CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
TARGET = memory_manager
OBJS = main.o mymemory.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

main.o: main.c mymemory.h
	$(CC) $(CFLAGS) -c main.c

mymemory.o: mymemory.c mymemory.h
	$(CC) $(CFLAGS) -c mymemory.c

clean:
	rm -f $(OBJS) $(TARGET)

run: $(TARGET)
	./$(TARGET)

valgrind: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)

.PHONY: all clean run valgrind