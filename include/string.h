#ifndef __STRING___
#define __STRING___

#include <stdlib.h>
#include <stdio.h>
#define true  1
#define false 0

void copiarString(char *destino, char *origem);
void copiarStringToUnsigned(unsigned char *destino,  char *origem);

char charUpperCase(char caracter);
int  verificaSePrimeiraPalavraEMenor(char *string1, char *string2);
int  stringSize(char *nome);
void copiarMemoria(char *destino, char *origem, int size);
void copiarMemoriaParaString(char *destino, char *origem, int size);
void converteParaBigEndian(unsigned char *valor, int size);
void printBits(size_t const size, void const * const ptr);
void printHexa(unsigned char *hex, int size);
void converte(unsigned char *valor, int size);
int  converteDoisBytesParaInt(unsigned char *ponteiro);
int  binToDec(void const * const ptr);
#endif
