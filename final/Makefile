CC = gcc
CFLAGS = -Wall -Iinclude -std=c11 -pthread
LDFLAGS = -lSDL2 -lSDL2_image -lSDL2_mixer -lpthread

SRC_DIR = src
OBJ_DIR = obj

SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SOURCES))
TARGET = block-breaker

all: $(TARGET) game 

$(TARGET): $(OBJECTS)
	mkdir -p $(OBJ_DIR)
	$(CC) -o $@ $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

game: $(SOURCES)  # 모든 소스 파일 추가
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -rf $(OBJ_DIR) $(TARGET) game 

.PHONY: all clean
