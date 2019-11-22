#include "../include/inodes.h"

//negativo em caso de erro, posicao em caso de sucesso
int retornaPosicaoLivreDeInodeEMarcaComoUsada(){
  printf("Particao ativa dentro do inode %d\n",particao_ativa );
  int	posicao = FALHA;
//  printf("gsygaaygsayg %d\n", retornaSetorDoSuperBloco(particao_ativa));
  if(openBitmap2(retornaSetorDoSuperBloco(particao_ativa)) == 0){
    posicao = searchBitmap2 (BITMAP_INODE, 0);
    setBitmap2 (BITMAP_INODE, posicao, 1);
  }
  if(closeBitmap2() != 0){
    printf("NAO FOI POSSIVEL FECHAR O BITMAP, DADOS NAO SALVOS\n" );
    return FALHA;
  }
  if(posicao < 0){
    printf("Nao foi possivel encotrar um inode\n" );
    return FALHA;
  }
  printf(" [[INODE %d ALOCADO]]\n", posicao);
  return posicao;
}

int devolve_setor_de_escrita_inode(int numero_inode){
  int inodes_por_setor         = tamanho_setor / sizeof(struct t2fs_inode);
  int setor_a_ser_escrito      = (numero_inode / inodes_por_setor) + particoes[particao_ativa].posicao_area_inodes;
  return setor_a_ser_escrito;
}

int devolve_setor_de_leitura_inode(int numero_inode){
  return devolve_setor_de_escrita_inode(numero_inode);
}

int devolve_deslocamento_em_bytes(int numero_inode){
  int inode_size               = sizeof(struct t2fs_inode);
  int inodes_por_setor         = tamanho_setor / inode_size;
  int deslocamento_em_bytes    = (numero_inode % inodes_por_setor) * inode_size;
  return deslocamento_em_bytes;
}

int escrever_inode(struct t2fs_inode* inode){
  int numero_inode = retornaPosicaoLivreDeInodeEMarcaComoUsada();

  if(numero_inode < 0 ){
    return FALHA;
  }

  unsigned char setor[tamanho_setor];
  int tamanho_inode = sizeof(struct t2fs_inode);
  int numero_setor  = devolve_setor_de_escrita_inode(numero_inode);
  int deslocamento  = devolve_deslocamento_em_bytes(numero_inode);

  if (read_sector(numero_setor, setor) == 0){
    copiarMemoria((char*) &setor[deslocamento], (char*) &inode, tamanho_inode);
    if(write_sector(numero_setor, setor) == 0){
        return numero_inode;
    }
  }
  return FALHA;
}

int ler_inode(struct t2fs_inode* inode, int numero_inode){
  unsigned char setor[tamanho_setor];
  int tamanho_inode = sizeof(struct t2fs_inode);
  int numero_setor  = devolve_setor_de_leitura_inode(numero_inode);
  int deslocamento  = devolve_deslocamento_em_bytes(numero_inode);

  if (read_sector(numero_setor, setor) == 0){
    copiarMemoria((char*) inode, (char*) &setor[deslocamento], tamanho_inode);

    return SUCESSO;
  }
  return FALHA;
}
