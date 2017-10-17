#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"


int main(int argc, char const *argv[]) {
  char *frase_bi, *frase, *frase_Orig, c, option;
  unsigned char *compactado;
  int tamL,tam,tamF, tamA , i, error;
  int *vetor;
  char *buffer,*dest, *orig;
  sNo *folhas, *arvore_final;
  sHeader *header;
  sFreq_entry *table;
  FILE *arq;

  //huffman -c orig dest
  if (argc == 4) {
    //compactar
    if (argv[1][1] == 'c') {
      tam = strlen(argv[2]);
      orig = (char *)malloc(tam);
      tam = strlen(argv[3]) + 5;
      dest = (char *)malloc(tam);
      strcpy(orig,argv[2]);
      strcpy(dest,argv[3]);
      strcat(dest,".hufx");
      option = 'c';
    }

    //descompactar
    else if (argv[1][1] == 'x') {
      tam = strlen(argv[2]);
      orig = (char *)malloc(tam);
      tam = strlen(argv[3]);
      dest = (char *)malloc(tam);
      strcpy(orig,argv[2]);
      strcpy(dest,argv[3]);
      option = 'x';
    }
    else{
      help();
    }

  }



  if (option == 'c') {
    if ((arq = fopen(orig,"r")) == NULL) {
      printf("Erro na abertura do arquivo %s",orig);
      exit(1);
    }
    vetor = le_arq(arq);
    folhas = cria_folhas(vetor,&tamA);
    table = criar_freq_table(vetor,&tamF);
    arvore_final = huffman(folhas,&tamA);
    compactado = compactaString(aplicar_huffman(arq, arvore_final), &tamL);
    fclose(arq);


    //gravando
    if((arq = fopen(dest, "wb")) == NULL){
      printf("Erro na criacao do arquivo %s",dest);
      exit(1);
    }

    header = criar_header(tamL,tamF);
    error = 0;
    // printf("%d\n",sizeof(sFreq_table) );
    // printf("%d\n",sizeof(sFreq_entry) );

    if(fwrite(header, sizeof(sHeader), 1,arq) != 1)
      error = 1;
    if (fwrite(table,sizeof(sFreq_entry),header[0].num_items,arq) != header[0].num_items)
      error = 1;
    if (fwrite(compactado,1,header[0].tamBitstream,arq) != header[0].tamBitstream)
      error = 1;

    if (error == 1) {
      printf("Erro na compactacao do arquivo %s\nMotivo: erro de escrita no arquivo %s",orig,dest);
      exit(1);
    }

    fclose(arq);
    free(header);
    free(orig);
    free(dest);
    free(compactado);
    free(arvore_final);
    free(table);
    return 0;
  }
  //extracao
  else if (option == 'x') {

    header = (sHeader*)calloc(1,sizeof(sHeader));
    // table = (sFreq_table*)calloc(1, sizeof(sFreq_table));
    // table[0].freqs = (sFreq_entry *)calloc(73,sizeof(sFreq_entry)*73);
    error = 0;
    if((arq = fopen(orig, "rb")) == NULL){
      printf("Erro na abertura do arquivo %s",orig);
      exit(1);
    }
    if(fread(header, sizeof(sHeader), 1,arq) != 1){
      printf("Erro na leitura do arquivo");
      exit(1);
    }
    // printf("%s\n",header[0].name );
    if (header[0].name[0] != 'L' && header[0].name[0] != 'b' ) {
      printf("Arquivo corrompido '%s' ou nao suportado!\n", orig);
      // printf("%s\n",header[0].name );
      exit(1);
    }
    tamF = header[0].num_items;
    table = (sFreq_entry *)calloc(tamF,sizeof(sFreq_entry)*tamF);

    if(fread(table, sizeof(sFreq_entry), tamF,arq) != tamF){
      printf("Arquivo corrompido %s", orig);
      exit(1);
    }

    tamL = header[0].tamBitstream;
    if ((compactado = (char *)calloc(tamL,sizeof(char)*tamL)) == NULL) {
      printf("Erro de alocacao dinamica de memoria\n");
      exit(1);
    }
    if((i = fread(compactado, 1, tamL,arq)) != tamL){
      printf("Arquivo corrompido %s", orig);
      exit(1);
    }
    fclose(arq);

    vetor = (int*)calloc(256,sizeof(int)*256);
    for ( i = 0; i < tamF; i++) {
      vetor[table[i].data] = table[i].freq;
    }
    folhas = cria_folhas(vetor,&tamA);
    arvore_final = huffman(folhas,&tamA);
    frase_bi = descompactaString(compactado,tamL);

    if((arq = fopen(dest, "w")) == NULL){
      printf("Erro na abertura do arquivo %s",dest);
      exit(1);
    }

    criarFrase(arq,arvore_final,frase_bi);

    fclose(arq);
    free(header);
    free(orig);
    free(dest);
    free(frase_bi);
    free(vetor);
    free(compactado);
    free(arvore_final);
    free(table);
    return 0;
  }
  else{
    help();
  }
  return 0;
}
