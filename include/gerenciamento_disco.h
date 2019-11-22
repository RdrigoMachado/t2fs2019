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
int tamanho_bloco_em_setores;
int particao_ativa;

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
  int posicao_area_inodes;
  int posicao_area_dados;
  int blocos_para_bitmap_dados;
  int blocos_para_bitmap_inodes;
  int area_inode_em_blocos;
  int area_dado_em_blocos;
  int blocos_disco;
  int tamanho_bloco_em_setores;
} Particao;


typedef struct handle{
	struct t2fs_inode arquivo;
  struct t2fs_record entrada;
	int posicao_atual;
	FILE2 identificador;
  int valido;
}Handle;



Handle arquivos[MAXIMO_ARQUIVOS_ABERTOS];
int arquivos_abertos;
int ultimo_id_utilizado;




Particao particoes[4];
SuperBloco super_bloco_atual;

int retornaSetorDoSuperBloco(int numero_particao);
int geraSuperBlocoESalva(int numero_particao, int setores_por_bloco);
void carregaDadosParticao(SuperBloco *super_bloco, int numero_particao);
void carregaDadosDisco();
int formatarParticao(int numero_particao, int setores_por_bloco);
#endif
