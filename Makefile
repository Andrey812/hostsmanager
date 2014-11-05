SRC_DIR=src/
OBJ_DIR=obj/
BIN_DIR=bin/

all: src/cmd.c
	gcc -c $(SRC_DIR)cmd.c -o $(OBJ_DIR)include.o
	gcc -c $(SRC_DIR)flow.c -o $(OBJ_DIR)flow.o
	gcc -c $(SRC_DIR)daemon.c -o $(OBJ_DIR)daemon.o
	gcc $(OBJ_DIR)include.o $(OBJ_DIR)daemon.o $(OBJ_DIR)flow.o -o $(BIN_DIR)daemon

clean:
	rm $(OBJ_DIR)*.o
	rm $(BIN_DIR)*
