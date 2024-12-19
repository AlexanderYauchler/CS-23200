CC = gcc
CFLAGS = -Wall -g -std=c99

TARGET = webSearch.exe
SRC = webSearch.c crawler.c
OBJ = $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) -lm

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm $(OBJ) $(TARGET)

rebuild: clean all
