

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

void leitura_direta_bloco(unsigned char* buffer, int setor_inicio_bloco, int bytes_a_serem_lidos, int* bytes_lidos, Handle* handle ){
  int deslocamento_bytes     = (handle->posicao_atual + *bytes_lidos) % tamanho_setor;
  int deslocamento_setores   = (handle->posicao_atual + *bytes_lidos) / tamanho_setor;
  int setor_a_ser_lido       = setor_inicio_bloco + deslocamento_setores;
  int byte_relativo_ao_bloco = deslocamento_bytes + deslocamento_setores* tamanho_setor;

  while(*bytes_lidos < bytes_a_serem_lidos && byte_relativo_ao_bloco < bytes_bloco){
    ler_bytes_setor_para_buffer(buffer, setor_a_ser_lido, bytes_a_serem_lidos, bytes_lidos, deslocamento_bytes);
    deslocamento_bytes = 0;
    setor_a_ser_lido++;
    byte_relativo_ao_bloco++;
  }
}

int retornaSetorParaLeituraDoBloco(int bloco_a_ser_lido, Handle* handle){
  int ponteiro = -1;
  if(bloco_a_ser_lido >= 0){
    if(bloco_a_ser_lido < maior_bloco_caso_1){
      ponteiro = handle->arquivo.dataPtr[bloco_a_ser_lido];
    }
    else if(bloco_a_ser_lido < maior_bloco_caso_2){
      unsigned char buffer[tamanho_setor];
      int bloco_dado = bloco_a_ser_lido - maior_bloco_caso_1;
      read_sector(handle->arquivo.singleIndPtr, buffer);
      copiarMemoria((char*) &ponteiro, (char*) &buffer[bloco_dado * PONTEIRO_EM_BYTES] , PONTEIRO_EM_BYTES);
    }
    else if(bloco_a_ser_lido < maior_bloco_caso_3){
      unsigned char buffer_indireto[tamanho_setor];
      unsigned char buffer[tamanho_setor];
      int bloco_indireto = bloco_a_ser_lido / ponteiros_por_bloco;
      int bloco_dado     = bloco_a_ser_lido % ponteiros_por_bloco;
      int ponteiro_bloco_dado;
      //le bloco de indices que aponta para outros blocos de indices
      read_sector(handle->arquivo.doubleIndPtr, buffer_indireto);
      //le ponteiro para bloco
      copiarMemoria((char*) &ponteiro_bloco_dado, (char*) &buffer_indireto[bloco_indireto * PONTEIRO_EM_BYTES] , PONTEIRO_EM_BYTES);
      read_sector(ponteiro_bloco_dado, buffer);
      copiarMemoria((char*) &ponteiro, (char*) &buffer[bloco_dado * PONTEIRO_EM_BYTES] , PONTEIRO_EM_BYTES);
    }
  }
  return ponteiro;
}

int leitura_arquivo(unsigned char* buffer, int bytes_a_serem_lidos, Handle* handle){
  int bytes_lidos = 0;
  int bloco_a_ser_lido = (handle->posicao_atual + bytes_lidos) / bytes_bloco;
  int inicio_bloco;
  while(bytes_lidos < bytes_a_serem_lidos){
    inicio_bloco = retornaSetorParaLeituraDoBloco(bloco_a_ser_lido, handle);
    if(inicio_bloco == -1){
      return FALHA;
    }
    leitura_direta_bloco(buffer,inicio_bloco,bytes_a_serem_lidos, &bytes_lidos, handle);
    bloco_a_ser_lido = (handle->posicao_atual + bytes_lidos) / bytes_bloco;
  }
  handle->posicao_atual = handle->posicao_atual + bytes_lidos;
  return SUCESSO;
}



struct t2fs_inode {
	DWORD	blocksFileSize;
	DWORD	bytesFileSize;
	DWORD	dataPtr[2];
	DWORD	singleIndPtr;
	DWORD	doubleIndPtr;
	DWORD	RefCounter;
	DWORD	reservado;
};

int main(){

  le_MBR_Preenche_Dados_Particoes();
  formatarParticao(1, 4);
  leSetorEPreencheStructSuperBloco(&super_bloco_atual, 1);

  Handle handle;
  handle.arquivo.blocksFileSize = 4;
  handle.bytesFileSize = 264192;
  unsigned char buffer[2500];
  leitura_arquivo(buffer, 2500, &handle);

  return 0;
}
