void escreve(buffer, arquivo)
if ((arquivo.blocks_file_size) < maior_bloco_caso_1){
  int posicao = getbitmap(dado);
  setBitmap2(dado, posicao, 1);
  write_sector(posicao, buffer);
  arquivo.ponteiro_simples[blocks_file_size] = posicao;
  arquivo.blocks_file_size++;}
else if ((blocks_file_size) < maior_bloco_caso_2){
  if(ponteiro_indirecao == NULL){
    int posicao = getbitmap(dado);
    setBitmap2(dado, posicao, 1);
    arquivo.ponteiro_indirecao = posicao;
  }
  unsigned char buffer_ponteiros[tamanho_setor];
  read_sector(arquivo.ponteiro_indirecao, buffer_ponteiros);
  int posicao = getbitmap(dado);
  setBitmap2(dado, posicao, 1);
  buffer_ponteiros[posicao_proximo_ponteiro] = posicao;
  write_sector(posicao, buffer);
  write_sector(arquivo.ponteiro_indirecao, buffer_ponteiros);
  blocks_file_size++;}
else if ((blocks_file_size) < maior_bloco_caso_3){
nao fiz
}
