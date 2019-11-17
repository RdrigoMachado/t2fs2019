#ifndef __LER_ESCREVER___
#define __LER_ESCREVER___

#include <stdio.h>
#include <stdlib.h>

#include "../include/apidisk.h"
#include "../include/bitmap2.h"
#include "../include/t2disk.h"
#include "../include/string.h"


//0x7E32 em inteiro
#define VERSAO 32306
#define FALHA -1
#define SUCESSO 0
#define PONTEIRO_EM_BYTES 4
#define MAXIMO_ARQUIVOS_ABERTOS 10

int tamanho_setor;
int maior_bloco_caso_1;
int maior_bloco_caso_2;
int maior_bloco_caso_3;
int ponteiros_por_bloco;
int bytes_bloco;
int inicioAreaDados;
int inicioAreaInodes;

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


typedef struct handle{
	struct t2fs_inode arquivo;
	int posicao_atual;
}Handle;



Handle arquivos[MAXIMO_ARQUIVOS_ABERTOS];

Particao particoes[4];
SuperBloco super_bloco_atual;

int retornaSetorDoSuperBloco(int numero_particao);
int geraSuperBlocoESalva(int numero_particao, int setores_por_bloco);
void leSetorEPreencheStructSuperBloco(SuperBloco *super_bloco, int numero_particao);
void le_MBR_Preenche_Dados_Particoes();
int formatarParticao(int numero_particao, int setores_por_bloco);
int leitura_arquivo(unsigned char* buffer, int bytes_a_serem_lidos, Handle* handle);
#endif
