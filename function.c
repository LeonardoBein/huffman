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
  *tamA = tamF;
  return arvore_final;
}

void create_bi(sNo *arvore, int caminhoED, unsigned int bi,int tamBI){
  if (tamBI >=0) {
    bi = caminhoED << (30-tamBI) | bi;
    if (arvore->letra != '\0') {
      tamBI++;
      arvore->valor_bi = bi;
      arvore->tamanho_bi = tamBI;
    }
  }
  tamBI++;
  if(arvore->left != NULL) create_bi(arvore->left, 0, bi, tamBI);
  if(arvore->right != NULL) create_bi(arvore->right, 1, bi, tamBI);

}


void get_bin(sNo *a, char letra, int echo,int *altura,int *valor_bi){
  if (!(a->left || a->right) && a->letra == letra){
		// return a->valor_bi>>31-a->tamanho_bi;
    if (echo == 1) {
      printf("%c: ",a->letra );
      for (int i = a->tamanho_bi; i > 0; i--) {
        printf("%d", a->valor_bi>>30-a->tamanho_bi+i & 1 );
      }
      printf("\t%d\n",a->valor_bi>>31-a->tamanho_bi);
    }
    if (altura != 0x0) {
      *altura = a->tamanho_bi;
    }
    if (valor_bi != 0x0) {
      *valor_bi = a->valor_bi;
    }
    // printf("valor_bi:%d\n",a->valor_bi>>32-a->tamanho_bi );
	}
	if(a->left != NULL) get_bin(a->left, letra,echo, altura,valor_bi);
	if(a->right != NULL) get_bin(a->right, letra,echo,altura,valor_bi);
}


void binarios(sNo *raiz, int freq[256]){
	for (int i = 0; i < 256; i++) {
    if (freq[i]>0) {
      get_bin(raiz,i,1,0,0);
    }
	}
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

int tamanho_bitstream(sNo *raiz,int *freq){
  int tam=0,anx=0;
  for (int i = 0; i < 256; i++) {
    if (freq[i]>0) {
      get_bin(raiz,i,0,&anx,0);
      tam += anx*freq[i];
      anx=0;
    }
	}
  return tam;
}

int compactar(sNo *raiz, FILE *ARQorig, FILE *ARQdest,int echo){
  unsigned char byte='\0';
  char c;
  unsigned int altura,valor_bi;
  int j,i=0,k=0;

  j=7;
  while ((c = fgetc(ARQorig)) != EOF) {
    altura = valor_bi = 0;
    get_bin(raiz, c, 0, &altura, &valor_bi);

    for (i = altura; i > 0; i--,j--) {
      if (j < 0) {
        j=7;
        if (fputc(byte,ARQdest) == EOF) { return 1; } //erro
        if(echo){
          printf("\t%x\n", byte );
        }
        byte = 0b00000000;
      }

      if (echo) {
        printf("%d",valor_bi>>30-altura+i & 1 );
      }

      if ((valor_bi>>30-altura+i & 1) == 1) {
        byte = (1 << j) | byte;
      }
    }
  }
  if (echo) {
    printf("\t%x\n",byte);
  }
  //escrever o ultimo byte.
  if (fputc(byte,ARQdest) == EOF) { return 1; } //erro

  rewind(ARQorig);
  return 0;
}



int descompactar(sNo *arvore,int tamA, FILE *arquivo_origem , FILE *arquivo_destino, int tamL){
  unsigned int codigo;
  char codigo_entrada;
  int tamanho_cod,i,j;

  while(!feof(arquivo_origem)){
        codigo_entrada = fgetc(arquivo_origem);
        codigo = 0;
        tamanho_cod = 0;

        for (i = 7; i >= 0 && tamL != 0; i--){
          codigo = (codigo<<1);
          tamL--;
          tamanho_cod++;
          codigo = codigo | ((codigo_entrada>>i) & 1);

          for ( j = 0; j < tamA+1; j++){
            if (codigo == arvore[j].valor_bi>>31-arvore[j].tamanho_bi && arvore[j].letra != '\0' && tamanho_cod == arvore[j].tamanho_bi){
              fputc(arvore[j].letra,arquivo_destino);
              // printf("%c",arvore[j].letra );
              codigo = 0;
              tamanho_cod = (0);
              j=tamA+1;
            }
          }

          if(i == 0){
            codigo_entrada = fgetc(arquivo_origem);
            i = 8 ;
          }
        }
      }
  return 0;
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
int vazia(sNo *a){
    return a == NULL;
}
void imprime(sNo *a){
	if (vazia(a))
		printf ("<>");
	else {
		printf ("<%c (%d)",a->letra, a->valor);
		imprime(a->left);
		imprime(a->right);
		printf (">");
	}
}
