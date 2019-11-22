#ifndef __INODES___
#define __INODES___

#include "gerenciamento_disco.h"

int escrever_inode(struct t2fs_inode* inode);
int ler_inode(struct t2fs_inode* inode, int numero_inode);
int retornaPosicaoLivreDeInodeEMarcaComoUsada();

#endif
