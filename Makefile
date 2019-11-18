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

all: string.o t2fs.o ler_e_escrever_no_disco.o leitura_arquivos.o escrever_arquivos.o
	$(CC) -o executavel $(BIN_DIR)/leitura_arquivos.o  $(BIN_DIR)/escrever_arquivos.o $(BIN_DIR)/ler_e_escrever_no_disco.o  $(BIN_DIR)/t2fs.o $(BIN_DIR)/string.o $(LIB_DIR)/apidisk.o $(LIB_DIR)/bitmap2.o

string.o:
	$(CC) -c $(SRC_DIR)/string.c -o $(BIN_DIR)/string.o -Wall

t2fs.o:
	$(CC) -c $(SRC_DIR)/t2fs.c -o $(BIN_DIR)/t2fs.o -Wall

ler_e_escrever_no_disco.o:
	$(CC) -c $(SRC_DIR)/ler_e_escrever_no_disco.c -o $(BIN_DIR)/ler_e_escrever_no_disco.o -Wall

leitura_arquivos.o:
	$(CC) -c $(SRC_DIR)/leitura_arquivos.c -o $(BIN_DIR)/leitura_arquivos.o -Wall

escrever_arquivos.o:
	$(CC) -c $(SRC_DIR)/escrever_arquivos.c -o $(BIN_DIR)/escrever_arquivos.o -Wall
