#include "../include/apidisk.h"
#include "../include/string.h"
#include "../include/bitmap2.h"
#include "../include/ler_e_escrever_no_disco.h"

#include <stdio.h>
#include <stdlib.h>

<<<<<<< Updated upstream
int main(){

  int index;
  for(index = 0; index<4; index++){
    formatarParticao(index, 4);
    leSetorEPreencheStructSuperBloco(&super_bloco_atual, index);

    printf("=====================================\n");
    printf("=====================================\n");
  }


=======
//0x7E32 em inteiro
#define VERSAO 32306


typedef struct superbloco{
  unsigned char id[4];
  int           versao;
  int           superblockSize;
  int           freeBlocksBitmapSize;
  int           freeInodeBitmapSize;
  int           inodeAreaSize;
  int           blockSize;
  int           diskSize;
  unsigned char Checksum[4];
  unsigned char reservado[232];
} SuperBloco;

typedef struct particao{
  int posicao_inicio;
  int posicao_fim;
  int tamanho_setor;
} Particao;

Particao particoes[4];

void recuperaMBREPreencheStructsDeParticao(){
  unsigned char         buffer[256] ;
  read_sector(0, buffer);

  int index;
  for (index = 0; index < 4; index++){
    Particao particao;
    copiarMemoria((char*) &particao.tamanho_setor,     (char*) &buffer[2] , 2);
    copiarMemoria((char*) &particao.posicao_inicio,(char*) &buffer[8] , 4);
    copiarMemoria((char*) &particao.posicao_fim, (char*) &buffer[12], 4);
    converteParaBigEndian((unsigned char *) &particao.tamanho_setor, 2);
    converte((unsigned char*) &particao.posicao_inicio, 4);
    converte((unsigned char*) &particao.posicao_fim, 4);
    particoes[index] = particao;
  }
}

int retornaSetorDoSuperBloco(int numero_particao){
  return particoes[numero_particao].posicao_inicio;
}

void leSetorEPreencheStructSuperBloco(SuperBloco *super_bloco, int numero_particao){

  unsigned char  buffer_super_bloco[256];
  int setor_do_super_bloco = retornaSetorDoSuperBloco(numero_particao);
  read_sector(setor_do_super_bloco, buffer_super_bloco);

  copiarMemoria((char*) &super_bloco->id, (char*) &buffer_super_bloco, 4);

  super_bloco->versao                = converteDoisBytesParaInt((unsigned char*) &buffer_super_bloco[4]);
  super_bloco->superblockSize        = converteDoisBytesParaInt((unsigned char*) &buffer_super_bloco[6]);
  super_bloco->freeBlocksBitmapSize  = converteDoisBytesParaInt((unsigned char*) &buffer_super_bloco[8]);
  super_bloco->freeInodeBitmapSize   = converteDoisBytesParaInt((unsigned char*) &buffer_super_bloco[10]);
  super_bloco->inodeAreaSize         = converteDoisBytesParaInt((unsigned char*) &buffer_super_bloco[12]);
  super_bloco->blockSize             = converteDoisBytesParaInt((unsigned char*) &buffer_super_bloco[14]);
  super_bloco->diskSize              = converteDoisBytesParaInt((unsigned char*) &buffer_super_bloco[16]);
}

void printSuperBloco(SuperBloco super_bloco){
    printf("---------------------------------------------------------------\n");
    printf("--Super Bloco--\n");
    printf("Id: ");
    int index;
    for (index=0; index<4;index++)
      printf("%c", super_bloco.id[index]);

    printf("\n");

    printf("Versao: %X\n", super_bloco.versao);
    printf("SuperBlock Size: %d\n", super_bloco.superblockSize);
    printf("freeBlocksBitmapSize: %d\n", super_bloco.freeBlocksBitmapSize);
    printf("freeInodeBitmapSize: %d\n", super_bloco.freeInodeBitmapSize);
    printf("inodeAreaSize: %d\n", super_bloco.inodeAreaSize);
    printf("blockSize: %d\n", super_bloco.blockSize);
    printf("diskSize: %d\n", super_bloco.diskSize);

}


int main(){

  recuperaMBREPreencheStructsDeParticao();

  printf("Tamanho Setor: %d  \n",particoes[0].tamanho_setor );

  int index;
  for (index = 0; index < 4; index++){
    printf("---------------------------------------------------------------\n");
    printf("--Particao %d--\n", index);
    SuperBloco super_bloco ;
    leSetorEPreencheStructSuperBloco(&super_bloco, index);
    printSuperBloco(super_bloco);
  }
>>>>>>> Stashed changes


  return 0;
}
