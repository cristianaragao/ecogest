#ifndef FUNCIONARIO_H
#define FUNCIONARIO_H

/* Modulo de funcionarios do sistema EcoGest */

#define ARQUIVO_FUNCIONARIOS "data/funcionarios.dat"
#define MAX_FUNCIONARIOS     500

typedef struct {
    int  id;
    char nome[100];
    char cpf[15];
    char cargo[80];
    char email[100];
    char telefone[20];
    char data_admissao[11]; /* DD/MM/AAAA */
    int  ativo;
} Funcionario;

void menu_funcionarios();
void novo_funcionario();
void editar_funcionario();
void listar_funcionarios();
void buscar_funcionario_cpf();
void desativar_funcionario();
int  carregar_funcionarios(Funcionario *lista, int *total);
int  salvar_funcionarios(Funcionario *lista, int total);
int  proximo_id_funcionario();

#endif /* FUNCIONARIO_H */
