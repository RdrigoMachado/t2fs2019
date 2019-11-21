#ifndef __INODES___
#define __INODES___

#include "ler_e_escrever_no_disco.h"

int escrever_inode(struct t2fs_inode* inode, int numero_inode);
int ler_inode(struct t2fs_inode* inode, int numero_inode);


#endif
