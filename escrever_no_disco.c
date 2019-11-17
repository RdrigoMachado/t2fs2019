void escrever_bytes_buffer_para_setor(unsigned char* buffer, int numero_setor, int bytes_a_serem_escritos, int* bytes_escritos, int deslocamento){
  unsigned char setor[tamanho_setor];
  read_sector(numero_setor, setor);
  int contador_bytes = deslocamento;

  while(contador_bytes < tamanho_setor && *bytes_escritos < bytes_a_serem_escritos){
    setor[contador_bytes] = buffer[*bytes_escritos];
    contador_bytes++;
    *bytes_escritos = *bytes_escritos + 1;
  }
  write_sector(numero_setor, setor);
}

void escrita_direta_bloco(unsigned char* buffer, int setor_inicio_bloco, int bytes_a_serem_escritos, int* bytes_escritos){
  int deslocamento_bytes     = (posicao_atual + *bytes_escritos) % tamanho_setor;
  int deslocamento_setores   = (posicao_atual + *bytes_escritos) / tamanho_setor;
  int setor_a_ser_escrito       = setor_inicio_bloco + deslocamento_setores;
  int byte_relativo_ao_bloco = deslocamento_bytes + deslocamento_setores* tamanho_setor;

  while(*bytes_escritos < bytes_a_serem_escritos && byte_relativo_ao_bloco < bytes_bloco){
    escrever_bytes_buffer_para_setor(buffer, setor_a_ser_escrito, bytes_a_serem_escritos, bytes_escritos, deslocamento_bytes);
    deslocamento_bytes = 0;
    setor_a_ser_escrito++;
    byte_relativo_ao_bloco++;
  }
}

int retornaSetorParaEscritaDoBloco(int bloco_a_ser_escrito, struct t2fs_inode arquivo){
  int ponteiro = -1;
  if(bloco_a_ser_escrito >= 0){
    if(bloco_a_ser_escrito < maior_bloco_caso_1){
      ponteiro = arquivo.dataPtr[bloco_a_ser_escrito];
    }
    else if(bloco_a_ser_escrito < maior_bloco_caso_2){
      unsigned char buffer[tamanho_setor];
      int bloco_dado = bloco_a_ser_escrito - maior_bloco_caso_1;
      read_sector(arquivo.singleIndPtr, buffer);
      copiarMemoria((char*) &ponteiro, (char*) &buffer[bloco_dado * PONTEIRO_EM_BYTES] , PONTEIRO_EM_BYTES);
    }
    else if(bloco_a_ser_escrito < maior_bloco_caso_3){
      unsigned char buffer_indireto[tamanho_setor];
      unsigned char buffer[tamanho_setor];
      int bloco_indireto = bloco_a_ser_escrito / ponteiros_por_bloco;
      int bloco_dado     = bloco_a_ser_escrito % ponteiros_por_bloco;
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

int escrita_arquivo(unsigned char* buffer, int bytes_a_serem_escritos, Handle handle){
  int bytes_escritos = 0;
  int bloco_a_ser_escrito = handle.posicao_atual / bytes_bloco;
  int inicio_bloco;
  while(bytes_escritos < bytes_a_serem_escritos){
    inicio_bloco = retornaSetorParaEscritaDoBloco(bloco_a_ser_escrito, handle.arquivo);
    if(inicio_bloco == -1){
      return FALHA;
    }
    escrita_direta_bloco(buffer,inicio_bloco,bytes_a_serem_escritos, &bytes_escritos);
    bloco_a_ser_escrito = (posicao_atual + bytes_escritos) / bytes_bloco;
  }
  posicao_atual = posicao_atual + bytes_escritos;
  return SUCESSO;
}
