#include <stdlib.h>
#include <string.h>
#include "arvorebmais.h"

static int cmpcep(const char *a, const char *b) {
	return memcmp(a, b, TAM_CEP);
}

static long offsetPagina(long pagina) {
	return (long) sizeof(CabecalhoBMais) + pagina * (long) sizeof(NoBMais);
}

static void lerNo(ArvoreBMais *t, long p, NoBMais *no) {
	fseek(t->f, offsetPagina(p), SEEK_SET);
	if (fread(no, sizeof(NoBMais), 1, t->f) != 1) {
		fprintf(stderr, "Erro lendo pagina %ld\n", p);
		exit(1);
	}
}

static void escreverNo(ArvoreBMais *t, long p, NoBMais *no) {
	fseek(t->f, offsetPagina(p), SEEK_SET);
	fwrite(no, sizeof(NoBMais), 1, t->f);
}

static long alocarNo(ArvoreBMais *t) { return t->numPaginas++; }

static void escreverCabecalho(ArvoreBMais *t) {
	CabecalhoBMais c;
	c.raiz = t->raiz;
	c.numPaginas = t->numPaginas;
	fseek(t->f, 0, SEEK_SET);
	fwrite(&c, sizeof(c), 1, t->f);
}

ArvoreBMais *abmais_criar(const char *nomeArquivo) {
	ArvoreBMais *t = (ArvoreBMais *) malloc(sizeof(ArvoreBMais));
	t->f = fopen(nomeArquivo, "wb+");
	if (!t->f) { free(t); return NULL; }
	t->raiz = 0;
	t->numPaginas = 1;
	escreverCabecalho(t);

	NoBMais raiz;
	memset(&raiz, 0, sizeof(raiz));
	raiz.folha = 1;
	raiz.n = 0;
	raiz.prox = -1;
	escreverNo(t, 0, &raiz);
	return t;
}

ArvoreBMais *abmais_abrir(const char *nomeArquivo) {
	ArvoreBMais *t = (ArvoreBMais *) malloc(sizeof(ArvoreBMais));
	t->f = fopen(nomeArquivo, "rb+");
	if (!t->f) { free(t); return NULL; }
	CabecalhoBMais c;
	fseek(t->f, 0, SEEK_SET);
	if (fread(&c, sizeof(c), 1, t->f) != 1) { fclose(t->f); free(t); return NULL; }
	t->raiz = c.raiz;
	t->numPaginas = c.numPaginas;
	return t;
}

void abmais_fechar(ArvoreBMais *t) {
	escreverCabecalho(t);
	fclose(t->f);
	free(t);
}

typedef struct {
	int  split;
	char router[TAM_CEP];
	long novaPagina;
} Resultado;

static Resultado inserirRec(ArvoreBMais *t, long p, const char *cep, long pos) {
	NoBMais no;
	lerNo(t, p, &no);
	Resultado r;
	r.split = 0;

	if (no.folha) {
		int i = 0;
		while (i < no.n && cmpcep(cep, no.chaves[i]) > 0) i++;

		if (i < no.n && cmpcep(cep, no.chaves[i]) == 0) {
			no.valores[i] = pos;
			escreverNo(t, p, &no);
			return r;
		}

		if (no.n < ORDEM) {
			for (int j = no.n; j > i; j--) {
				memcpy(no.chaves[j], no.chaves[j - 1], TAM_CEP);
				no.valores[j] = no.valores[j - 1];
			}
			memcpy(no.chaves[i], cep, TAM_CEP);
			no.valores[i] = pos;
			no.n++;
			escreverNo(t, p, &no);
			return r;
		}

		char tch[ORDEM + 1][TAM_CEP];
		long tva[ORDEM + 1];
		int m = 0;
		for (int j = 0; j < i; j++) { memcpy(tch[m], no.chaves[j], TAM_CEP); tva[m] = no.valores[j]; m++; }
		memcpy(tch[m], cep, TAM_CEP); tva[m] = pos; m++;
		for (int j = i; j < no.n; j++) { memcpy(tch[m], no.chaves[j], TAM_CEP); tva[m] = no.valores[j]; m++; }

		int esq = (m + 1) / 2;
		int dir = m - esq;

		long zP = alocarNo(t);
		NoBMais z;
		memset(&z, 0, sizeof(z));
		z.folha = 1;
		z.n = dir;
		for (int j = 0; j < dir; j++) { memcpy(z.chaves[j], tch[esq + j], TAM_CEP); z.valores[j] = tva[esq + j]; }
		z.prox = no.prox;

		no.n = esq;
		for (int j = 0; j < esq; j++) { memcpy(no.chaves[j], tch[j], TAM_CEP); no.valores[j] = tva[j]; }
		no.prox = zP;

		escreverNo(t, p, &no);
		escreverNo(t, zP, &z);

		r.split = 1;
		memcpy(r.router, no.chaves[esq - 1], TAM_CEP);
		r.novaPagina = zP;
		return r;
	}

	int i = 0;
	while (i < no.n && cmpcep(cep, no.chaves[i]) > 0) i++;
	long filhoP = no.filhos[i];

	Resultado rf = inserirRec(t, filhoP, cep, pos);
	if (!rf.split) return r;

	lerNo(t, p, &no);

	if (no.n < ORDEM - 1) {
		for (int j = no.n; j > i; j--) {
			memcpy(no.chaves[j], no.chaves[j - 1], TAM_CEP);
		}
		for (int j = no.n + 1; j > i + 1; j--) {
			no.filhos[j] = no.filhos[j - 1];
		}
		memcpy(no.chaves[i], rf.router, TAM_CEP);
		no.filhos[i + 1] = rf.novaPagina;
		no.n++;
		escreverNo(t, p, &no);
		return r;
	}

	char trt[ORDEM][TAM_CEP];
	long tfi[ORDEM + 1];
	int nr = 0, nf = 0;
	for (int j = 0; j < i; j++) { memcpy(trt[nr], no.chaves[j], TAM_CEP); nr++; }
	memcpy(trt[nr], rf.router, TAM_CEP); nr++;
	for (int j = i; j < no.n; j++) { memcpy(trt[nr], no.chaves[j], TAM_CEP); nr++; }
	for (int j = 0; j <= i; j++) { tfi[nf] = no.filhos[j]; nf++; }
	tfi[nf] = rf.novaPagina; nf++;
	for (int j = i + 1; j <= no.n; j++) { tfi[nf] = no.filhos[j]; nf++; }

	int meio = T_BMAIS;

	NoBMais z;
	memset(&z, 0, sizeof(z));
	z.folha = 0;
	z.n = nr - meio - 1;
	for (int j = 0; j < z.n; j++) memcpy(z.chaves[j], trt[meio + 1 + j], TAM_CEP);
	for (int j = 0; j <= z.n; j++) z.filhos[j] = tfi[meio + 1 + j];

	no.n = meio;
	for (int j = 0; j < meio; j++) memcpy(no.chaves[j], trt[j], TAM_CEP);
	for (int j = 0; j <= meio; j++) no.filhos[j] = tfi[j];

	long zP = alocarNo(t);
	escreverNo(t, p, &no);
	escreverNo(t, zP, &z);

	r.split = 1;
	memcpy(r.router, trt[meio], TAM_CEP);
	r.novaPagina = zP;
	return r;
}

void abmais_inserir(ArvoreBMais *t, const char *cep, long pos) {
	Resultado r = inserirRec(t, t->raiz, cep, pos);
	if (r.split) {
		long novaP = alocarNo(t);
		NoBMais nr;
		memset(&nr, 0, sizeof(nr));
		nr.folha = 0;
		nr.n = 1;
		memcpy(nr.chaves[0], r.router, TAM_CEP);
		nr.filhos[0] = t->raiz;
		nr.filhos[1] = r.novaPagina;
		escreverNo(t, novaP, &nr);
		t->raiz = novaP;
	}
}

int abmais_buscar(ArvoreBMais *t, const char *cep, long *posicao) {
	NoBMais no;
	long p = t->raiz;
	lerNo(t, p, &no);
	while (!no.folha) {
		int i = 0;
		while (i < no.n && cmpcep(cep, no.chaves[i]) > 0) i++;
		p = no.filhos[i];
		lerNo(t, p, &no);
	}
	int i = 0;
	while (i < no.n && cmpcep(cep, no.chaves[i]) > 0) i++;
	if (i < no.n && cmpcep(cep, no.chaves[i]) == 0) {
		*posicao = no.valores[i];
		return 1;
	}
	return 0;
}

void abmais_percorrer(ArvoreBMais *t,
                      void (*cb)(const char *cep, long posicao, void *ud),
                      void *ud) {
	NoBMais no;
	long p = t->raiz;
	lerNo(t, p, &no);
	while (!no.folha) {
		p = no.filhos[0];
		lerNo(t, p, &no);
	}
	while (1) {
		for (int i = 0; i < no.n; i++)
			cb(no.chaves[i], no.valores[i], ud);
		if (no.prox < 0) break;
		lerNo(t, no.prox, &no);
	}
}
