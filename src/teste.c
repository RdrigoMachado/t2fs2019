#include "../include/apidisk.h"
#include "../include/string.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct particao{
  int posicao_inicio;
  int posicao_final;
  int bitmap_inodes;
  int bitmap_dados;
  int area_inodes;
  int area_dados;
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

  int index;
  int setoresPorBloco         = 4;
  int blocosParaSuperBloco    = 1;
  int bitsPorSetor            = 256 * 8;
  int bitsPorBloco            = setoresPorBloco * bitsPorSetor;





  for (index = 0; index < 4; index++){

    int tamanhoParticaoEmSetores  = particoes[index].posicao_final - particoes[index].posicao_inicio;

    int numeroBlocosNaParticao    = tamanhoParticaoEmSetores / setoresPorBloco;

    // 10% para blocos de inode
    int areaInodeEmBlocos         = (int) (numeroBlocosNaParticao * 0.1) + ((numeroBlocosNaParticao % 10) > 0? 1: 0);

    // numero de blocos necessarios para o bit map do inode
    int blocosParaBitMapInode     = areaInodeEmBlocos / bitsPorBloco;
    if (areaInodeEmBlocos % bitsPorBloco > 0){
      blocosParaBitMapInode++;
    }

    // numero de blocos designados para área de dados
    int areaDadosEmBlocos          = numeroBlocosNaParticao - (blocosParaSuperBloco + blocosParaBitMapInode + areaInodeEmBlocos);

    // numero de blocos designados para área do bitmap de dados
    int blocosParaBitMapDados      = areaDadosEmBlocos / bitsPorBloco;
    if (areaDadosEmBlocos % bitsPorBloco > 0){
      blocosParaBitMapDados++;
    }

    areaDadosEmBlocos              = (int) areaDadosEmBlocos - blocosParaBitMapDados;

    particoes[index].bitmap_inodes =  blocosParaBitMapInode;
    particoes[index].bitmap_dados  =  blocosParaBitMapDados;
    particoes[index].area_inodes   =  areaInodeEmBlocos;
    particoes[index].area_dados    =  areaDadosEmBlocos;

 }

  printf("---------------------------------------------------------------\n");
  printf("Versao: ");                                           printHexa(versao, 2);          printf("\n");
  printf("Tamanho: ");                                          printHexa(tam_setor, 2);       printf("\n");
  printf("Byte inicial: ");                                     printHexa(byte_inicial, 2);    printf("\n");
  printf("Quantidade part: ");                                  printHexa(quant_particoes, 2); printf("\n");

  for (index = 0; index < 4; index++){
    printf("---------------------------------------------------------------\n");
    printf("Nome part%d\n", index);
    printf("Inicio       : %d\n", particoes[index].posicao_inicio);
    printf("Fim          : %d\n", particoes[index].posicao_final );
    printf("Blocos Inode : %d\n", particoes[index].area_inodes   );
    printf("Blocos Dados : %d\n", particoes[index].area_dados    );
    printf("Bitmap Inodes: %d\n", particoes[index].bitmap_inodes );
    printf("Bitmap Dados : %d\n", particoes[index].bitmap_dados  );
  }

  return 0;
}
