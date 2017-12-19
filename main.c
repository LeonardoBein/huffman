#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"


int main(int argc, char const *argv[]) {
  char *frase_bi, *frase, *frase_Orig, c, option, didatico=0;
  unsigned char *compactado;
  int tamL,tam,tamF, tamA , i, error;
  int *vetor;
  char *bytes,*dest, *orig;
  sNo *folhas, *arvore;
  sHeader *header;
  sFreq_entry *table;
  FILE *arquivo_origem,*arquivo_destino;

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

    if(argv[1][2] == 'd'){
      didatico = 1;
    }

  }


  if (option == 'c') {
    if ((arquivo_origem = fopen(orig,"r")) == NULL) {
      printf("Erro na abertura do arquivo %s",orig);
      exit(1);
    }
    vetor = le_arq(arquivo_origem);
    folhas = cria_folhas(vetor,&tamA);
    if (didatico) {
      printf("\nFrequência\n" );
      for (i = 0; i < tamA; i++) {
        printf("'%c':%d\n",folhas[i].letra,folhas[i].valor );
      }
    }
    table = criar_freq_table(vetor,&tamF);

    arvore = huffman(folhas,&tamA);

    create_bi(arvore,0,0,-1);


    if (didatico) {
      printf("\nTabela de Símbolos\n" );
      binarios(arvore,vetor);
      printf("\nArvore\n\n" );
      imprime(arvore);
      printf("\n");
    }

    //gravando
    if((arquivo_destino = fopen(dest, "wb")) == NULL){
     printf("Erro na criacao do arquivo %s",dest);
    exit(1);
    }


    header = criar_header(tamanho_bitstream(arvore,vetor),tamF);
    error = 0;
    // printf("%d\n",sizeof(sFreq_table) );
    // printf("%d\n",sizeof(sFreq_entry) );

    // printf("freq: %d \tbits: %d\n",header->num_items,header->tamBitstream);


    if(fwrite(header, sizeof(sHeader), 1,arquivo_destino) != 1)
      error = 1;
    if (fwrite(table,sizeof(sFreq_entry),header[0].num_items,arquivo_destino) != header[0].num_items)
      error = 1;
    if (didatico) {
      printf("\nBytes de Saída\n" );
    }
    if(compactar(arvore, arquivo_origem, arquivo_destino,didatico))
      error = 1;


    if (error == 1) {
      printf("Erro na compactacao do arquivo %s\nMotivo: erro de escrita no arquivo %s",orig,dest);
      exit(1);
    }

    fclose(arquivo_origem);
    fclose(arquivo_destino);
    free(header);
    free(orig);
    free(dest);
    free(arvore);
    free(table);
    return 0;
  }
  // //extracao
  else if (option == 'x') {

    header = (sHeader*)calloc(1,sizeof(sHeader));
    error = 0;

    if((arquivo_origem = fopen(orig, "rb")) == NULL){
      printf("Erro na abertura do arquivo %s",orig);
      exit(1);
    }

    if(fread(header, sizeof(sHeader), 1,arquivo_origem) != 1){
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

    if(fread(table, sizeof(sFreq_entry), tamF,arquivo_origem) != tamF){
      printf("Arquivo corrompido %s", orig);
      exit(1);
    }

    tamL = header[0].tamBitstream;

    vetor = (int*)calloc(256,sizeof(int)*256);
    for ( i = 0; i < tamF; i++) {
      vetor[table[i].data] = table[i].freq;
    }
    folhas = cria_folhas(vetor,&tamA);
    arvore = huffman(folhas,&tamA);
    create_bi(arvore,0,0,-1);

    if((arquivo_destino = fopen(dest, "w")) == NULL){
      printf("Erro na abertura do arquivo %s",dest);
      exit(1);
    }

    if(descompactar(arvore,tamA,arquivo_origem,arquivo_destino,tamL)){
      printf("Error\n" );
      exit(1);
    }


    fclose(arquivo_origem);
    fclose(arquivo_destino);
    free(header);
    free(orig);
    free(dest);
    free(vetor);
    free(arvore);
    free(table);
    return 0;
  }
  else{
    help();
  }
  return 0;
}
