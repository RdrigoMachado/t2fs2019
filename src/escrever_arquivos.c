#include "../include/escrever_arquivos.h"
int contador_indirecao = 0;



//negativo em caso de erro, posicao em caso de sucesso
int retornaPosicaoLivreDeDadosEMarcaComoUsada(){
  int	posicao = FALHA;
  if(openBitmap2(retornaSetorDoSuperBloco(particao_ativa)) == 0){
    posicao = searchBitmap2 (BITMAP_DADOS, 0);
    if(posicao >=0 ){
      setBitmap2 (BITMAP_DADOS, posicao, 1);
    }
  }
  return posicao;
}

//retorna endereço para o setor do inicio do bloco livre
int retornaBlocoLivreEMarcaBimapComoUsado(){
    int numero_bloco = retornaPosicaoLivreDeDadosEMarcaComoUsada();
    printf("Bloco livre = %d\n",numero_bloco );
    if(numero_bloco < 0)
      return FALHA;
    else
      return particoes[particao_ativa].posicao_area_dados + (numero_bloco * particoes[particao_ativa].tamanho_bloco_em_setores);
}

int bitmap_dados_size(){
  int outros_blocos = particoes[particao_ativa].blocos_para_bitmap_dados;
  outros_blocos    += particoes[particao_ativa].blocos_para_bitmap_inodes;
  outros_blocos    += particoes[particao_ativa].area_inode_em_blocos;
  outros_blocos++;
  int blocos_dados = particoes[particao_ativa].blocos_disco - (outros_blocos);
  return blocos_dados;
}

void print_bitmap_livre(){
  int blocos_livres = 0;
  int index;
  int limite = bitmap_dados_size();
  for(index = 0; index< limite; index++){
    if(getBitmap2(BITMAP_DADOS, index) == 0){
      blocos_livres++;
    }
  }
  printf("Blocos livres: %d\n", blocos_livres);
}

int tem_blocos_livres_suficientes(int blocos_necessarios){

  int blocos_livres = 0;
  int index;
  int limite = bitmap_dados_size();
  for(index = 0; index< limite; index++){
    if(getBitmap2(BITMAP_DADOS, index) == 0){
      blocos_livres++;
    }
  }
  printf("blocos necessarios: %d - blocos livres: %d\n", blocos_necessarios, blocos_livres );
  if(blocos_necessarios <= blocos_livres) {
    return TRUE;
  }
  return FALSE;

}

int blocos_necessarios_para_escrita(Handle* handle, int bytes_a_serem_escritos){
  int total_bytes          = (handle->arquivo.bytesFileSize + bytes_a_serem_escritos);
  int setores_necessarios  = total_bytes / tamanho_setor;
  if(total_bytes % tamanho_setor > 0){
    setores_necessarios++;
  }

  int blocos_necessarios   = setores_necessarios / particoes[particao_ativa].tamanho_bloco_em_setores;
  if(setores_necessarios % particoes[particao_ativa].tamanho_bloco_em_setores > 0){
    blocos_necessarios++;
  }

  return blocos_necessarios;
}

int escrever_bytes_buffer_para_setor(unsigned char* buffer, int numero_setor, int bytes_a_serem_escritos, int* bytes_escritos, int deslocamento){
  unsigned char setor[tamanho_setor];
  read_sector(numero_setor, setor);
  int contador_bytes = deslocamento;
  if(numero_setor <= particoes[particao_ativa].posicao_inicio || numero_setor >= particoes[particao_ativa].posicao_fim){
    printf("Area invalida #%d\n", numero_setor);
    return FALHA;
  }
  printf("    [SETOR: #%d]   \n", numero_setor);
  while(contador_bytes < tamanho_setor && *bytes_escritos < bytes_a_serem_escritos){
    setor[contador_bytes] = buffer[*bytes_escritos];
    contador_bytes++;
    *bytes_escritos = *bytes_escritos + 1;
  }
  write_sector(numero_setor, setor);
  return SUCESSO;
}

int escrita_direta_bloco(unsigned char* buffer, int setor_inicio_bloco, int bytes_a_serem_escritos, int* bytes_escritos, Handle *handle, int deslocameto_ativado){
  int deslocamento_bytes   = 0;
  int deslocamento_setores = 0;
  if(deslocameto_ativado == TRUE){
    deslocamento_bytes     = (handle->posicao_atual + *bytes_escritos) % tamanho_setor;
    deslocamento_setores   = (handle->posicao_atual + *bytes_escritos) / tamanho_setor;
  }
  int setor_a_ser_escrito    = setor_inicio_bloco + deslocamento_setores;
  int setor_escrito_relativo_ao_bloco = deslocamento_setores;
  int setores_por_bloco = bytes_bloco / tamanho_setor;

  while(*bytes_escritos < bytes_a_serem_escritos && setor_escrito_relativo_ao_bloco <= setores_por_bloco){
    if (escrever_bytes_buffer_para_setor(buffer, setor_a_ser_escrito, bytes_a_serem_escritos, bytes_escritos, deslocamento_bytes) == FALHA){
      return FALHA;
    }
    deslocamento_bytes = 0;
    setor_a_ser_escrito++;
    setor_escrito_relativo_ao_bloco++;
  }
  return SUCESSO;
}


void invalidaEnderecos(unsigned char* buffer){
  int index;
  int invalido = -1;
  for(index = 0; index < tamanho_setor/PONTEIRO_EM_BYTES; index++){
    copiarMemoria((char*) &buffer[index*PONTEIRO_EM_BYTES], (char*) &invalido, PONTEIRO_EM_BYTES);
  }
}
void invalidaEnderecosBloco(int setor_inicio_bloco){
  int setores_por_bloco = particoes[particao_ativa].tamanho_bloco_em_setores;
  int index;
  unsigned char buffer[tamanho_setor];
  invalidaEnderecos(buffer);
  for(index = 0; index < setores_por_bloco; index++){
    write_sector(setor_inicio_bloco+index, buffer);
  }
}

int retornaSetorParaEscritaDoBloco(int bloco_a_ser_escrito,int bytes_escritos, Handle* handle){

  int ponteiro = -1;
  unsigned char buffer[tamanho_setor];

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
      printf("Dado %d\n", bloco_a_ser_escrito);
    }
    else if(bloco_a_ser_escrito < maior_bloco_caso_2){
      printf("Single %d\n", bloco_a_ser_escrito);
        int ponteiros_por_setor = (ponteiros_por_bloco / particoes[particao_ativa].tamanho_bloco_em_setores);
        int bloco               = bloco_a_ser_escrito - maior_bloco_caso_1;
        int ponteiro_dado;
        if (handle->arquivo.singleIndPtr == -1) {
          handle->arquivo.singleIndPtr = retornaBlocoLivreEMarcaBimapComoUsado();
          invalidaEnderecosBloco(handle->arquivo.singleIndPtr);
          contador_indirecao++;
        }

        int deslocamento_bloco_ind_simples = (bloco % ponteiros_por_bloco) / ponteiros_por_setor;
        int setor_bloco_indireto_simples = handle->arquivo.singleIndPtr + deslocamento_bloco_ind_simples;
        read_sector(setor_bloco_indireto_simples, buffer);

        int deslocamento_setor_ind_simples = (bloco % ponteiros_por_bloco) % ponteiros_por_setor;
        int posicao_escrita_setor = deslocamento_setor_ind_simples * PONTEIRO_EM_BYTES;
        int validade;

        copiarMemoria((char*) &validade, (char*) &buffer[posicao_escrita_setor], PONTEIRO_EM_BYTES);
        if(validade == -1){
          ponteiro_dado = retornaBlocoLivreEMarcaBimapComoUsado();
          handle->arquivo.blocksFileSize++;
          copiarMemoria((char*) &buffer[posicao_escrita_setor], (char*) &ponteiro_dado, PONTEIRO_EM_BYTES);
          write_sector(setor_bloco_indireto_simples, buffer);
        }
        else{
          copiarMemoria((char*) &ponteiro_dado, (char*) &buffer[posicao_escrita_setor], PONTEIRO_EM_BYTES);
        }
        ponteiro = ponteiro_dado;
        //ate aqui parece ok
    }
    else if(bloco_a_ser_escrito < maior_bloco_caso_3){
        printf("Double %d\n", bloco_a_ser_escrito);
        int ponteiro_simples, ponteiro_dado;
        int ponteiros_por_setor = (ponteiros_por_bloco / particoes[particao_ativa].tamanho_bloco_em_setores);
        int bloco               = bloco_a_ser_escrito - maior_bloco_caso_2;
        if(handle->arquivo.doubleIndPtr == -1){
          handle->arquivo.doubleIndPtr = retornaBlocoLivreEMarcaBimapComoUsado();
          invalidaEnderecosBloco(handle->arquivo.doubleIndPtr);
          printf("Criando bloco ind dupla:    ==  %d\n",handle->arquivo.doubleIndPtr);
          contador_indirecao++;
        }

        int ponteiro_ind_dupla   = bloco / ponteiros_por_bloco;
        int setor_ind_dupla      = handle->arquivo.doubleIndPtr + (ponteiro_ind_dupla / ponteiros_por_setor);
        int deslocamento_bytes   = (ponteiro_ind_dupla % ponteiros_por_setor) * PONTEIRO_EM_BYTES;


        read_sector(setor_ind_dupla, buffer);
        int validade;
        copiarMemoria((char*) &validade, (char*) &buffer[deslocamento_bytes], PONTEIRO_EM_BYTES);

        if(validade == -1){
          ponteiro_simples = retornaBlocoLivreEMarcaBimapComoUsado();
          invalidaEnderecosBloco(ponteiro_simples);
          copiarMemoria((char*) &buffer[deslocamento_bytes], (char*) &ponteiro_simples, PONTEIRO_EM_BYTES);
          write_sector(setor_ind_dupla, buffer);
          printf("Criando bloco ind simples:   == %d\n", ponteiro_simples);
        }
        else{
          copiarMemoria((char*) &ponteiro_simples, (char*) &buffer[deslocamento_bytes], PONTEIRO_EM_BYTES);
        }

        int ponteiro_ind_simples = bloco % ponteiros_por_bloco;
        int setor_ind_simples    = ponteiro_simples + ponteiro_ind_simples / ponteiros_por_setor;
        deslocamento_bytes       = (ponteiro_ind_simples % ponteiros_por_setor) * PONTEIRO_EM_BYTES;
        read_sector(setor_ind_simples, buffer);

        copiarMemoria((char*) &validade, (char*) &buffer[deslocamento_bytes], PONTEIRO_EM_BYTES);
        if(validade == -1){
          ponteiro_dado = retornaBlocoLivreEMarcaBimapComoUsado();
          invalidaEnderecosBloco(ponteiro_dado);
          handle->arquivo.blocksFileSize++;
          copiarMemoria((char*) &buffer[deslocamento_bytes], (char*) &ponteiro_dado, PONTEIRO_EM_BYTES);
          write_sector(setor_ind_simples, buffer);
          printf("Retornando bloco dados:   == %d\n", ponteiro_dado);
        }
        else {
          copiarMemoria((char*) &ponteiro_dado, (char*) &buffer[deslocamento_bytes], PONTEIRO_EM_BYTES);
          printf("Retornando bloco dados:   == %d\n", ponteiro_dado);
        }
        ponteiro = ponteiro_dado;
      }
  }
  return ponteiro;
}

int escrita_arquivo(unsigned char* buffer, int bytes_a_serem_escritos, Handle* handle) {
  int bytes_escritos = 0;
  int bloco_a_ser_escrito = handle->posicao_atual / bytes_bloco;
  int blocos_necessarios = blocos_necessarios_para_escrita(handle, bytes_a_serem_escritos) - handle->arquivo.blocksFileSize ;
  if(tem_blocos_livres_suficientes(blocos_necessarios) == FALSE){
    printf("Falha: blocos necessarios: %d - blocos usados: %d\n",blocos_necessarios, handle->arquivo.blocksFileSize );
    return FALHA;
  }
  int inicio_bloco;
  int deslocameto_ativado = TRUE;

  while(bytes_escritos < bytes_a_serem_escritos){
    inicio_bloco = retornaSetorParaEscritaDoBloco(bloco_a_ser_escrito, bytes_escritos, handle);
    if(inicio_bloco == -1){
      printf("FALHA %d\n",inicio_bloco );
      return FALHA;
    }
    if (escrita_direta_bloco(buffer,inicio_bloco,bytes_a_serem_escritos, &bytes_escritos, handle, deslocameto_ativado) == FALHA){
      return FALHA;
    }
    bloco_a_ser_escrito = (handle->posicao_atual + bytes_escritos) / bytes_bloco;
    deslocameto_ativado = FALSE;
  }
  handle->posicao_atual = handle->posicao_atual + bytes_escritos;
  handle->arquivo.bytesFileSize += bytes_escritos;
  return SUCESSO;
}

int main(){

  carregaDadosDisco();
   int index = 0;
   for(index =0; index<4 ; index++){
    formatarParticao(index, 4);
    carregaDadosParticao(&super_bloco_atual, index);
    printf("Inicio %d: %d\n",index, particoes[index].posicao_inicio );
    printf("Fim %d: %d\n",index, particoes[index].posicao_fim );
    printf("Bloco inicio inodes: %d\n", particoes[index].posicao_area_inodes );
    printf("Bloco inicio dados: %d\n", particoes[index].posicao_area_dados );
    printf("Area Inode: %d \n\n\n\n",super_bloco_atual.inodeAreaSize );
  }
  index = 0;

  carregaDadosParticao(&super_bloco_atual, index);



  Handle handle;
  handle.posicao_atual          = 0;
  handle.arquivo.blocksFileSize = 0;
  handle.arquivo.bytesFileSize  = 0;
  handle.arquivo.dataPtr[0]     = -1;
  handle.arquivo.dataPtr[1]     = -1;
  handle.arquivo.singleIndPtr   = -1;
  handle.arquivo.doubleIndPtr   = -1;
  handle.arquivo.RefCounter     = 0;


  char nome[30] = {'a','r','q','u','i','v','o','x','\0'};
  struct t2fs_record novo;
  novo.TypeVal = TYPEVAL_REGULAR;

  for(index = 0; index < 18000; index++){

    copiarMemoria((char*) novo.name, (char*) &index, 4);

    if(escrita_arquivo((unsigned char*)&novo, sizeof(struct t2fs_record), &handle) < 0){
          printf("Erro\n");
    } else {
      printf("===============================================================\n");
     printf("pointer0 %d - ponter1 %d - pointerSingle %d - pointerDoubl %d\n", handle.arquivo.dataPtr[0],handle.arquivo.dataPtr[1],handle.arquivo.singleIndPtr, handle.arquivo.doubleIndPtr );
      printf("BLOCOS %d - indirecao %d ", handle.arquivo.blocksFileSize, contador_indirecao);
      print_bitmap_livre();


    }
  }
  handle.posicao_atual = 0;
  printf("Leitura\n" );
  for(index = 0; index < 4136; index++){
    struct t2fs_record novo;
    if(leitura_arquivo((unsigned char*)&novo, sizeof(struct t2fs_record), &handle) < 0)
      printf("Erro\n");
    else{
      printf("===============================================================\n");
      int numero;
      copiarMemoria((char*) &numero, (char*) novo.name, 4);

      printf("nome  %d \n", numero );
      printf("Size %d\n", handle.posicao_atual);
    }
  }
  return 0;
}
