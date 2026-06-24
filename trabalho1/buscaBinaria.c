#include <stdio.h>
#include <string.h>

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
	FILE *f;
	Endereco e;
	long inicio, fim, meio, total;
	int c, cmp, encontrado;

	if(argc != 2)
	{
		fprintf(stderr, "USO: %s [CEP]\n", argv[0]);
		return 1;
	}

	f = fopen("cep-ordenado.dat", "rb");
	if(!f)
	{
		fprintf(stderr, "Erro ao abrir o arquivo cep-ordenado.dat\n");
		return 1;
	}

	fseek(f, 0, SEEK_END);
	total = ftell(f) / sizeof(Endereco);

	inicio = 0;
	fim = total - 1;
	encontrado = 0;
	c = 0;

	while(inicio <= fim)
	{
		c++;
		meio = (inicio + fim) / 2;
		fseek(f, meio * sizeof(Endereco), SEEK_SET);
		fread(&e, sizeof(Endereco), 1, f);

		cmp = strncmp(argv[1], e.cep, 8);
		if(cmp == 0)
		{
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
	fclose(f);
	return 0;
}