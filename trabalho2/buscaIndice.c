#include <stdio.h>
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

int main(int argc, char **argv)
{
	FILE *fidx, *fdat;
	Endereco e;
	Indice idx;
	long inicio, fim, meio, total;
	int c, cmp, encontrado;

	if(argc != 2)
	{
		fprintf(stderr, "USO: %s [CEP]\n", argv[0]);
		return 1;
	}

	fidx = fopen("indice.dat", "rb");
	if(!fidx)
	{
		fprintf(stderr, "Erro ao abrir indice.dat\n");
		return 1;
	}

	fdat = fopen("cep.dat", "rb");
	if(!fdat)
	{
		fprintf(stderr, "Erro ao abrir cep.dat\n");
		fclose(fidx);
		return 1;
	}

	fseek(fidx, 0, SEEK_END);
	total = ftell(fidx) / sizeof(Indice);

	inicio = 0;
	fim = total - 1;
	encontrado = 0;
	c = 0;

	while(inicio <= fim)
	{
		c++;
		meio = (inicio + fim) / 2;
		fseek(fidx, meio * sizeof(Indice), SEEK_SET);
		fread(&idx, sizeof(Indice), 1, fidx);

		cmp = strncmp(argv[1], idx.cep, 8);
		if(cmp == 0)
		{
			fseek(fdat, idx.posicao * sizeof(Endereco), SEEK_SET);
			fread(&e, sizeof(Endereco), 1, fdat);
			printf("%.72s\n%.72s\n%.72s\n%.72s\n%.2s\n%.8s\n",
				e.logradouro, e.bairro, e.cidade, e.uf, e.sigla, e.cep);
			encontrado = 1;
			break;
		}
		else if(cmp > 0)
		{
			inicio = meio + 1;
		}
		else
		{
			fim = meio - 1;
		}
	}

	if(!encontrado)
	{
		fprintf(stderr, "CEP %s nao encontrado.\n", argv[1]);
	}

	printf("Comparacoes realizadas: %d\n", c);
	fclose(fidx);
	fclose(fdat);
	return 0;
}