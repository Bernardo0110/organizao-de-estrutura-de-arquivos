# organiza-o-de-estrutura-de-arquivos
repositorio criado para exercicios passados na aula de arquivos

Todos os programas utilizam a mesma estrutura de registro:

ctypedef struct _Endereco Endereco;
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