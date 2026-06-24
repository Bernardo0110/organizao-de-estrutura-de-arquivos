#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "CSVParser.h"
 
#define READ_BUF_SIZE 8192

#define PAIS_ALVO        "Brazil"
#define COL_LOCATION     "location"
#define COL_DATE         "date"
#define COL_NEW_CASES    "new_cases"
#define COL_NEW_DEATHS   "new_deaths"
#define ARQ_SAIDA        "covid.dat"
 
typedef struct {
	int dia;
	int novos_casos;
	int obitos;
} Registro;
 

typedef struct {
	FILE *saida;
	int cabecalhoLido;
	int idxLocation, idxDate, idxNewCases, idxNewDeaths;
	int dia;    
	long gravados; 
	long linhas;    
} Contexto;

static int acharColuna(char **cols, int ncols, const char *nome) {
	for (int i = 0; i < ncols; i++)
		if (strcmp(cols[i], nome) == 0)
			return i;
	return -1;
}
 
static int paraInteiro(const char *s) {
	if (s == NULL || s[0] == '\0') return 0;
	return atoi(s);
}
 
void callback(char **cols, int ncols, void *userData) {
	Contexto *ctx = (Contexto *) userData;
 
	if (!ctx->cabecalhoLido) {
		ctx->idxLocation = acharColuna(cols, ncols, COL_LOCATION);
		ctx->idxDate     = acharColuna(cols, ncols, COL_DATE);
		ctx->idxNewCases = acharColuna(cols, ncols, COL_NEW_CASES);
		ctx->idxNewDeaths= acharColuna(cols, ncols, COL_NEW_DEATHS);
		ctx->cabecalhoLido = 1;
 
		if (ctx->idxLocation < 0 || ctx->idxDate < 0 ||
		    ctx->idxNewCases < 0 || ctx->idxNewDeaths < 0) {
			fprintf(stderr, "ERRO: nao achei alguma coluna esperada.\n");
			fprintf(stderr, "Cabecalho encontrado (%d colunas):\n", ncols);
			for (int i = 0; i < ncols; i++)
				fprintf(stderr, "  [%d] %s\n", i, cols[i]);
			fprintf(stderr, "Ajuste os #define COL_* no topo do programa.\n");
			exit(1);
		}
		return;
	}
 
	ctx->linhas++;
 
	if (ctx->idxLocation >= ncols || ctx->idxDate >= ncols ||
	    ctx->idxNewCases >= ncols || ctx->idxNewDeaths >= ncols)
		return;
 
	if (strcmp(cols[ctx->idxLocation], PAIS_ALVO) != 0) return;
 
	Registro r;
	r.dia        = ctx->dia++;
	r.novos_casos= paraInteiro(cols[ctx->idxNewCases]);
	r.obitos     = paraInteiro(cols[ctx->idxNewDeaths]);
 
	fwrite(&r, sizeof(Registro), 1, ctx->saida);
	ctx->gravados++;
}
 
int main(int argc, char **argv) {
	const char *entrada = (argc >= 2) ? argv[1] : "owid-covid-data.csv";
 
	FILE *f = fopen(entrada, "rb");
	if (!f) {
		fprintf(stderr, "Erro ao abrir %s\n", entrada);
		return 1;
	}
 
	Contexto ctx;
	memset(&ctx, 0, sizeof(ctx));
	ctx.saida = fopen(ARQ_SAIDA, "wb");
	if (!ctx.saida) {
		fprintf(stderr, "Erro ao criar %s\n", ARQ_SAIDA);
		fclose(f);
		return 1;
	}
 
	char *buf = (char *) malloc(READ_BUF_SIZE);
	CSVParser csv;
	CSVParser_init(&csv);
 
	int qt;
	char ultimo = '\n';
	while ((qt = fread(buf, 1, READ_BUF_SIZE, f)) > 0) {
		CSVParser_processLines(&csv, buf, qt, callback, &ctx);
		ultimo = buf[qt - 1];
	}
	if (ultimo != '\n')
		CSVParser_processLines(&csv, "\n", 1, callback, &ctx);
 
	free(buf);
	fclose(f);
	fclose(ctx.saida);
 
	printf("Linhas de dados lidas: %ld\n", ctx.linhas);
	printf("Registros de \"%s\" gravados em %s: %ld\n",
	       PAIS_ALVO, ARQ_SAIDA, ctx.gravados);
	return 0;
}
