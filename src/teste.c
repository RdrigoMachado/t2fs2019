#include "../include/apidisk.h"
#include "../include/string.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct particao{
  int posicao_inicio;
  int posicao_final;
}Particao;

Particao particoes[4];

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

  copiarMemoria((char*) versao,           (char*)  &buffer[0] , 2);
  copiarMemoria((char*) tam_setor,        (char*)  &buffer[2] , 2);
  copiarMemoria((char*) byte_inicial,     (char*)  &buffer[4] , 2);
  copiarMemoria((char*) quant_particoes,  (char*)  &buffer[6] , 2);

  copiarMemoria((char*) inicio_part0,     (char*) &buffer[8] , 4);
  copiarMemoria((char*) fim_part0,        (char*) &buffer[12], 4);
  copiarMemoria((char*) nome_part0,       (char*) &buffer[16], 24);

  copiarMemoria((char*) inicio_part1,     (char*) &buffer[40] , 4);
  copiarMemoria((char*) fim_part1,        (char*) &buffer[44], 4);
  copiarMemoria((char*) nome_part1,       (char*) &buffer[48], 24);

  copiarMemoria((char*) inicio_part2,     (char*) &buffer[72] , 4);
  copiarMemoria((char*) fim_part2,        (char*) &buffer[76], 4);
  copiarMemoria((char*) nome_part2,       (char*) &buffer[80], 24);

  copiarMemoria((char*) inicio_part3,     (char*) &buffer[104] , 4);
  copiarMemoria((char*) fim_part3,        (char*) &buffer[108], 4);
  copiarMemoria((char*) nome_part3,       (char*)  &buffer[112], 24);

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

  particoes[0].posicao_inicio = binToDec(inicio_part0);
  particoes[0].posicao_final  = binToDec(fim_part0);

  particoes[1].posicao_inicio = binToDec(inicio_part1);
  particoes[1].posicao_final  = binToDec(fim_part1);

  particoes[2].posicao_inicio = binToDec(inicio_part2);
  particoes[2].posicao_final  = binToDec(fim_part2);

  particoes[3].posicao_inicio = binToDec(inicio_part3);
  particoes[3].posicao_final  = binToDec(fim_part3);

  printf("---------------------------------------------------------------\n");
  printf("Versao: ");                                           printHexa(versao, 2);          printf("\n");
  printf("Tamanho: ");                                          printHexa(tam_setor, 2);       printf("\n");
  printf("Byte inicial: ");                                     printHexa(byte_inicial, 2);    printf("\n");
  printf("Quantidade part: ");                                  printHexa(quant_particoes, 2); printf("\n");
  printf("---------------------------------------------------------------\n");
  printf("Nome part0: ");                                        printf("%s", nome_part0);    printf("\n");
  printf("Inicio part0: %d ", particoes[0].posicao_inicio);      printHexa(inicio_part0, 4);  printf("\n");
  printf("Fim    part0: %d ", particoes[0].posicao_final);       printHexa(fim_part0, 4);     printf("\n");
  printf("---------------------------------------------------------------\n");
  printf("Nome part1: ");       printf("%s", nome_part1);        printf("\n");
  printf("Inicio part1: %d ",  particoes[1].posicao_inicio);     printHexa(inicio_part1, 4);  printf("\n");
  printf("Fim    part1: %d ",  particoes[1].posicao_final);      printHexa(fim_part1, 4);     printf("\n");
  printf("---------------------------------------------------------------\n");
  printf("Nome part2: ");       printf("%s", nome_part2);        printf("\n");
  printf("Inicio part2: %d ",  particoes[2].posicao_inicio);     printHexa(inicio_part2, 4);  printf("\n");
  printf("Fim    part2: %d ",  particoes[2].posicao_final);      printHexa(fim_part2, 4);     printf("\n");
  printf("---------------------------------------------------------------\n");
  printf("Nome part3: ");       printf("%s", nome_part3);        printf("\n");
  printf("Inicio part3: %d ",  particoes[3].posicao_inicio);     printHexa(inicio_part3, 4);  printf("\n");
  printf("Fim    part3: %d ",  particoes[3].posicao_final);      printHexa(fim_part3, 4);     printf("\n");
  printf("---------------------------------------------------------------\n");

  return 0;
}
