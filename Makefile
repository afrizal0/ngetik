CC = cc
FLAGS = -Wall -Werror -Wpedantic -Wextra
SRC = src
OBJ = obj
BIN = ngetik
SRC_FILES = $(wildcard $(SRC)/*.[ch])
OBJ_FILES = $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(wildcard $(SRC)/*.c))

$(BIN): $(OBJ_FILES)
	$(CC) $(FLAGS) -o $@ $(OBJ_FILES) 

$(OBJ)/main.o: $(SRC_FILES)
	$(CC) $(FLAGS) -c -o $@ $(SRC)/main.c


$(OBJ)/%.o: $(SRC)/%.c $(SRC)/%.h
	$(CC) $(FLAGS) -c -o $@ $<
