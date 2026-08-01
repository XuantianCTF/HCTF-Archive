CC := gcc
CFLAGS := -Wall -Wextra -O2
TARGET := tree

all: $(TARGET)

$(TARGET): tree.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGET)

.PHONY: all clean
