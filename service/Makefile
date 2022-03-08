SRC = $(wildcard ./source/*.c)
OBJ = $(patsubst ./source/%.c, ./source/%.o, $(SRC))
LIB = 
INCLUDE = 
DEBUG = 
CC := gcc
RM = rm -rf
TARGET = ./bin/chat-room-service

$(TARGET):$(OBJ)
	$(CC) -g $(DEBUG) $^ -o $(TARGET) $(LIB)

.PHONY:
clear:
	$(RM) ./source/*.o $(TARGET)