SRC_DIR=src/
OBJ_DIR=obj/
BIN_DIR=bin/

BIN_NAME=hostmon

all: src/cmd.c
	gcc -c $(SRC_DIR)cmd.c -o $(OBJ_DIR)include.o
	gcc -c $(SRC_DIR)flow.c -o $(OBJ_DIR)flow.o
	gcc -c $(SRC_DIR)daemon.c -o $(OBJ_DIR)daemon.o
	gcc -c $(SRC_DIR)raidix.c -o $(OBJ_DIR)raidix.o
	gcc -c $(SRC_DIR)hosts.c -o $(OBJ_DIR)hosts.o
	gcc -c $(SRC_DIR)remote_cmd.c -o $(OBJ_DIR)remote_cmd.o
	gcc $(OBJ_DIR)include.o $(OBJ_DIR)daemon.o $(OBJ_DIR)flow.o $(OBJ_DIR)raidix.o $(OBJ_DIR)hosts.o $(OBJ_DIR)remote_cmd.o -o $(BIN_DIR)$(BIN_NAME)

clean:
	rm $(OBJ_DIR)*.o
	rm $(BIN_DIR)$(BIN_NAME)
