Repositorio criado para os trabalhos da aula de arquivos.

os programas de CEP utilizam a mesma estrutura de registro:

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
