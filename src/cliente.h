#ifndef CLIENTE_H
#define CLIENTE_H

/* Modulo de clientes do sistema EcoGest */

#define ARQUIVO_CLIENTES "data/clientes.dat"
#define MAX_CLIENTES     1000

typedef struct {
    int  id;
    char nome_responsavel[100];
    char nome_empresa[150];
    char cnpj[20];           /* formato: XX.XXX.XXX/XXXX-XX */
    char razao_social[150];
    char nome_fantasia[100];
    char telefone[20];
    char rua[100];
    char numero[10];
    char bairro[80];
    char cidade[80];
    char estado[3];          /* sigla UF */
    char cep[10];
    char email[100];
    char data_abertura[11];  /* DD/MM/AAAA */
    char segmento[100];
    int  ativo;
} Cliente;

void menu_clientes();
void novo_cliente();
void editar_cliente();
void listar_clientes();
void buscar_cliente_cnpj();
void buscar_cliente_nome();
void desativar_cliente();
int  carregar_clientes(Cliente *lista, int *total);
int  salvar_clientes(Cliente *lista, int total);
int  buscar_cliente_por_id(int id, Cliente *c);
int  proximo_id_cliente();

#endif /* CLIENTE_H */
