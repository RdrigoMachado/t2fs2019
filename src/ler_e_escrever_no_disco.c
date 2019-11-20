#include "../include/ler_e_escrever_no_disco.h"

int retornaSetorDoSuperBloco(int numero_particao){
  return particoes[numero_particao].posicao_inicio;
}

void carregaDadosDisco(){
  unsigned char buffer[256] ;
  unsigned char tam_setor[2] ;

  read_sector(0, buffer);
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

void init(int numero_particao){

  bytes_bloco         = tamanho_setor    * particoes[numero_particao].tamanho_bloco_em_setores;
  maior_bloco_caso_1  = 2;
  maior_bloco_caso_2  = (bytes_bloco/particoes[numero_particao].tamanho_bloco_em_setores) + maior_bloco_caso_1;
  maior_bloco_caso_3  = ((bytes_bloco/particoes[numero_particao].tamanho_bloco_em_setores) * (bytes_bloco/particoes[numero_particao].tamanho_bloco_em_setores)) + maior_bloco_caso_2;
  ponteiros_por_bloco = bytes_bloco / PONTEIRO_EM_BYTES;

  int tamanhoSetor            = tamanho_setor;
  int blocosParaSuperBloco    = 1;
  int bitsPorSetor            = tamanhoSetor * 8;
  int bitsPorBloco            = particoes[numero_particao].tamanho_bloco_em_setores * bitsPorSetor;

  int tamanhoParticaoEmSetores  = particoes[numero_particao].posicao_fim - particoes[numero_particao].posicao_inicio;
  int numeroBlocosNaParticao    = tamanhoParticaoEmSetores / particoes[numero_particao].tamanho_bloco_em_setores;
  // 10% para blocos de inode
  int areaInodeEmBlocos         = (numeroBlocosNaParticao / 10);
  if(numeroBlocosNaParticao % 10 > 0)
    areaInodeEmBlocos++;

  // numero de blocos necessarios para o bit map do inode
  particoes[numero_particao].blocos_para_bitmap_inodes     = areaInodeEmBlocos / bitsPorBloco;
  if (areaInodeEmBlocos % bitsPorBloco > 0){
    particoes[numero_particao].blocos_para_bitmap_inodes++;
  }

  // numero de blocos designados para área de dados
  int areaDadosEmBlocos          = numeroBlocosNaParticao - (blocosParaSuperBloco + particoes[numero_particao].blocos_para_bitmap_inodes + areaInodeEmBlocos);
  // numero de blocos designados para área do bitmap de dados
  particoes[numero_particao].blocos_para_bitmap_dados      = areaDadosEmBlocos / bitsPorBloco;
  if (areaDadosEmBlocos % bitsPorBloco > 0){
    particoes[numero_particao].blocos_para_bitmap_dados++;
  }
  areaDadosEmBlocos                        = (int) areaDadosEmBlocos - particoes[numero_particao].blocos_para_bitmap_dados;

  particoes[numero_particao].posicao_area_inodes = particoes[numero_particao].posicao_inicio + blocosParaSuperBloco + particoes[numero_particao].blocos_para_bitmap_inodes + particoes[numero_particao].blocos_para_bitmap_dados;
  particoes[numero_particao].posicao_area_dados  = particoes[numero_particao].posicao_area_inodes + areaInodeEmBlocos;
  particoes[numero_particao].area_inode_em_blocos= areaInodeEmBlocos;
}

void carregaDadosParticao(SuperBloco *super_bloco, int numero_particao){

  particao_ativa = numero_particao;
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
  particoes[numero_particao].tamanho_bloco_em_setores = super_bloco->blockSize;
  //printf("block size %d\n", particoes[numero_particao].tamanho_bloco_em_setores);
  init(numero_particao);
}

int geraSuperBlocoESalva(int numero_particao, int setores_por_bloco){
  particoes[numero_particao].tamanho_bloco_em_setores = setores_por_bloco;
  init(numero_particao);

  unsigned char* ponteiroAuxiliar;
  unsigned char  bloco[256];
  unsigned char  id[4];
  int            versao           = 32306;
  int            superblockSize   = 1;
  particoes[numero_particao].blocos_disco   = (particoes[numero_particao].posicao_fim - particoes[numero_particao].posicao_inicio) / particoes[numero_particao].tamanho_bloco_em_setores;
  id[0] = 'T';id[1] = '2'; id[2] = 'F'; id[3] = 'S';
  copiarMemoria((char*) bloco, (char*) id, 4);

  //converteParaBigEndian((unsigned char*) &versao, 2);
  ponteiroAuxiliar = (unsigned char*) &versao;
  copiarMemoria((char*) &bloco[4], (char*) ponteiroAuxiliar, 2);
  ponteiroAuxiliar = (unsigned char*) &superblockSize;
  copiarMemoria((char*) &bloco[6], (char*) ponteiroAuxiliar, 2);
  ponteiroAuxiliar = (unsigned char*) &particoes[numero_particao].blocos_para_bitmap_dados;
  copiarMemoria((char*) &bloco[8], (char*) ponteiroAuxiliar, 2);
  ponteiroAuxiliar = (unsigned char*) &particoes[numero_particao].blocos_para_bitmap_inodes;
  copiarMemoria((char*) &bloco[10], (char*) ponteiroAuxiliar, 2);
  ponteiroAuxiliar = (unsigned char*) &particoes[numero_particao].area_inode_em_blocos;
  copiarMemoria((char*) &bloco[12], (char*) ponteiroAuxiliar, 2);
  ponteiroAuxiliar = (unsigned char*) &setores_por_bloco;
  copiarMemoria((char*) &bloco[14], (char*) ponteiroAuxiliar, 2);
  unsigned char a[2];
  copiarMemoria((char*) a, (char*) &bloco[14], 2);
  
  ponteiroAuxiliar = (unsigned char*) &particoes[numero_particao].blocos_disco;
  copiarMemoria((char*) &bloco[16], (char*) ponteiroAuxiliar, 4);

  return write_sector (retornaSetorDoSuperBloco(numero_particao), bloco);
}

int formatarParticao(int numero_particao, int setores_por_bloco){

  geraSuperBlocoESalva(numero_particao, setores_por_bloco);

  if(openBitmap2 (retornaSetorDoSuperBloco(numero_particao)) != 0){
    return FALHA;
  }

  int index;
  for(index = 0; index < particoes[numero_particao].blocos_disco; index++){
    if (setBitmap2 (BITMAP_DADOS, index, 0) != 0){
      return FALHA;
    }
  }

  for(index = 0; index< particoes[numero_particao].area_inode_em_blocos; index++){
    if (setBitmap2 (BITMAP_INODE, index, 0) != 0){
      return FALHA;
    }
  }
  return SUCESSO;
}

/*
int main(){
  le_MBR_Preenche_Dados_Particoes();
  formatarParticao(1, 4);
  leSetorEPreencheStructSuperBloco(&super_bloco_atual, 1);
  int posicao = retornaPosicaoLivreDeDadosEMarcaComoUsada();
  printf("==========  Primeira posicao livre ==================\n");
  printf("Posicao: %d\n", posicao);
  printf("==========  Primeira posicao livre apos ocupar========\n");
  posicao = retornaPosicaoLivreDeDadosEMarcaComoUsada();
  printf("Posicao: %d\n", posicao);
  return 0;
}*/
