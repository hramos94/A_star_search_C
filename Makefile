CC      := gcc
CFLAGS  := -std=c99 -O2 -Wall
SRC     := metro_astar.c
TARGET  := metro


START   ?= 0
GOAL    ?= 13

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $< -o $@

run: $(TARGET)
	@echo "Running $(TARGET) from E$$(($(START)+1)) to E$$(($(GOAL)+1))..."
	./$(TARGET) $(START) $(GOAL)

clean:
	rm -f $(TARGET)
