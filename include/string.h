#ifndef __STRING___
#define __STRING___

#include <stdlib.h>
#define true  1
#define false 0

void copiarString(char *destino, char *origem);
void copiarStringToUnsigned(unsigned char *destino,  char *origem);

char charUpperCase(char caracter);
int verificaSePrimeiraPalavraEMenor(char *string1, char *string2);
int stringSize(char *nome);
void copiarMemoria(char *destino, char *origem, int size);
void copiarMemoriaParaString(char *destino, char *origem, int size);
#endif
