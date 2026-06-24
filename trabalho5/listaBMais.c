#include <stdio.h>
#include "arvorebmais.h"

long total = 0;

void imprime(const char *cep, long pos, void *ud)
{
	printf("%.8s -> registro %ld\n", cep, pos);
	total++;
}

int main(int argc, char **argv)
{
	const char *arqIndice = (argc >= 2) ? argv[1] : "cep-bmais.idx";

	ArvoreBMais *t = abmais_abrir(arqIndice);
	if(!t)
	{
		fprintf(stderr, "Erro ao abrir indice %s\n", arqIndice);
		return 1;
	}

	abmais_percorrer(t, imprime, NULL);
	abmais_fechar(t);

	fprintf(stderr, "Total: %ld CEPs\n", total);
	return 0;
}
