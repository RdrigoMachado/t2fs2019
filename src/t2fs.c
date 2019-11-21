
/**
*/
#include "../include/apidisk.h"
#include "../include/string.h"
#include "../include/gerenciamento_disco.h"
#include "../include/bitmap2.h"
#include "../include/t2disk.h"
#include <stdio.h>
#include <stdlib.h>


struct t2fs_inode* diretorio;

int inicializada = FALSE;


void inicializar(){
	carregaDadosDisco();
	inicializada = TRUE;
}


/*-----------------------------------------------------------------------------
Função:	Informa a identificação dos desenvolvedores do T2FS.
-----------------------------------------------------------------------------*/
int identify2 (char *name, int size) {
	return -1;
}

/*-----------------------------------------------------------------------------
Função:	Formata logicamente uma partição do disco virtual t2fs_disk.dat para o sistema de
		arquivos T2FS definido usando blocos de dados de tamanho
		corresponde a um múltiplo de setores dados por sectors_per_block.
-----------------------------------------------------------------------------*/
int format2(int partition, int sectors_per_block) {
	if(inicializada == FALSE)
		inicializar();
	return formatarParticao(partition, sectors_per_block);
}

/*-----------------------------------------------------------------------------
Função:	Monta a partição indicada por "partition" no diretório raiz
-----------------------------------------------------------------------------*/
int mount(int partition) {
	if(inicializada == FALSE)
		inicializar();
	return -1;
}

/*-----------------------------------------------------------------------------
Função:	Desmonta a partição atualmente montada, liberando o ponto de montagem.
-----------------------------------------------------------------------------*/
int unmount(void) {
	if(inicializada == FALSE)
		inicializar();
	return -1;
}

/*-----------------------------------------------------------------------------
Função:	Função usada para criar um novo arquivo no disco e abrí-lo,
		sendo, nesse último aspecto, equivalente a função open2.
		No entanto, diferentemente da open2, se filename referenciar um
		arquivo já existente, o mesmo terá seu conteúdo removido e
		assumirá um tamanho de zero bytes.
-----------------------------------------------------------------------------*/
FILE2 create2 (char *filename) {
	if(inicializada == FALSE)
		inicializar();

/*-----------------------------------------------------------------
    verifica Open

*/
		int inode_novo_arquivo;
		int nova_quant_blocos;


		if (diretorio == NULL){
            return -1;
		}

		openBitmap2(retornaSetorDoSuperBloco(particao_ativa));
		inode_novo_arquivo = searchBitmap2 (BITMAP_INODE, 0);
		if (inode_novo_arquivo <= 0) {
            return -1;
		}


   	nova_quant_blocos = (diretorio->bytesFileSize + sizeof(struct t2fs_record)) / bytes_bloco;
		if ((diretorio->bytesFileSize + sizeof(struct t2fs_record)) % bytes_bloco){
			nova_quant_blocos++;
		}
		if (nova_quant_blocos > diretorio->blocksFileSize){
			if( searchBitmap2 (BITMAP_DADOS, 0) < 0){
				return -1;
			}
		}

		setBitmap2(BITMAP_INODE, inode_novo_arquivo, 1);
		struct t2fs_inode novo_inode;
		novo_inode.blocksFileSize=0;
		novo_inode.bytesFileSize=0;
		novo_inode.dataPtr[0] = -1;
		novo_inode.dataPtr[1] = -1;
		novo_inode.singleIndPtr = -1;
		novo_inode.doubleIndPtr = -1;
		novo_inode.RefCounter = 1;

         /*
         salva inode
         preencher entry
         salva entrada no diretorio
         open


         */

/*-------------------------------------------------------------------*/
	return -1;
}

/*-----------------------------------------------------------------------------
Função:	Função usada para remover (apagar) um arquivo do disco.
-----------------------------------------------------------------------------*/
int delete2 (char *filename) {
	if(inicializada == FALSE)
		inicializar();
	return -1;
}

/*-----------------------------------------------------------------------------
Função:	Função que abre um arquivo existente no disco.
-----------------------------------------------------------------------------*/
FILE2 open2 (char *filename) {
	if(inicializada == FALSE)
		inicializar();
	return -1;
}

/*-----------------------------------------------------------------------------
Função:	Função usada para fechar um arquivo.
-----------------------------------------------------------------------------*/
int close2 (FILE2 handle) {
	if(inicializada == FALSE)
		inicializar();
	return -1;
}

/*-----------------------------------------------------------------------------
Função:	Função usada para realizar a leitura de uma certa quantidade
		de bytes (size) de um arquivo.
-----------------------------------------------------------------------------*/
int read2 (FILE2 handle, char *buffer, int size) {
	if(inicializada == FALSE)
		inicializar();
	return leitura_arquivo((unsigned char*) buffer, size, &arquivos[handle]);
}

/*-----------------------------------------------------------------------------
Função:	Função usada para realizar a escrita de uma certa quantidade
		de bytes (size) de  um arquivo.
-----------------------------------------------------------------------------*/
int write2 (FILE2 handle, char *buffer, int size) {
	if(inicializada == FALSE)
		inicializar();
	return -1;
}

/*-----------------------------------------------------------------------------
Função:	Função que abre um diretório existente no disco.
-----------------------------------------------------------------------------*/
int opendir2 () {
	if(inicializada == FALSE)
		inicializar();
	return -1;
}

/*-----------------------------------------------------------------------------
Função:	Função usada para ler as entradas de um diretório.
-----------------------------------------------------------------------------*/
int readdir2 (DIRENT2 *dentry) {
	if(inicializada == FALSE)
		inicializar();
	return -1;
}

/*-----------------------------------------------------------------------------
Função:	Função usada para fechar um diretório.
-----------------------------------------------------------------------------*/
int closedir2 () {
	if(inicializada == FALSE)
		inicializar();
	return -1;
}

/*-----------------------------------------------------------------------------
Função:	Função usada para criar um caminho alternativo (softlink)
-----------------------------------------------------------------------------*/
int sln2 (char *linkname, char *filename) {
	if(inicializada == FALSE)
		inicializar();
	return -1;
}

/*-----------------------------------------------------------------------------
Função:	Função usada para criar um caminho alternativo (hardlink)
-----------------------------------------------------------------------------*/
int hln2(char *linkname, char *filename) {
	if(inicializada == FALSE)
		inicializar();
	return -1;
}
