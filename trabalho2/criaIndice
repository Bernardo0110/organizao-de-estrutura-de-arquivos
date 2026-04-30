#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _Endereco Endereco;
typedef struct _Indice Indice;

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

struct _Indice
{
	char cep[8];
	long posicao;
};

int compara(const void *a, const void *b)
{
	return strncmp(((Indice*)a)->cep, ((Indice*)b)->cep, 8);
}

int main()
{
	FILE *f, *saida;
	Endereco e;
	Indice *idx;
	long total, i;

	f = fopen("cep.dat", "rb");
	if(!f)
	{
		fprintf(stderr, "Erro ao abrir cep.dat\n");
		return 1;
	}

	fseek(f, 0, SEEK_END);
	total = ftell(f) / sizeof(Endereco);
	rewind(f);

	idx = (Indice*) malloc(total * sizeof(Indice));
	if(!idx)
	{
		fprintf(stderr, "Erro ao alocar memoria\n");
		fclose(f);
		return 1;
	}

	for(i = 0; i < total; i++)
	{
		fread(&e, sizeof(Endereco), 1, f);
		strncpy(idx[i].cep, e.cep, 8);
		idx[i].posicao = i;
	}
	fclose(f);

	qsort(idx, total, sizeof(Indice), compara);

	saida = fopen("indice.dat", "wb");
	if(!saida)
	{
		fprintf(stderr, "Erro ao criar indice.dat\n");
		free(idx);
		return 1;
	}

	fwrite(idx, sizeof(Indice), total, saida);
	fclose(saida);
	free(idx);

	printf("Indice criado com sucesso: %ld registros indexados.\n", total);
	return 0;
}