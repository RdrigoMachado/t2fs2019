#include "../include/inodes.c"

int devolve_setor_de_escrita_inode(int numero_inode){
  int inodes_por_setor         = tamanho_setor / sizeof(struct t2fs_inode);
  int setor_a_ser_escrito      = (numero_inode / inodes_por_setor) + particoes[particao_ativa].posicao_area_inodes;
  return setor_a_ser_escrito;
}

int devolve_deslocamento_em_bytes(int numero_inode){
  int inode_size               = sizeof(struct t2fs_inode);
  int inodes_por_setor         = tamanho_setor / inode_size;
  int deslocamento_em_bytes    = (numero_inode % inodes_por_setor) * inode_size;
  return deslocamento_em_bytes;
}

int escrever_inode(struct t2fs_inode* inode, int numero_inode){
  unsigned char setor[tamanho_setor];
  int tamanho_inode = sizeof(struct t2fs_inode);
  int numero_setor  = devolve_setor_de_escrita_inode(numero_inode);
  int deslocamento  = devolve_deslocamento_em_bytes(numero_inode);

  if (read_sector(numero_setor, setor) == 0){
    copiarMemoria((char*) &setor[deslocamento], (char*) &inode, tamanho_inode);
    if(write_sector(numero_setor, setor) == 0){
        return SUCESSO;
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
    copiarMemoria((char*) &inode, (char*) &setor[deslocamento], tamanho_inode);
    return SUCESSO;
  }
  return FALHA;
}
