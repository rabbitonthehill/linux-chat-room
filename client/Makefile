SRC = $(wildcard ./source/*.c)
OBJ = $(patsubst ./source/%.c, ./source/%.o, $(SRC))
TARGET = ./bin/chat-room-client
CC := gcc
LIB = 
INCLUDE = 
RM = rm -rf

$(TARGET):$(OBJ)
	$(CC) -g $^ -o $(TARGET) $(LIB)

.PHONY:
clear:
	$(RM) ./source/*.o $(TARGET)
