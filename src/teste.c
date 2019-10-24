#include "../include/apidisk.h"
#include "../include/string.h"
#include <stdio.h>
#include <stdlib.h>

int converteUnsignedCharToInt(unsigned char *valor){
    int resultado;
    unsigned char *ponteiro = &resultado;
    int i;


    for(i=0; i<4; i++){
      ponteiro[i] = valor[i];
    }

    return resultado;
}

int main(){

  unsigned char         buffer[256] ;
  unsigned char           versao[2] ;
  unsigned char        tam_setor[2] ;
  unsigned char     byte_inicial[2] ;
  unsigned char  quant_particoes[2] ;

  unsigned char     inicio_part0[4] ;
  unsigned char        fim_part0[4] ;
  unsigned char       nome_part0[24];

  unsigned char     inicio_part1[4] ;
  unsigned char        fim_part1[4] ;
  unsigned char       nome_part1[24];

  unsigned char     inicio_part2[4] ;
  unsigned char        fim_part2[4] ;
  unsigned char       nome_part2[24];

  unsigned char     inicio_part3[4] ;
  unsigned char        fim_part3[4] ;
  unsigned char       nome_part3[24];

  read_sector(0, buffer);

  copiarMemoria(versao,           &buffer[0] , 2);
  copiarMemoria(tam_setor,        &buffer[2] , 2);
  copiarMemoria(byte_inicial,     &buffer[4] , 2);
  copiarMemoria(quant_particoes,  &buffer[6] , 2);

  copiarMemoria(inicio_part0,     &buffer[8] , 4);
  copiarMemoria(fim_part0,        &buffer[12], 4);
  copiarMemoria(nome_part0,       &buffer[16], 24);

  copiarMemoria(inicio_part1,     &buffer[40] , 4);
  copiarMemoria(fim_part1,        &buffer[44], 4);
  copiarMemoria(nome_part1,       &buffer[48], 24);

  copiarMemoria(inicio_part2,     &buffer[72] , 4);
  copiarMemoria(fim_part2,        &buffer[76], 4);
  copiarMemoria(nome_part2,       &buffer[80], 24);

  copiarMemoria(inicio_part3,     &buffer[104] , 4);
  copiarMemoria(fim_part3,        &buffer[108], 4);
  copiarMemoria(nome_part3,       &buffer[112], 24);

  converteParaBigEndian(versao, 2);
  converteParaBigEndian(tam_setor, 2);
  converteParaBigEndian(byte_inicial, 2);
  converteParaBigEndian(quant_particoes, 2);

  converte(inicio_part0, 4);
  converte(fim_part0, 4);
  converte(inicio_part1, 4);
  converte(fim_part1, 4);
  converte(inicio_part2, 4);
  converte(fim_part2, 4);
  converte(inicio_part3, 4);
  converte(fim_part3, 4);


  int inicio0 = converteUnsignedCharToInt(inicio_part0);
  int fim0 = converteUnsignedCharToInt(fim_part0);
  int inicio1 = converteUnsignedCharToInt(inicio_part1);
  int fim1 = converteUnsignedCharToInt(fim_part1);
  int inicio2 = converteUnsignedCharToInt(inicio_part2);
  int fim2 = converteUnsignedCharToInt(fim_part2);
  int inicio3 = converteUnsignedCharToInt(inicio_part3);
  int fim3 = converteUnsignedCharToInt(fim_part3);


  int a;
  unsigned char *pointer = &a;

  pointer[0] = 0;
  pointer[1] = 80;
  pointer[2] = 0;
  pointer[3] = 0;

  unsigned char *ptr;

  printf("---------------------------------------------------------------\n");
  printf("Versao: ");           printHexa(versao, 2);           printf("\n");
  printf("Tamanho: ");          printHexa(tam_setor, 2);        printf("\n");
  printf("Byte inicial: ");     printHexa(byte_inicial, 2);     printf("\n");
  printf("Quantidade part: ");  printHexa(quant_particoes, 2);  printf("\n");
  printf("---------------------------------------------------------------\n");
  printf("Nome part0: ");  printf("%s", nome_part0);       printf("\n");

  printf("Inicio part0: %d||", binToDec(inicio_part0));     printHexa(inicio_part0, 4);  printf("\n");
  printf("Fim    part0: %d||", binToDec(fim_part0));     printHexa(fim_part0, 4);  printf("\n");
  printf("\n---------------------------------------------------------------\n");
  printf("Nome part1: ");       printf("%s", nome_part1);       printf("\n");
  printf("Inicio part1: %d||",  binToDec(inicio_part1));     printHexa(inicio_part1, 4);  printf("\n");
  printf("Fim    part1: %d||",  binToDec(fim_part1));     printHexa(fim_part1, 4);  printf("\n");
  printf("---------------------------------------------------------------\n");
  printf("Nome part2: ");       printf("%s", nome_part2);       printf("\n");
  printf("Inicio part2: %d||",  binToDec(inicio_part2));     printHexa(inicio_part2, 4);  printf("\n");
  printf("Fim    part2: %d||",  binToDec(fim_part2));     printHexa(fim_part2, 4);  printf("\n");
  printf("---------------------------------------------------------------\n");
  printf("Nome part3: ");       printf("%s", nome_part3);       printf("\n");
  printf("Inicio part3: %d||",  binToDec(inicio_part3));     printHexa(inicio_part3, 4);  printf("\n");
  printf("Fim    part3: %d||",  binToDec(fim_part3));     printHexa(fim_part3, 4);  printf("\n");
  printf("---------------------------------------------------------------\n");

  return 0;
}
