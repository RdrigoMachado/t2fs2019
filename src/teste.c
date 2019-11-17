#include "../include/apidisk.h"
#include "../include/string.h"
#include "../include/bitmap2.h"
#include "../include/ler_e_escrever_no_disco.h"

#include <stdio.h>
#include <stdlib.h>

int main(){

  int index;
  for(index = 0; index<4; index++){
    formatarParticao(index, 4);
    leSetorEPreencheStructSuperBloco(&super_bloco_atual, index);

    printf("=====================================\n");
    printf("=====================================\n");
  }



  return 0;
}
