#include "../include/leitura_arquivos.h"

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

void leitura_direta_bloco(unsigned char* buffer, int setor_inicio_bloco, int bytes_a_serem_lidos, int* bytes_lidos, Handle* handle, int deslocameto_ativado){
  int deslocamento_bytes   = 0;
  int deslocamento_setores = 0;
  if(deslocameto_ativado == TRUE){
    deslocamento_bytes     = (handle->posicao_atual + *bytes_lidos) % tamanho_setor;
    deslocamento_setores   = (handle->posicao_atual + *bytes_lidos) / tamanho_setor;
  }
  int setor_a_ser_lido             = setor_inicio_bloco + deslocamento_setores;
  int setor_lido_relativo_ao_bloco = deslocamento_setores;
  int setores_por_bloco = bytes_bloco / tamanho_setor;
  while(*bytes_lidos < bytes_a_serem_lidos && setor_lido_relativo_ao_bloco < setores_por_bloco){
    ler_bytes_setor_para_buffer(buffer, setor_a_ser_lido, bytes_a_serem_lidos, bytes_lidos, deslocamento_bytes);
    deslocamento_bytes = 0;
    setor_a_ser_lido++;
    setor_lido_relativo_ao_bloco++;
  }
}


int retornaSetorParaLeituraDoBloco(int bloco_a_ser_lido, Handle* handle){

  int ponteiro = -1;
  unsigned char buffer[tamanho_setor];

  if(bloco_a_ser_lido >= 0){
    if(bloco_a_ser_lido < maior_bloco_caso_1){
      if(handle->arquivo.dataPtr[bloco_a_ser_lido] == -1){
        return FALHA;
      }
      ponteiro = handle->arquivo.dataPtr[bloco_a_ser_lido];
      printf("Dado %d\n", bloco_a_ser_lido);
    }
    else if(bloco_a_ser_lido < maior_bloco_caso_2){
        printf("Single %d\n", bloco_a_ser_lido);
        int ponteiros_por_setor = (ponteiros_por_bloco / particoes[particao_ativa].tamanho_bloco_em_setores);
        int bloco               = bloco_a_ser_lido - maior_bloco_caso_1;
        int ponteiro_dado;
        if (handle->arquivo.singleIndPtr == -1) {
          return FALHA;
        }

        int deslocamento_bloco_ind_simples = (bloco % ponteiros_por_bloco) / ponteiros_por_setor;
        int setor_bloco_indireto_simples = handle->arquivo.singleIndPtr + deslocamento_bloco_ind_simples;
        read_sector(setor_bloco_indireto_simples, buffer);

        int deslocamento_setor_ind_simples = (bloco % ponteiros_por_bloco) % ponteiros_por_setor;
        int posicao_escrita_setor = deslocamento_setor_ind_simples * PONTEIRO_EM_BYTES;
        int validade;

        copiarMemoria((char*) &validade, (char*) &buffer[posicao_escrita_setor], PONTEIRO_EM_BYTES);
        if(validade == -1){
          return FALHA;
        }
        else{
          copiarMemoria((char*) &ponteiro_dado, (char*) &buffer[posicao_escrita_setor], PONTEIRO_EM_BYTES);
        }
        ponteiro = ponteiro_dado;
        //ate aqui parece ok
    }
    else if(bloco_a_ser_lido < maior_bloco_caso_3){
        printf("Double %d\n", bloco_a_ser_lido);
        int ponteiro_simples, ponteiro_dado;
        int ponteiros_por_setor = (ponteiros_por_bloco / particoes[particao_ativa].tamanho_bloco_em_setores);
        int bloco               = bloco_a_ser_lido - maior_bloco_caso_2;
        if(handle->arquivo.doubleIndPtr == -1){
          return FALHA;
        }

        int ponteiro_ind_dupla   = bloco / ponteiros_por_bloco;
        int setor_ind_dupla      = handle->arquivo.doubleIndPtr + (ponteiro_ind_dupla / ponteiros_por_setor);
        int deslocamento_bytes   = ((bloco / ponteiros_por_bloco) % ponteiros_por_setor) * PONTEIRO_EM_BYTES;

        read_sector(setor_ind_dupla, buffer);
        int validade;
        copiarMemoria((char*) &validade, (char*) &buffer[deslocamento_bytes], PONTEIRO_EM_BYTES);

        if(validade == -1){
          return FALHA;
        }
        copiarMemoria((char*) &ponteiro_simples, (char*) &buffer[deslocamento_bytes], PONTEIRO_EM_BYTES);

        int ponteiro_ind_simples = bloco % ponteiros_por_bloco;
        int setor_ind_simples    = ponteiro_simples + (ponteiro_ind_simples / ponteiros_por_setor);
        deslocamento_bytes       = (bloco % ponteiros_por_setor) * PONTEIRO_EM_BYTES;
        read_sector(setor_ind_simples, buffer);

        copiarMemoria((char*) &validade, (char*) &buffer[deslocamento_bytes], PONTEIRO_EM_BYTES);
        if(validade == -1){
          return FALHA;
        }
        copiarMemoria((char*) &ponteiro_dado, (char*) &buffer[deslocamento_bytes], PONTEIRO_EM_BYTES);
        printf("Retornando bloco dados:   == %d\n", ponteiro_dado);
        ponteiro = ponteiro_dado;
      }
  }
  return ponteiro;
}

int leitura_arquivo(unsigned char* buffer, int bytes_a_serem_lidos, Handle* handle){

  int bytes_lidos = 0;
  int bloco_a_ser_lido = handle->posicao_atual / bytes_bloco;
  int inicio_bloco;
  int deslocameto_ativado = TRUE;
  if ((handle->posicao_atual+bytes_a_serem_lidos)> handle->arquivo.bytesFileSize){
    return FALHA;
  }
  while(bytes_lidos < bytes_a_serem_lidos){
    inicio_bloco = retornaSetorParaLeituraDoBloco(bloco_a_ser_lido, handle);
    if(inicio_bloco == -1){
       return FALHA;
    }
    leitura_direta_bloco(buffer,inicio_bloco,bytes_a_serem_lidos, &bytes_lidos, handle, deslocameto_ativado);
    bloco_a_ser_lido = (handle->posicao_atual + bytes_lidos) / bytes_bloco;
    deslocameto_ativado = FALSE;
  }
  handle->posicao_atual = handle->posicao_atual + bytes_lidos;
  return SUCESSO;
}
