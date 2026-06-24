#include <stdio.h>
#include <stdlib.h>
#include "arvorebmais.h"

typedef struct _Endereco Endereco;

struct _Endereco
{
	char logradouro[72];
	char bairro[72];
	char cidade[72];
	char uf[72];
	char sigla[2];
	char cep[8];
	char lixo[2];
};

int main(int argc, char **argv)
{
	const char *arqDados  = (argc >= 2) ? argv[1] : "cep.dat";
	const char *arqIndice = (argc >= 3) ? argv[2] : "cep-bmais.idx";

	FILE *fd = fopen(arqDados, "rb");
	if(!fd)
	{
		fprintf(stderr, "Erro ao abrir %s\n", arqDados);
		return 1;
	}

	ArvoreBMais *t = abmais_criar(arqIndice);
	if(!t)
	{
		fprintf(stderr, "Erro ao criar %s\n", arqIndice);
		fclose(fd);
		return 1;
	}

	Endereco e;
	long i = 0;
	while(fread(&e, sizeof(Endereco), 1, fd) == 1)
	{
		abmais_inserir(t, e.cep, i);
		i++;
	}

	abmais_fechar(t);
	fclose(fd);

	printf("Indice B+ criado em %s a partir de %ld registros de %s\n",
	       arqIndice, i, arqDados);
	return 0;
}
