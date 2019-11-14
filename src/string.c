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

void printBits(size_t const size, void const * const ptr){
    unsigned char *b = (unsigned char*) ptr;
    unsigned char byte;
    int i, j;



    for (i=0;i<size;i++)
    {
        for (j=7;j>=0;j--)
        {
            byte = (b[i] >> j) & 1;
            printf("%u", byte);
        }
        printf(" | ");
    }
    puts("");
}

int power(int base,int expoente){
  int resultado;
  if(expoente == 0)
    return 1;
  else{
    int i;
    resultado = base;
    for (i=1; i< expoente; i++){
      resultado = resultado * base;
    }
  }
  return resultado;

}

int binToDec(void const * const ptr){
    unsigned char *b = (unsigned char*) ptr;
    unsigned char byte;
    int i, j, k=0, aux = 0, size = 4;

    int resultado = 0;

    for (i=size-1;i>=0;i--)
    {
        for (j=0;j<8;j++)
        {
            byte = (b[i] >> j) & 1;
            aux = byte * power(2,k);
            resultado = resultado + aux;
            k++;
        }
    }
    return resultado;
}

void converteParaBigEndian(unsigned char *valor, int size){
  int i;
  unsigned char aux;
  size = size -1;
  for (i=0; i <= (size/2); i++){
      aux = valor[i];
      valor[i] = valor[size-i];
      valor[size-i] = aux;
  }

}

void converte(unsigned char *valor, int size){
  unsigned char aux;
  aux = valor[2];
  valor[2] = valor[1];
  valor[3] = valor[0];
  valor[0] = aux;
  valor[1] = aux;
}


int converteDoisBytesParaInt(unsigned char *ponteiro){
  unsigned char* ponteiroAuxiliar;
  int valorConvertido = 0;
  ponteiroAuxiliar = (unsigned char*) &valorConvertido;
  ponteiroAuxiliar[0] = ponteiro[0];
  ponteiroAuxiliar[1] = ponteiro[1];
  return valorConvertido;
}

void printHexa(unsigned char *hex, int size){
  int i, flag = 0;
  printf("(0x");
  for (i=0;i<size; i++){
    if(hex[i] == 0 && flag ==0)
      continue;

    flag  = 1;
    printf("%02X",hex[i]);
  }
  printf(")");
}
