#
# Makefile ESQUELETO
#
# DEVE ter uma regra "all" para geração da biblioteca
# regra "clean" para remover todos os objetos gerados.
#
# NECESSARIO adaptar este esqueleto de makefile para suas necessidades.
#
#

CC=gcc -m32
LIB_DIR=./lib
INC_DIR=./include
BIN_DIR=./bin
SRC_DIR=./src

all: string.o ler_e_escrever_no_disco.o
	mkdir -p lib
	ar crs $(LIB_DIR)/libt2fs.a $(LIB_DIR)/apidisk.o $(LIB_DIR)/bitmap2.o $(BIN_DIR)/string.o  $(BIN_DIR)/ler_e_escrever_no_disco.o

string.o:
		$(CC) -c $(SRC_DIR)/string.c -o $(BIN_DIR)/string.o -Wall
ler_e_escrever_no_disco.o:
	$(CC) -c $(SRC_DIR)/ler_e_escrever_no_disco.c -o  $(BIN_DIR)/ler_e_escrever_no_disco.o -Wall
