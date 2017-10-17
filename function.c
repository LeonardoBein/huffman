#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"


void help() {
  printf("Compactador de arquivo.txt: %s %s\n", NAME_SOFTWARE ,VERSION );
  printf("Uso: huffman -[option] orig dest \n" );
  printf("\nOpcões:\n");
  printf("-c\tcompacta um arquivo.\n");
  printf("-x\tdescompacta um arquivo compactado por %s \n", NAME_SOFTWARE);
  printf("-h\timprime esta ajuda\n\n");
  exit(1);
}
int *le_arq(FILE *arq){
  char c;
  int *freq,i;
  freq = (int*)calloc(256,sizeof(int)*256);
  while( (c=fgetc(arq))!= EOF ){
    if (c >= 0 && c < 256) {
    // printf("'%c':%d\n", c,c);
      freq[c]++;
    }
    else{
      printf("Caracter '%c': %d nao suportado!\n",c,c );
      // exit(1);
    }
    }
  rewind(arq);
  return freq;
}

sNo *cria_folhas(int freq[256] ,int *p){
  int i,j=1,n=0;
  sNo *folhas;
  for ( i = 0, j = 0; i < 256; i++) {
    if (freq[i] > 0) {
      j++;
    }
  }
  folhas = (sNo *)calloc(j,sizeof(sNo)*j);

  for (i = 0; i < 256; i++) {
    if (freq[i] > 0) {
        folhas[n].letra = i;
        folhas[n].valor = freq[i];
        folhas[n].left = inicializa();
        folhas[n].right = inicializa();
        n++;
    }
  }
  *p = j;
  ordenar(folhas, j,'+');
  return folhas;
}

sFreq_entry *criar_freq_table(int freq[256],int *r){
  char c;
  int j=1,n=0,i;
  sFreq_entry *freqs;
  // table = (sFreq_table *)calloc(1,sizeof(sFreq_table));

  for ( i = 0, j = 0; i < 256; i++) {
    if (freq[i] > 0) {
      j++;
    }
  }

  freqs = (sFreq_entry *)calloc(j,sizeof(sFreq_entry)*j);

  for (i = 0; i < 256; i++) {
    if (freq[i] > 0) {
        freqs[n].data = i;
        freqs[n].freq = freq[i];
        n++;
    }
  }
  *r = j;
  return freqs;
}

char *aplicar_huffman(FILE *arq, sNo *arvore){
  char *final, *buffer,c;
  int len, lenBuffer, more, i;

    len = lenBuffer = altura(arvore);
    final = (char *)calloc(len,sizeof(char)*len);
    buffer = (char *)calloc(len,sizeof(char)*len);
    while ((c = fgetc(arq)) != EOF) {
      buffer[0] = '\0';
      if (pegaCodigo( arvore, c , buffer , 0 ) ) {
        if(strlen(final) == len   || strlen(final)+strlen(buffer) >= len){
          len += lenBuffer;
          // printf("aumentar memoria final [%c] de %d para %d\n",c,strlen(final),len);
          if ((final = (char *)realloc(final, sizeof(char)*len)) == NULL) {
            printf("Error de memoria\n");
            exit(1);
          }
        }
        strcat(final,buffer);
      }
    }
    free(buffer);
    rewind(arq);
    return final;
}


sNo *inicializa(void){
  return NULL;
}
sNo *cria(int valor, sNo *esq, sNo *dir){
  sNo *no;
  no = (sNo *) calloc(1,sizeof(sNo)); //devolve o endereco da nova memoria criada
  if(no != NULL){
    no->left = esq;
    no->right = dir;
    no->valor = valor;
    no->letra = '\0';
    return no;
  }
  printf("error\n" );
  exit(1);
}
void ordenar(sNo *lista, int tam, char tipo){
  sNo swap;
  int min, i,j;

  for (i = 0; i < tam; i++) {
    min = i;
    for (j = (i+1); j < tam; j++) {
      if (tipo == '+') {
        if(lista[j].valor < lista[min].valor){
          min = j;
        }
      }
      else{
        if(lista[j].valor > lista[min].valor){
          min = j;
        }
      }
    }
    if (i != min) {
      swap = lista[i];
      lista[i] = lista[min];
      lista[min] = swap;
    }
  }

}
int vazio(sNo *a){
  return a==NULL;
}
sNo *huffman(sNo *arvore, int *tamA){
  int tamF, i =1, j =2, tam, k;
  sNo *arvore_final, *no;

  tam = *tamA;
  tamF = 2*tam-1;
  arvore_final = (sNo *) calloc(tamF,sizeof(sNo)*(tamF) );
  arvore = (sNo *) realloc( arvore , sizeof(sNo)*(++tam) );
  no = (sNo *) calloc(1,sizeof(sNo));


  while(tam != 1){
    arvore_final[i] = arvore[0];
    arvore_final[j] = arvore[1];
    arvore[0].valor = -1;
    arvore[1].valor = -1;

    no = cria( (arvore_final[i].valor+arvore_final[j].valor), &arvore_final[i], &arvore_final[j]);
    arvore[tam-1] = *no;
    ordenar(arvore,tam,'-');
    tam -= 2;
    arvore = (sNo *) realloc( arvore , sizeof(sNo)*(tam) );
    ordenar(arvore,tam,'+');
    if (tam != 1) {
      arvore = (sNo *) realloc(arvore,sizeof(sNo)*(++tam));
      i += 2;
      j += 2;
    }
  }
  arvore_final[0] = arvore[0];
  free(arvore);
  *tamA = tam;
  return arvore_final;
}
int pegaCodigo(sNo *n, char c, char *codigo, int tamanho){
    // printf("%c==%c\n", n->letra,c );
    if (!(n->left || n->right) && n->letra == c)
    {
        codigo[tamanho] = '\0';
        return 1;
    }
    else{
        int encontrado = 0;

        if (n->left){
            codigo[tamanho] = '0';
            encontrado = pegaCodigo(n->left, c, codigo, tamanho + 1);
        }
        if (!encontrado && n->right){
            codigo[tamanho] = '1';
            encontrado = pegaCodigo(n->right, c, codigo, tamanho + 1);
        }
        if (!encontrado){
            codigo[tamanho] = '\0';
        }
        return encontrado;
    }
}

char pegaChar(sNo *no, char *string, int p){
    if (string[p] != ' ') {
      if (no->letra != '\0') {
        return no->letra;
      }
      if (string[p] == '1') {
        string[p] = ' ';
        return pegaChar(no->right,string,p+1);
      }
      if (string[p] == '0') {
        string[p] = ' ';
        return pegaChar(no->left,string,p+1);
      }
    }
    if (string[p] == '\0') {
      return '\0';
    }
    else{
     return pegaChar(no,string,p+1);
   }
}

void criarFrase(FILE *arq, sNo *no, char *string){
  char *frase_Orig, c;
  rewind(arq);
  while((c = pegaChar(no,string,0)) != '\0'){
    // printf("%s\n",string );
    fputc(c,arq);
  }
  rewind(arq);
}
// retorna a altura da arvore
int altura (sNo *a){
	int ae, ad;

	if (a == NULL)
		return -1; // arvore vazia tem altura -1
	else if (a->left == NULL && a->right == NULL)
		return 0; // arvore com apenas raiz tem altura 0
	else {
		ae = altura (a->left);
		ad = altura (a->right);

		if (ae > ad)
			return 1 + ae;
		else
			return 1 + ad;
	}
}

sHeader *criar_header(int Bitstream,int num){
  sHeader *header;
  header = (sHeader*)calloc(1,sizeof(sHeader));

  strcpy(header[0].name,"Lb" );
  // printf("%s\n", header[0].name);
  // header[0].dados = dividir_int(tamL,tamA);
  header[0].tamBitstream = Bitstream;
  header[0].num_items = num;
  return header;
}

char *compactaString(char *string, int *tamL){
  int i,j ,tam, tamBits = 8 , posicao,anx=0, anx2=0;
  char  *compactacao;
  tam = strlen(string)/tamBits;

  if (tam == 0) {tam = 1;}
  else if(strlen(string)%tamBits != 0){tam++;}
  tam++;

  compactacao = (char  *) calloc(tam,sizeof(char)*tam);

  for ( i = 1, posicao = 0 ; i < tam; i++) {
    for ( j = (tamBits-1); j >= 0 ; j--,++posicao) {
      if (string[posicao] == '1' ) {
        anx += ipow(2,j);
      }
      else if (string[posicao] == '\0') {
        compactacao[0] = j+1;
        j=-1;
      }
    }
    compactacao[i] = anx;
    anx = 0;
  }
  *tamL = tam;
  return compactacao;
}
char *descompactaString( char *string, int tamL){
  int tam,tamDes, Ianx, i;
  char *descompactacao, *anx;
  tam = tamL;
  tamDes = 8*(tam-1);
  descompactacao = (char *)calloc(tamDes,sizeof(char)*tamDes);

  for ( i = 1; i < tam; i++) {
    anx = Var_Char_Bin(string[i]);
    strcat(descompactacao,anx);
  }
  Ianx = string[0];
  if (Ianx) {
    descompactacao[tamDes-Ianx] = '\0';
  }
  return descompactacao;
}

char *Var_Char_Bin(int c){
  char *bi, anx;
  int i,posicao;
  bi = (char *)calloc(8,sizeof(char)*8);
  for ( i = 7, posicao=0; i >= 0; i--,posicao++) {
    if (c & ipow(2,i)) {
      anx = '1';
    }
    else{
      anx = '0';
    }
      bi[posicao] = anx;
  }
  return bi;
}
//funcao a ser ussada
int dividir_int(int a, int b){
  char *number;
  int num=0, i,posicao,anx;
  number = Var_Char_Bin(a);
  strcat(number,Var_Char_Bin(b));
  for ( i = 15, posicao=0; i >= 0 ; i--,posicao++) {
    if (number[posicao] == '1') {
      num+=ipow(2,i);
    }
  }
  free(number);
  return num;
}
int *get_int_dividido(int a){
  int *vetor,i,j;
  vetor = (int *) calloc(2,sizeof(int)*2);
  for ( i = 15,j=7; i > 7; i--,j--) {
    if (a & ipow(2,i)) {
      vetor[0] += ipow(2,j);
    }
  }

  for (i = 7; i >=0; i--) {
    if (a & ipow(2,i)) {
      vetor[1] += ipow(2,i);
    }
  }
  return vetor;
}
int ipow(int base, int exp){
    int result = 1;
    while (exp)
    {
        if (exp & 1)
            result *= base;
        exp >>= 1;
        base *= base;
    }

    return result;
}
