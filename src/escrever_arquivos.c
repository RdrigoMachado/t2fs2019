#include "../include/escrever_arquivos.h"

//negativo em caso de erro, posicao em caso de sucesso
int retornaPosicaoLivreDeDadosEMarcaComoUsada(){
  int	posicao = -1;
  if(openBitmap2(retornaSetorDoSuperBloco(particao_ativa)) == 0){
    posicao = searchBitmap2 (BITMAP_DADOS, 0);
    if(posicao >=0 ){
      setBitmap2 (BITMAP_DADOS, posicao, 1);
    }
  }
  else{
    printf("Erro bitmap\n");
  }
  printf("posicao %d\n", posicao);
  return posicao;
}

//retorna endereço para o setor do inicio do bloco livre
int retornaBlocoLivreEMarcaBimapComoUsado(){
    int numero_bloco = retornaPosicaoLivreDeDadosEMarcaComoUsada();
    printf("tamanho em setores %d\n", particoes[particao_ativa].tamanho_bloco_em_setores);
    printf("bloco %d\n", numero_bloco);
    printf("=============\n" );
    return particoes[particao_ativa].posicao_area_dados + (numero_bloco * particoes[particao_ativa].tamanho_bloco_em_setores);
}

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

void escrita_direta_bloco(unsigned char* buffer, int setor_inicio_bloco, int bytes_a_serem_escritos, int* bytes_escritos, Handle *handle, int deslocameto_ativado){
  int deslocamento_bytes   = 0;
  int deslocamento_setores = 0;
  if(deslocameto_ativado == TRUE){
    deslocamento_bytes     = (handle->posicao_atual + *bytes_escritos) % tamanho_setor;
    deslocamento_setores   = (handle->posicao_atual + *bytes_escritos) / tamanho_setor;
  }
  int setor_a_ser_escrito    = setor_inicio_bloco + deslocamento_setores;
  int setor_escrito_relativo_ao_bloco = deslocamento_setores;
  int setores_por_bloco = bytes_bloco / tamanho_setor;

  printf("setor escrever %d - setor relativo %d - setores bloco %d \n", setor_a_ser_escrito, setor_escrito_relativo_ao_bloco, setores_por_bloco);
  printf("%d < %d && %d <= %d\n", *bytes_escritos, bytes_a_serem_escritos, setor_escrito_relativo_ao_bloco, setores_por_bloco);
  while(*bytes_escritos < bytes_a_serem_escritos && setor_escrito_relativo_ao_bloco <= setores_por_bloco){
    escrever_bytes_buffer_para_setor(buffer, setor_a_ser_escrito, bytes_a_serem_escritos, bytes_escritos, deslocamento_bytes);
    deslocamento_bytes = 0;
    setor_a_ser_escrito++;
    setor_escrito_relativo_ao_bloco++;
  }
}

int retornaSetorParaEscritaDoBloco(int bloco_a_ser_escrito,int bytes_escritos, Handle* handle){
  int ponteiro = -1;
  if(bloco_a_ser_escrito >= 0){
    if(bloco_a_ser_escrito < maior_bloco_caso_1){
      if(handle->arquivo.dataPtr[bloco_a_ser_escrito] == -1){
        ponteiro = retornaBlocoLivreEMarcaBimapComoUsado();
        handle->arquivo.dataPtr[bloco_a_ser_escrito] = ponteiro;
        handle->arquivo.blocksFileSize++;
      }
      else {
        ponteiro = handle->arquivo.dataPtr[bloco_a_ser_escrito];
      }
      printf("Direto %d\n", ponteiro );
    }
    else if(bloco_a_ser_escrito < maior_bloco_caso_2){
        unsigned char setor_ind_simples[tamanho_setor];
        int ponteiros_por_setor = (ponteiros_por_bloco / particoes[particao_ativa].tamanho_bloco_em_setores);
        int bloco               = bloco_a_ser_escrito - maior_bloco_caso_1;
        int ponteiro_dado;
        if (handle->arquivo.singleIndPtr == -1) {
          handle->arquivo.singleIndPtr = retornaBlocoLivreEMarcaBimapComoUsado();
        }
        int deslocamento_bloco_ind_simples = (bloco % ponteiros_por_bloco) / ponteiros_por_setor;
        int setor_bloco_indireto_simples = handle->arquivo.singleIndPtr + deslocamento_bloco_ind_simples;
        read_sector(setor_bloco_indireto_simples, setor_ind_simples);

        int deslocamento_setor_ind_simples = (bloco % ponteiros_por_bloco) % ponteiros_por_setor;
        printf("*********bloco %d  - deslocamento %d \n", bloco, deslocamento_setor_ind_simples );
        int posicao_escrita_setor = deslocamento_setor_ind_simples * PONTEIRO_EM_BYTES;

        if(bloco_a_ser_escrito == handle->arquivo.blocksFileSize){
          ponteiro_dado = retornaBlocoLivreEMarcaBimapComoUsado();
          handle->arquivo.blocksFileSize++;
          copiarMemoria((char*) &setor_ind_simples[posicao_escrita_setor],(char*) &ponteiro_dado,  PONTEIRO_EM_BYTES);
          write_sector(setor_bloco_indireto_simples, setor_ind_simples);
        }
        copiarMemoria((char*) & ponteiro_dado, (char*) &setor_ind_simples[posicao_escrita_setor], PONTEIRO_EM_BYTES);
        ponteiro = ponteiro_dado;
        printf("Single Ponteiro> %d\n", ponteiro);
        //ate aqui parece ok
    }
    else if(bloco_a_ser_escrito < maior_bloco_caso_3){
        unsigned char setor_ind_duplo[tamanho_setor];
        unsigned char setor_ind_simples[tamanho_setor];
        int ponteiro_simples, ponteiro_dado;
        int ponteiros_por_setor = (ponteiros_por_bloco / particoes[particao_ativa].tamanho_bloco_em_setores);
        int bloco               = bloco_a_ser_escrito - maior_bloco_caso_2;
        if(handle->arquivo.doubleIndPtr == -1){
          handle->arquivo.doubleIndPtr = retornaBlocoLivreEMarcaBimapComoUsado();
        }

        int deslocamento_bloco_ind_duplo = (bloco/ponteiros_por_bloco) / ponteiros_por_setor;
        int setor_bloco_indireto_duplo = handle->arquivo.doubleIndPtr + deslocamento_bloco_ind_duplo;
        read_sector(setor_bloco_indireto_duplo, setor_ind_duplo);

        ponteiro_simples = retornaBlocoLivreEMarcaBimapComoUsado();
        int deslocamento_setor_ind_duplo = (bloco/ponteiros_por_bloco) % ponteiros_por_setor;
        int posicao_escrita_setor = deslocamento_setor_ind_duplo * PONTEIRO_EM_BYTES;
        copiarMemoria((char*) &setor_ind_duplo[posicao_escrita_setor], (char*) ponteiro_simples, PONTEIRO_EM_BYTES);
        write_sector(setor_bloco_indireto_duplo, setor_ind_duplo);

        int deslocamento_bloco_ind_simples = (bloco % ponteiros_por_bloco) / ponteiros_por_setor;
        int setor_bloco_indireto_simples = ponteiro_simples + deslocamento_bloco_ind_simples;
        read_sector(setor_bloco_indireto_simples, setor_ind_simples);
        ponteiro_dado = retornaBlocoLivreEMarcaBimapComoUsado();
        handle->arquivo.blocksFileSize++;
        int deslocamento_setor_ind_simples = (bloco % ponteiros_por_bloco) % ponteiros_por_setor;
        posicao_escrita_setor = deslocamento_setor_ind_simples * PONTEIRO_EM_BYTES;
        copiarMemoria((char*) &setor_ind_simples[posicao_escrita_setor], (char*) & ponteiro_dado, PONTEIRO_EM_BYTES);
        write_sector(setor_bloco_indireto_simples, setor_ind_simples);
        ponteiro = ponteiro_dado;
      }
  }
  return ponteiro;
}

int escrita_arquivo(unsigned char* buffer, int bytes_a_serem_escritos, Handle* handle){

  int bytes_escritos = 0;
  int bloco_a_ser_escrito = handle->posicao_atual / bytes_bloco;
  printf("tamanho futuro %d - tamanho maximo %d \n", particoes[particao_ativa].posicao_inicio + (handle->posicao_atual+bytes_a_serem_escritos)/256, particoes[particao_ativa].posicao_fim );
  if(particoes[particao_ativa].posicao_inicio + (handle->posicao_atual+bytes_a_serem_escritos)/256 > particoes[particao_ativa].posicao_fim ){
    return FALHA;
  }
  printf("depois verificacao \n");
  int inicio_bloco;
  int deslocameto_ativado = TRUE;
  printf("Bytes escritos %d < bytes escrever %d ?\n",bytes_escritos, bytes_a_serem_escritos );

  while(bytes_escritos < bytes_a_serem_escritos){

    inicio_bloco = retornaSetorParaEscritaDoBloco(bloco_a_ser_escrito, bytes_escritos, handle);
    printf("bloco inicio %d\n", inicio_bloco);
    if(inicio_bloco == -1){
      return FALHA;
    }
    escrita_direta_bloco(buffer,inicio_bloco,bytes_a_serem_escritos, &bytes_escritos, handle, deslocameto_ativado);
    bloco_a_ser_escrito = (handle->posicao_atual + bytes_escritos) / bytes_bloco;
    printf("Bytes escritos %d < bytes escrever %d ?\n",bytes_escritos, bytes_a_serem_escritos );
    deslocameto_ativado = FALSE;
  }
  handle->posicao_atual = handle->posicao_atual + bytes_escritos;
  handle->arquivo.bytesFileSize += bytes_escritos;
  return SUCESSO;
}
//
// int main(){
//   le_MBR_Preenche_Dados_Particoes();
//   formatarParticao(1, 4);
//
//   leSetorEPreencheStructSuperBloco(&super_bloco_atual, 1);
//
//   int bloco = retornaBlocoLivreEMarcaBimapComoUsado();
//   printf("Blcoo livre %d\n",bloco );
//   bloco = retornaBlocoLivreEMarcaBimapComoUsado();
//   printf("Blcoo livre %d\n",bloco );
//   bloco = retornaBlocoLivreEMarcaBimapComoUsado();
//   printf("Blcoo livre %d\n",bloco );
//   bloco = retornaBlocoLivreEMarcaBimapComoUsado();
//   printf("Blcoo livre %d\n",bloco );
//   bloco = retornaBlocoLivreEMarcaBimapComoUsado();
//   printf("Blcoo livre %d\n",bloco );
//
//   return 0;
// }



int main(){

  carregaDadosDisco();
  int index = 0;
   for(index =0; index<4 ; index++){
     //formatarParticao(index, 4);

    carregaDadosParticao(&super_bloco_atual, index);
    printf("Inicio %d: %d\n",index, particoes[index].posicao_inicio );
    printf("Fim %d: %d\n",index, particoes[index].posicao_fim );
    printf("Bloco inicio inodes: %d\n", particoes[index].posicao_area_inodes );
    printf("Bloco inicio dados: %d\n", particoes[index].posicao_area_dados );
    printf("Area Inode: %d \n\n\n\n",super_bloco_atual.inodeAreaSize );
  }
  // carregaDadosParticao(&super_bloco_atual, 0);
  //
  //
  //
  // Handle handle;
  // handle.posicao_atual          = 0;
  // handle.arquivo.blocksFileSize = 0;
  // handle.arquivo.bytesFileSize  = 0;
  // handle.arquivo.dataPtr[0]     = -1;
  // handle.arquivo.dataPtr[1]     = -1;
  // handle.arquivo.singleIndPtr   = -1;
  // handle.arquivo.doubleIndPtr   = -1;
  // handle.arquivo.RefCounter     = 0;
  //
  //
  // char nome[30] = {'a','r','q','u','i','v','o','x','\0'};
  // for(index = 0; index < 80; index++){
  //   struct t2fs_record novo;
  //   novo.TypeVal = TYPEVAL_REGULAR;
  //   copiarMemoria(novo.name, nome, 9);
  //
  //   if(escrita_arquivo((unsigned char*)&novo, sizeof(struct t2fs_record), &handle) < 0)
  //     printf("Erro\n");
  //   else{
  //     printf("===============================================================\n");
  //     printf("pointer0 %d - ponter1 %d - pointerSingle %d - pointerDoubl %d\n", handle.arquivo.dataPtr[0],handle.arquivo.dataPtr[1],handle.arquivo.singleIndPtr, handle.arquivo.doubleIndPtr );
  //     printf("Size %d\n", handle.arquivo.bytesFileSize);
  //   }
  // }
  // handle.posicao_atual = 0;
  // printf("Leitura\n" );
  // for(index = 0; index < 80; index++){
  //   struct t2fs_record novo;
  //   if(leitura_arquivo((unsigned char*)&novo, sizeof(struct t2fs_record), &handle) < 0)
  //     printf("Erro\n");
  //   else{
  //     printf("===============================================================\n");
  //     printf("nome  %s \n", novo.name );
  //     printf("Size %d\n", handle.posicao_atual);
  //   }
  // }

  return 0;
}
