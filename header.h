#define VERSION "1.0"
#define NAME_SOFTWARE "huffman"


struct sNo{
  char letra;
  int valor;
  struct sNo *left;
  struct sNo *right;
};

typedef struct sNo sNo;

struct sHeader{
  char name[3];
  unsigned long int tamBitstream;
  unsigned short int num_items;
};
typedef struct sHeader sHeader;


struct freq_entry {
  unsigned short int freq;
  char data;
};
typedef struct freq_entry sFreq_entry;

void help();

sNo *inicializa();
sNo *cria(int valor, sNo *esq, sNo *dir);
sNo *cria_folhas(int freq[225] ,int *p);
sFreq_entry *criar_freq_table(int freq[256],int *r);

void ordenar(sNo *lista, int tam, char tipo);

int *le_arq(FILE *arq);
sNo *huffman(sNo *arvore, int *tamA);
int vazia(sNo *a);
void imprime(sNo *a);
int pegaCodigo(sNo *n, char c, char *codigo, int tamanho);
char pegaChar(sNo *no, char *string, int p);
char *aplicar_huffman(FILE *arq, sNo *arvore);

void criarFrase(FILE *arq, sNo *no, char *string);

int altura (sNo *a);
sHeader *criar_header(int Bitstream,int num);
char *compactaString(char *string, int *tamL);
char *descompactaString(char  *string, int tamL);
char *Var_Char_Bin(int c);

int dividir_int(int a, int b);
int *get_int_dividido(int a);

int ipow(int base, int exp);
