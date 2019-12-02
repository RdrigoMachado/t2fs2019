
/**
*/
#include "../include/apidisk.h"
#include "../include/string.h"
#include "../include/gerenciamento_disco.h"
#include "../include/escrever_arquivos.h"
#include "../include/leitura_arquivos.h"
#include "../include/bitmap2.h"
#include "../include/t2disk.h"
#include "../include/inodes.h"
#include <stdio.h>
#include <stdlib.h>


struct t2fs_inode* diretorio;
int ultima_entrada_lida = 0;

int inicializada     = FALSE;



void inicializar(){
	carregaDadosDisco();
//	diretorio = malloc(sizeof( struct t2fs_inode* ));
	inicializada = TRUE;
}

int retornaFILE2Livre(struct t2fs_record arquivo){
  int index;
  for(index = 0; index < MAXIMO_ARQUIVOS_ABERTOS; index++){
    if(arquivos[index].valido == -1){
      Handle novo;
      novo.posicao_atual = 0;
      novo.valido = 1;
      novo.entrada = arquivo;
      arquivos[index] = novo;
      ultimo_id_utilizado++;
      novo.identificador = ultimo_id_utilizado;
			arquivos_abertos++;
      return ultimo_id_utilizado;
    }
  }
  return -1;
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
	particao_ativa = partition;
	if(inicializada == FALSE)
		inicializar();
	if (formatarParticao(partition, sectors_per_block) == FALHA){
		printf("Nao foi possivel formatar a particao\n");
		return FALHA;
	}

	struct t2fs_inode inode_raiz;
	inode_raiz.blocksFileSize = 0;
	inode_raiz.bytesFileSize  = 0;
	inode_raiz.dataPtr[0]     = -1;
	inode_raiz.dataPtr[1]     = -1;
	inode_raiz.singleIndPtr   = -1;
	inode_raiz.doubleIndPtr   = -1;
	inode_raiz.RefCounter     =  1;

	if( escrever_inode(&inode_raiz) == FALHA){
		printf("Nao foi possivel escrever inode\n");
		return FALHA;
	}
	return SUCESSO;
}

/*-----------------------------------------------------------------------------
Função:	Monta a partição indicada por "partition" no diretório raiz
-----------------------------------------------------------------------------*/
int mount(int partition) {
	if(inicializada == FALSE)
		inicializar();
	carregaDadosParticao(&super_bloco_atual, partition);
	ultima_entrada_lida = 0;
	return SUCESSO;
}

/*-----------------------------------------------------------------------------
Função:	Desmonta a partição atualmente montada, liberando o ponto de montagem.
-----------------------------------------------------------------------------*/
int unmount(void) {
	if(inicializada == FALSE)
		inicializar();
	int index;
	for(index = 0; index < MAXIMO_ARQUIVOS_ABERTOS; index++){
		arquivos[index].valido = -1;
	}
	diretorio = NULL;
	particao_ativa = -1;
	return SUCESSO;
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
	if(diretorio == NULL)	{
		printf("NENHUM DIRETORIO ABERTO, NAO E POSSIVEL CRIAR ARQUIVO\n");
		return FALHA;
	}
	if(arquivos_abertos >= MAXIMO_ARQUIVOS_ABERTOS){
		printf("NUMERO MAXIMO DE ARQUIVOS ABERTOS, FECHE UM ARQUIVO E TENTE DENOVO\n");
		return FALHA;
	}

	struct t2fs_record nova_entrada;
	nova_entrada.TypeVal = TYPEVAL_REGULAR;
	int index = 0;
	char *aux = filename;
	int menor = FALSE;
	for(index = 50; index >= 0; index--){
		if(*aux == '\0'){
			menor = TRUE;
		}
		aux++;
	}
	if(menor == FALSE){
		return FALHA;
	}

	struct t2fs_inode inode1;
	inode1.bytesFileSize = 0;
	inode1.blocksFileSize = 0;
	inode1.dataPtr[0] = -1;
	inode1.dataPtr[1] = -1;
	inode1.doubleIndPtr = -1;
	inode1.singleIndPtr = -1;
	inode1.doubleIndPtr = -1;
	inode1.RefCounter = 1;

	nova_entrada.inodeNumber  = escrever_inode(&inode1);
	copiarString(nova_entrada.name, filename);

	int continuar = TRUE;
	Handle diretorio_raiz;
//	copiarMemoria((unsigned char*) &diretorio_raiz.arquivo, diretorio, sizeof(struct t2fs_inode));
	diretorio_raiz.arquivo = *diretorio;
	diretorio_raiz.posicao_atual = 0;
	int tamanho_record  =  sizeof(struct t2fs_record);
	struct t2fs_record record_aux;
	int ja_existe_mesmo_nome = FALSE;
	while(continuar == TRUE ){
		if(leitura_arquivo((unsigned char*) &record_aux, tamanho_record, &diretorio_raiz) == FALHA){
			printf("Nenhum arquivo com o mesmo nome\n");
			continuar = FALSE;
			ja_existe_mesmo_nome = FALSE;
		}
		if(verificaSePalavrasSaoIguais(filename, record_aux.name) == TRUE){
			printf("Ja existe arquivo com mesmo nome\n");
			ja_existe_mesmo_nome = TRUE;
			continuar = FALSE;
		}
	}
	if(ja_existe_mesmo_nome == TRUE){
		return FALHA;
	}
	diretorio_raiz.posicao_atual = diretorio_raiz.arquivo.bytesFileSize;
	printf("Direotrio block usado %d", diretorio_raiz.arquivo.blocksFileSize);
	escrita_arquivo((unsigned char*) &nova_entrada, tamanho_record, &diretorio_raiz);
	copiarMemoria((unsigned char*) diretorio, (unsigned char*) &diretorio_raiz.arquivo,sizeof(struct t2fs_inode));

	int retorno = retornaFILE2Livre(nova_entrada);
	for(index = 0; index < MAXIMO_ARQUIVOS_ABERTOS; index++){
		if (arquivos[index].identificador == retorno)
		{
			 copiarMemoria((char*) &(arquivos[index].arquivo), (char*) &inode1, sizeof(struct t2fs_inode));
		}
	}
	return 0;
}



/*-----------------------------------------------------------------------------
Função:	Função usada para remover (apagar) um arquivo do disco.
-----------------------------------------------------------------------------*/
int delete2 (char *filename) {
	if(inicializada == FALSE)
		inicializar();


	int continuar = TRUE;
	Handle diretorio_raiz;
	diretorio_raiz.arquivo = *diretorio;
	diretorio_raiz.posicao_atual = 0;
	int tamanho_record  =  sizeof(struct t2fs_record);
	struct t2fs_record aux;
	int ja_existe_mesmo_nome = FALSE;

	while(continuar == TRUE ){
		if(leitura_arquivo((unsigned char*) &aux, tamanho_record, &diretorio_raiz) == FALHA){
			printf("Nenhum arquivo com o mesmo nome\n");
			continuar = FALSE;
			ja_existe_mesmo_nome = FALSE;
		}
		if(verificaSePalavrasSaoIguais(filename, aux.name) == TRUE){
			printf("Ja existe arquivo com mesmo nome\n");
			ja_existe_mesmo_nome = TRUE;
			continuar = FALSE;
		}
	}



	return -1;
}

/*-----------------------------------------------------------------------------
Função:	Função que abre um arquivo existente no disco.
-----------------------------------------------------------------------------*/
FILE2 open2 (char *filename) {
	if(inicializada == FALSE)
		inicializar();
	if(diretorio == NULL)
		return -1;

	if(arquivos_abertos == MAXIMO_ARQUIVOS_ABERTOS ){
		return -1;
	}


	int continuar = TRUE;
	Handle diretorio_raiz;
	diretorio_raiz.arquivo = *diretorio;
	diretorio_raiz.posicao_atual = 0;
	int tamanho_record  =  sizeof(struct t2fs_record);
	struct t2fs_record aux;
	int encontrado = FALSE;

	while(continuar == TRUE ){
		if(leitura_arquivo((unsigned char*) &aux, tamanho_record, &diretorio_raiz) == FALHA){
			printf("Nenhum arquivo com o mesmo nome\n");
			continuar = FALSE;
			encontrado = FALSE;
		}
		if(verificaSePalavrasSaoIguais(filename, aux.name) == TRUE){
			printf("Ja existe arquivo com mesmo nome\n");
			encontrado = TRUE;
			continuar = FALSE;
		}
	}
	int retorno = 	 retornaFILE2Livre(aux);
	int index;
	for(index = 0; index<MAXIMO_ARQUIVOS_ABERTOS; index++){
		if(arquivos[index].identificador == retorno)
			ler_inode(&(arquivos[index].arquivo), arquivos[index].entrada.inodeNumber);
	}
	if(encontrado == TRUE){
		return retorno;

	}
	return -1;
}

/*-----------------------------------------------------------------------------
Função:	Função usada para fechar um arquivo.
-----------------------------------------------------------------------------*/
int close2 (FILE2 handle) {
	if(inicializada == FALSE)
		inicializar();
	if(arquivos_abertos == 0)
		return -1;

	int fechou = FALSE;
	int index;
	for(index = 0; index <MAXIMO_ARQUIVOS_ABERTOS; index++){
		if(arquivos[index].identificador == handle){
				arquivos[index].valido = -1;
				fechou = TRUE;
				arquivos_abertos--;
				return 0;
		}
	}

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
	Handle* handle_local = NULL;
	int index;
	for(index = 0; index <MAXIMO_ARQUIVOS_ABERTOS; index++){
		if(arquivos[index].identificador == handle){
				handle_local = &arquivos[index];
		}
	}
	if(handle_local == NULL)
		return -1;

	return escrita_arquivo((char*)buffer, size, handle_local);
}

/*-----------------------------------------------------------------------------
Função:	Função que abre um diretório existente no disco.
-----------------------------------------------------------------------------*/
int opendir2 () {
	if(inicializada == FALSE)
		inicializar();
	if(diretorio == NULL){
		diretorio = malloc(sizeof(struct t2fs_inode));
		if (ler_inode(diretorio, 0) == FALHA){
			printf("FALHA AO LER DIRETORIO RAIZ\n");
			return FALHA;
		}
	}else{
		printf("DIRETORIO JA ESTA MONTADO\n");
	}
	printf("SUCESSO AO MONTAR DIRETORIO\n" );
	if(diretorio == NULL){
		printf("COMO ASSIM???\n");
		return FALHA;
	}
	printf("AAAAAAA %d\n",diretorio->blocksFileSize );
	return SUCESSO;
}

/*-----------------------------------------------------------------------------
Função:	Função usada para ler as entradas de um diretório.
-----------------------------------------------------------------------------*/
int readdir2 (DIRENT2 *dentry) {
	if(inicializada == FALSE)
		inicializar();
	struct t2fs_record aux;
	int tamanho = sizeof(struct t2fs_record);

	Handle diretorio_raiz;
	diretorio_raiz.arquivo = *diretorio;
	printf("diretorio byte size %d\n", diretorio_raiz.arquivo.bytesFileSize);
	diretorio_raiz.posicao_atual = ultima_entrada_lida * tamanho;
	int continuar = TRUE;
	while (leitura_arquivo((unsigned char*) &aux, tamanho, &diretorio_raiz) != FALHA && continuar == TRUE){
		printf("entrei\n" );
		ultima_entrada_lida++;
		if(aux.TypeVal != TYPEVAL_INVALIDO){
			continuar = FALSE;
			copiarMemoria((char*) dentry->name, (char*) aux.name, MAX_FILE_NAME_SIZE+1);
			dentry->fileType = TYPEVAL_REGULAR;
			Handle handle;
			ler_inode(&(handle.arquivo), aux.inodeNumber);
			printf("inode numeber %d\n",aux.inodeNumber );
			dentry->fileSize  = handle.arquivo.bytesFileSize;
			return 0;
		}

	}
printf("passei largado \n" );
	return -1;
}

/*-----------------------------------------------------------------------------
Função:	Função usada para fechar um diretório.
-----------------------------------------------------------------------------*/
int closedir2 () {
	if(inicializada == FALSE)
		inicializar();
	if(diretorio == NULL)
		return -1;
	int index;
	for(index =0; index < MAXIMO_ARQUIVOS_ABERTOS; index++){
		arquivos[index].valido = -1;
	}
	arquivos_abertos = 0;
	diretorio = NULL;
	return 0;
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

int main(){

	//format2(0, 4);
	mount(0);
	opendir2();

	// if(create2("chegadisso")  >= 0)
	// 	printf("Parece que criou\n");
	// if(create2("chegadisso2")  >= 0)
	// 	printf("Parece que criou\n");
	// if(create2("chegadisso3")  >= 0)
	// 	printf("Parece que criou\n");

	DIRENT2 *dentry = malloc(sizeof(DIRENT2));
	readdir2 (dentry);
	printf("dentry name %s\n", dentry->name );
	DIRENT2 *dentry2 = malloc(sizeof(DIRENT2));
	readdir2 (dentry2);
	printf("dentry name %s\n", dentry2->name );
	DIRENT2 *dentry3 = malloc(sizeof(DIRENT2));
	readdir2 (dentry3);
	printf("dentry name %s\n", dentry3->name );

	printf("arquivos abertos %d\n",arquivos_abertos );
	closedir2();
	if(diretorio == NULL)
	printf("Diretorio fechado\n" );
	printf("arquivos abertos %d\n",arquivos_abertos );
	return 0;
}
