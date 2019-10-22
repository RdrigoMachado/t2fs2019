#include "../include/apidisk.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../include/string.h"

void printBits(size_t const size, void const * const ptr)
{
    unsigned char *b = (unsigned char*) ptr;
    unsigned char byte;
    int i, j;

    for (i=size-1;i>=0;i--)
    {
        for (j=7;j>=0;j--)
        {
            byte = (b[i] >> j) & 1;
            printf("%u", byte);
        }
    }
    puts("");
}

int main(){

  unsigned char buffer[256] ;
  unsigned char versao[4] ;
  read_sector (0, buffer);
  copiarMemoria(versao, &buffer[15], 4);



    printBits(sizeof(unsigned char), &versao[0]);
    printBits(sizeof(unsigned char), &versao[1]);
    printBits(sizeof(unsigned char), &versao[2]);
    printBits(sizeof(unsigned char), &versao[3]);
    int tamanho;
    unsigned char* temp;
    temp = &tamanho;
    temp[0] = versao[3];
    temp[1] = versao[2];
    temp[2] = versao[1];
    temp[3] = versao[0];
  printf("%d",tamanho);
  printf("\nAAAAHHHHHHH\n");


  return 0;
}
