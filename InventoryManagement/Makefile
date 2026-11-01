CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -pedantic -O2
SRCS = main.c inventory.c sorting.c file.c
OBJS = $(SRCS:.c=.o)
TARGET = inventory_app.exe
TEST_TARGET = test_suite.exe

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

test: $(TEST_TARGET)
	./$(TEST_TARGET)

$(TEST_TARGET): test_suite.c inventory.c sorting.c file.c
	$(CC) $(CFLAGS) test_suite.c inventory.c sorting.c file.c -o $(TEST_TARGET)

clean:
	rm -f $(OBJS) $(TARGET) $(TEST_TARGET) *.o

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean test run
