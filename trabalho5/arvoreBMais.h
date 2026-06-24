#ifndef ARVOREBMAIS_H
#define ARVOREBMAIS_H
 
#include <stdio.h>
 
#ifndef T_BMAIS
#define T_BMAIS 64        
#endif
 
#define TAM_CEP   8
#define ORDEM     (2 * T_BMAIS) 

typedef struct {
	int  n;                   
	int  folha;                  
	char chaves[ORDEM][TAM_CEP];  
	long valores[ORDEM];          
	long filhos[ORDEM + 1];       
	long prox;                    
} NoBMais;
 
typedef struct {
	long raiz;
	long numPaginas;
} CabecalhoBMais;
 
typedef struct {
	FILE *f;
	long  raiz;
	long  numPaginas;
} ArvoreBMais;
 
ArvoreBMais *abmais_criar(const char *nomeArquivo);
ArvoreBMais *abmais_abrir(const char *nomeArquivo);
void         abmais_fechar(ArvoreBMais *t);
 
void abmais_inserir(ArvoreBMais *t, const char *cep, long posicao);
int  abmais_buscar(ArvoreBMais *t, const char *cep, long *posicao);
 

void abmais_percorrer(ArvoreBMais *t,
                      void (*cb)(const char *cep, long posicao, void *ud),
                      void *ud);
 
#endif
