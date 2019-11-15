#include "../include/apidisk.h"
#include "../include/string.h"
#include "../include/bitmap2.h"

#include <stdio.h>
#include <stdlib.h>

//0x7E32 em inteiro
#define VERSAO 32306
#define FALHA -1
#define SUCESSO 0
int tamanho_setor;


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
} Particao;

Particao particoes[4];
SuperBloco super_bloco_atual;

int retornaSetorDoSuperBloco(int numero_particao){
  return particoes[numero_particao].posicao_inicio;
}

int geraSuperBlocoESalva(int numero_particao, int setores_por_bloco){

  int tamanhoSetor            = tamanho_setor;
  int setoresPorBloco         = setores_por_bloco;
  int blocosParaSuperBloco    = 1;
  int bitsPorSetor            = tamanhoSetor * 8;
  int bitsPorBloco            = setoresPorBloco * bitsPorSetor;

  int tamanhoParticaoEmSetores  = particoes[numero_particao].posicao_fim - particoes[numero_particao].posicao_inicio;
  int numeroBlocosNaParticao    = tamanhoParticaoEmSetores / setoresPorBloco;
  // 10% para blocos de inode
  int areaInodeEmBlocos         = (numeroBlocosNaParticao / 10);
  if(numeroBlocosNaParticao % 10 > 0)
    areaInodeEmBlocos++;

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
  areaDadosEmBlocos                        = (int) areaDadosEmBlocos - blocosParaBitMapDados;

  unsigned char* ponteiroAuxiliar;

  unsigned char  bloco[256];
  unsigned char  id[4];
  int            versao           = 32306;
  int            superblockSize   = 1;
  int            blocosParticao   = (particoes[numero_particao].posicao_fim - particoes[numero_particao].posicao_inicio)/setoresPorBloco;

  id[0] = 'T';id[1] = '2'; id[2] = 'F'; id[3] = 'S';
  copiarMemoria((char*) bloco, (char*) id, 4);

  //converteParaBigEndian((unsigned char*) &versao, 2);
  ponteiroAuxiliar = (unsigned char*) &versao;
  copiarMemoria((char*) &bloco[4], (char*) ponteiroAuxiliar, 2);
  ponteiroAuxiliar = (unsigned char*) &superblockSize;
  copiarMemoria((char*) &bloco[6], (char*) ponteiroAuxiliar, 2);
  ponteiroAuxiliar = (unsigned char*) &blocosParaBitMapDados;
  copiarMemoria((char*) &bloco[8], (char*) ponteiroAuxiliar, 2);
  ponteiroAuxiliar = (unsigned char*) &blocosParaBitMapInode;
  copiarMemoria((char*) &bloco[10], (char*) ponteiroAuxiliar, 2);
  ponteiroAuxiliar = (unsigned char*) &areaInodeEmBlocos;
  copiarMemoria((char*) &bloco[12], (char*) ponteiroAuxiliar, 2);
  ponteiroAuxiliar = (unsigned char*) &setoresPorBloco;
  copiarMemoria((char*) &bloco[14], (char*) ponteiroAuxiliar, 2);
  ponteiroAuxiliar = (unsigned char*) &blocosParticao;
  copiarMemoria((char*) &bloco[16], (char*) ponteiroAuxiliar, 4);
  return write_sector (retornaSetorDoSuperBloco(numero_particao), bloco);
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

void le_MBR_Preenche_Dados_Particoes(){
  unsigned char         buffer[256] ;

  read_sector(0, buffer);
  unsigned char        tam_setor[2] ;
  copiarMemoria((char*) tam_setor,        (char*)  &buffer[2] , 2);
  tamanho_setor = converteDoisBytesParaInt(tam_setor);

  int index;
  for (index = 0; index < 4; index++){
    Particao particao;
    copiarMemoria((char*) &particao.posicao_inicio,    (char*) &buffer[8 + (index * 32)] , 4);
    copiarMemoria((char*) &particao.posicao_fim,       (char*) &buffer[12 + (index * 32)], 4);
    particoes[index] = particao;
  }
}

int formatarParticao(int numero_particao, int setores_por_bloco){
  SuperBloco super_bloco_sendo_formatado;

  geraSuperBlocoESalva(numero_particao, setores_por_bloco);
  leSetorEPreencheStructSuperBloco(&super_bloco_sendo_formatado, numero_particao);

  if(openBitmap2 (retornaSetorDoSuperBloco(numero_particao)) != 0){
    return FALHA;
  }

  int index;
  for(index = 0; index < super_bloco_sendo_formatado.diskSize; index++){
    if (setBitmap2 (BITMAP_DADOS, index, 0) != 0){
      return FALHA;
    }
  }

  for(index = 0; index< super_bloco_sendo_formatado.inodeAreaSize; index++){
    if (setBitmap2 (BITMAP_INODE, index, 0) != 0){
      return FALHA;
    }
  }
  return SUCESSO;
}
