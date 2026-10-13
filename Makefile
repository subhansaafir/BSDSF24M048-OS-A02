
CC = gcc
CFLAGS = -Wall -Wextra
TARGET = bin/lsv1.0.0
SOURCE = src/lsv1.0.0.c

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) $(SOURCE) -o $(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all clean
