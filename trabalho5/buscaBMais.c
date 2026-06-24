#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
	if(argc < 4)
	{
		fprintf(stderr, "USO: %s [cep.dat] [indice.idx] [CEP]\n", argv[0]);
		return 1;
	}

	const char *arqDados  = argv[1];
	const char *arqIndice = argv[2];
	const char *cepStr    = argv[3];

	char chave[8];
	memset(chave, ' ', 8);
	memcpy(chave, cepStr, strnlen(cepStr, 8));

	ArvoreBMais *t = abmais_abrir(arqIndice);
	if(!t)
	{
		fprintf(stderr, "Erro ao abrir indice %s\n", arqIndice);
		return 1;
	}

	long pos;
	if(!abmais_buscar(t, chave, &pos))
	{
		printf("CEP %s nao encontrado.\n", cepStr);
		abmais_fechar(t);
		return 0;
	}

	FILE *fd = fopen(arqDados, "rb");
	if(!fd)
	{
		fprintf(stderr, "Erro ao abrir %s\n", arqDados);
		abmais_fechar(t);
		return 1;
	}

	Endereco e;
	fseek(fd, pos * sizeof(Endereco), SEEK_SET);
	if(fread(&e, sizeof(Endereco), 1, fd) != 1)
	{
		fprintf(stderr, "Erro lendo registro na posicao %ld\n", pos);
		fclose(fd);
		abmais_fechar(t);
		return 1;
	}

	printf("%.72s\n%.72s\n%.72s\n%.72s\n%.2s\n%.8s\n",
		e.logradouro, e.bairro, e.cidade, e.uf, e.sigla, e.cep);

	fclose(fd);
	abmais_fechar(t);
	return 0;
}
