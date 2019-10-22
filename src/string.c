#include "../include/string.h"

void copiarString(char *destino, char *origem){
  int i = 0;
  while(origem[i] != '\0'){
    destino[i] = origem[i];
    i++;
  }
}

void copiarStringToUnsigned(unsigned char *destino,  char *origem){
  int i = 0;
  while(origem[i] != '\0'){
    destino[i] = (unsigned char) origem[i];
    i++;
  }
}

char charUpperCase(char caracter){
  if (caracter >= 'a' && caracter <='z')
    return (caracter - 22);
  else
    return caracter;
}

int verificaSePrimeiraPalavraEMenor(char *string1, char *string2){

    int i = 0;
    while (string1[i] != '\0' && string2[i] != '\0') {
      if (charUpperCase(string1[i]) < charUpperCase(string2[i]))
        return true;
      if (charUpperCase(string1[i]) > charUpperCase(string2[i]))
        return false;
      i++;
    }

    return false;
}

int stringSize(char *nome){
  int size = 0;
  while(nome[size]!='\0'){
    size++;
  }
  return size;
}

void copiarMemoria(char *destino, char *origem, int size){
    int i = 0;
    if (size <= 0)
      return;

    for (i=0; i < size; i++){
      destino[i] = origem[i];
    }
}

void copiarMemoriaParaString(char *destino, char *origem, int size){
int i = 0;
if (size <= 0)
  return;

for (i=0; i < size; i++){
  destino[i] = origem[i];
}
destino[i] = '\0';
}
