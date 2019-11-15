#ifndef __LER_ESCREVER___
#define __LER_ESCREVER___

int retornaSetorDoSuperBloco(int numero_particao);
int geraSuperBlocoESalva(int numero_particao, int setores_por_bloco);
void leSetorEPreencheStructSuperBloco(SuperBloco *super_bloco, int numero_particao);
void le_MBR_Preenche_Dados_Particoes();
int formatarParticao(int numero_particao, int setores_por_bloco);

#endif
