#include "../include/apidisk.h"
#include "../include/string.h"
#include <stdio.h>
#include <stdlib.h>

//0x7E32 em inteiro
#define VERSAO 32306

typedef struct particao{
  int posicao_inicio;
  int posicao_final;
  int bitmap_inodes;
  int bitmap_dados;
  int area_inodes;
  int area_dados;
}Particao;

typedef struct superbloco{
  unsigned char id[4];
  int versao;
  int superblockSize;
  int freeBlocksBitmapSize;
  int freeInodeBitmapSize;
  int inodeAreaSize;
  int blockSize;
  int diskSize;
  unsigned char Checksum[4];
  unsigned char reservado[232];
}SuperBloco;

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

  printf("------------------------------------------------------------\n");
  printf("Versao: ");          printHexa(versao, 2);          printf("\n");
  printf("Tamanho: ");         printHexa(tam_setor, 2);       printf("\n");
  printf("Byte inicial: ");    printHexa(byte_inicial, 2);    printf("\n");
  printf("Quantidade part: "); printHexa(quant_particoes, 2); printf("\n");

  for (index = 0; index < 4; index++){
    printf("---------------------------------------------------------------\n");
    printf("--Particao %d--\n", index);
    printf("Inicio       : %d\n", particoes[index].posicao_inicio);
    printf("Fim          : %d\n", particoes[index].posicao_final );
    printf("Blocos Inode : %d\n", particoes[index].area_inodes   );
    printf("Blocos Dados : %d\n", particoes[index].area_dados    );
    printf("Bitmap Inodes: %d\n", particoes[index].bitmap_inodes );
    printf("Bitmap Dados : %d\n", particoes[index].bitmap_dados  );


    //Preencher SuperBloco------------------------------------------------------
    unsigned char  bloco[256];
    unsigned char  id[4];
    unsigned char* ponteiroAuxiliar;
    int            versao           = 32306;
    int            superblockSize   = 1;
    int            blocosParticao   = (particoes[index].posicao_final - particoes[index].posicao_inicio)/setoresPorBloco;

    id[0] = 'T';id[1] = '2'; id[2] = 'F'; id[3] = 'S';
    copiarMemoria((char*) bloco, (char*) id, 4);

    //converteParaBigEndian((unsigned char*) &versao, 2);
    ponteiroAuxiliar = (unsigned char*) &versao;
    copiarMemoria((char*) &bloco[4], (char*) ponteiroAuxiliar, 2);

    ponteiroAuxiliar = (unsigned char*) &superblockSize;
    copiarMemoria((char*) &bloco[6], (char*) ponteiroAuxiliar, 2);

    ponteiroAuxiliar = (unsigned char*) &particoes[index].bitmap_dados;
    copiarMemoria((char*) &bloco[8], (char*) ponteiroAuxiliar, 2);

    ponteiroAuxiliar = (unsigned char*) &particoes[index].bitmap_inodes;
    copiarMemoria((char*) &bloco[10], (char*) ponteiroAuxiliar, 2);

    ponteiroAuxiliar = (unsigned char*) &particoes[index].area_inodes;
    copiarMemoria((char*) &bloco[12], (char*) ponteiroAuxiliar, 2);

    ponteiroAuxiliar = (unsigned char*) &setoresPorBloco;
    copiarMemoria((char*) &bloco[14], (char*) ponteiroAuxiliar, 2);

    ponteiroAuxiliar = (unsigned char*) &blocosParticao;
    copiarMemoria((char*) &bloco[16], (char*) ponteiroAuxiliar, 4);


    // recuperar e printar superbloco
    printf("\n--SuperBloco--\n");
    SuperBloco superbloco;
    copiarMemoria((char*) &(superbloco.id), (char*) &bloco, 4);
    printf("ID: ");
    int x;
    for(x=0; x<4; x++){
      printf("%c", superbloco.id[x]);
    }

    superbloco.versao               = converteDoisBytesParaInt((unsigned char*) &bloco[4]);
    printf("\nVersao: %X\n", superbloco.versao);

    superbloco.superblockSize       = converteDoisBytesParaInt((unsigned char*) &bloco[6]);
    printf("Tamanho SuperBloco: %d\n", superbloco.superblockSize);

    superbloco.freeBlocksBitmapSize = converteDoisBytesParaInt((unsigned char*) &bloco[8]);
    printf("Tamanho Bitmap Dados: %d\n", superbloco.freeBlocksBitmapSize);

    superbloco.freeInodeBitmapSize  = converteDoisBytesParaInt((unsigned char*) &bloco[10]);
    printf("Tamanho Bitmap Inodes: %d\n", superbloco.freeInodeBitmapSize);

    superbloco.inodeAreaSize        = converteDoisBytesParaInt((unsigned char*) &bloco[12]);
    printf("Tamanho Area Inodes: %d\n", superbloco.inodeAreaSize);

    superbloco.blockSize            = converteDoisBytesParaInt((unsigned char*) &bloco[14]);
    printf("Tamanho do Bloco: %d\n", superbloco.blockSize);

    superbloco.diskSize = converteDoisBytesParaInt((unsigned char*) &bloco[16]);
    printf("Tamanho Disco em Blocos: %d\n", superbloco.diskSize);


  }

  return 0;
}
