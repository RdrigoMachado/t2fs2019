#include "../include/ler_e_escrever_no_disco.h"

void calcular_limites_de_tipos_leitura(){
  int setores_no_bloco  = 1;
  bytes_bloco       = tamanho_setor    * setores_no_bloco;
  limite_caso_1         = bytes_bloco       * 2;
  int bytes_ind_simples = (bytes_bloco/4)   * bytes_bloco;
  limite_caso_2         = bytes_ind_simples + limite_caso_1;
  int bytes_ind_dupla   = bytes_ind_simples ^ 2;
  limite_caso_3         = bytes_ind_dupla   + limite_caso_1;
  maior_bloco_caso_1         = 2;
  maior_bloco_caso_2         = (bytes_bloco/4) + maior_bloco_caso_1;
  maior_bloco_caso_3         = ((bytes_bloco/4) * (bytes_bloco/4)) + maior_bloco_caso_2;
  ponteiros_por_bloco        = bytes_bloco / PONTEIRO_EM_BYTES;

}

void ler_bytes_setor_para_buffer(unsigned char* buffer, int numero_setor, int bytes_a_serem_lidos, int* bytes_lidos, int deslocamento){
  unsigned char setor[tamanho_setor];
  read_sector(numero_setor, setor);
  int contador_bytes = deslocamento;

  while(contador_bytes < tamanho_setor && *bytes_lidos < bytes_a_serem_lidos){
    buffer[*bytes_lidos] = setor[contador_bytes];
    contador_bytes++;
    *bytes_lidos = *bytes_lidos + 1;
  }
}

void leitura_direta_bloco(unsigned char* buffer, int setor_inicio_bloco, int bytes_a_serem_lidos, int* bytes_lidos){
  int deslocamento_bytes     = (posicao_atual + *bytes_lidos) % tamanho_setor;
  int deslocamento_setores   = (posicao_atual + *bytes_lidos) / tamanho_setor;
  int setor_a_ser_lido       = setor_inicio_bloco + deslocamento_setores;
  int byte_relativo_ao_bloco = deslocamento_bytes + deslocamento_setores* tamanho_setor;

  while(*bytes_lidos < bytes_a_serem_lidos && byte_relativo_ao_bloco < bytes_bloco){
    ler_bytes_setor_para_buffer(buffer, setor_a_ser_lido, bytes_a_serem_lidos, bytes_lidos, deslocamento_bytes);
    deslocamento_bytes = 0;
    setor_a_ser_lido++;
    byte_relativo_ao_bloco++;
  }
}

int retornaSetorParaLeituraDoBloco(int bloco_a_ser_lido, struct t2fs_inode arquivo){
  int ponteiro = -1;
  if(bloco_a_ser_lido >= 0){
    if(bloco_a_ser_lido < maior_bloco_caso_1){
      ponteiro = arquivo.dataPtr[bloco_a_ser_lido];
    }
    else if(bloco_a_ser_lido < maior_bloco_caso_2){
      unsigned char buffer[tamanho_setor];
      int bloco_dado = bloco_a_ser_lido - maior_bloco_caso_1;
      read_sector(arquivo.singleIndPtr, buffer);
      copiarMemoria((char*) &ponteiro, (char*) &buffer[bloco_dado * PONTEIRO_EM_BYTES] , PONTEIRO_EM_BYTES);
    }
    else if(bloco_a_ser_lido < maior_bloco_caso_3){
      unsigned char buffer_indireto[tamanho_setor];
      unsigned char buffer[tamanho_setor];
      int bloco_indireto = bloco_a_ser_lido / ponteiros_por_bloco;
      int bloco_dado     = bloco_a_ser_lido % ponteiros_por_bloco;
      int ponteiro_bloco_dado;
      //le bloco de indices que aponta para outros blocos de indices
      read_sector(arquivo.doubleIndPtr, buffer_indireto);
      //le ponteiro para bloco
      copiarMemoria((char*) &ponteiro_bloco_dado, (char*) &buffer_indireto[bloco_indireto * PONTEIRO_EM_BYTES] , PONTEIRO_EM_BYTES);
      read_sector(ponteiro_bloco_dado, buffer);
      copiarMemoria((char*) &ponteiro, (char*) &buffer[bloco_dado * PONTEIRO_EM_BYTES] , PONTEIRO_EM_BYTES);
    }
  }
  return ponteiro;
}

int leitura_arquivo(unsigned char* buffer, int bytes_a_serem_lidos, Handle handle){
  int bytes_lidos = 0;
  int bloco_a_ser_lido = (handle.posicao_atual + bytes_lidos) / bytes_bloco;
  int inicio_bloco;
  while(bytes_lidos < bytes_a_serem_lidos){
    inicio_bloco = retornaSetorParaLeituraDoBloco(bloco_a_ser_lido, handle.arquivo);
    if(inicio_bloco == -1){
      return FALHA;
    }
    leitura_direta_bloco(buffer,inicio_bloco,bytes_a_serem_lidos, &bytes_lidos);
    bloco_a_ser_lido = (posicao_atual + bytes_lidos) / bytes_bloco;
  }
  posicao_atual = posicao_atual + bytes_lidos;
  return SUCESSO;
}

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
  calcular_limites_de_tipos_leitura();
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

/*
int main(){
le_MBR_Preenche_Dados_Particoes();
  int index;
  for(index = 0; index<4; index++){
    formatarParticao(index, 4);
    leSetorEPreencheStructSuperBloco(&super_bloco_atual, index);
    printf("=====================================\n");
    printf("IHEEEEEEEEEE\n");
    printf("=====================================\n");
  }



  return 0;
}*/
