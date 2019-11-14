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
LIB_DIR=../lib
INC_DIR=../include
BIN_DIR=../bin
SRC_DIR=../src

all: t2shell.o	string.o
	$(CC) -o executavel $(BIN_DIR)/teste.o $(BIN_DIR)/string.o $(LIB_DIR)/apidisk.o

t2shell.o:
	$(CC) -c ./t2shell.c -o  $(BIN_DIR)/t2shell.o -Wall

string.o:
	$(CC) -c $(SRC_DIR)/string.c -o $(BIN_DIR)/string.o -Wall
