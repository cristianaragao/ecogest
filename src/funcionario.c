/*
 * funcionario.c — CRUD de funcionarios do sistema EcoGest
 * PIM IV — UNIP ADS | Linguagem C
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "funcionario.h"
#include "utils.h"

/* ------------------------------------------------------------------
 * Carrega todos os funcionarios do arquivo binario
 * ------------------------------------------------------------------ */
int carregar_funcionarios(Funcionario *lista, int *total) {
    *total = 0;
    FILE *fp = fopen(ARQUIVO_FUNCIONARIOS, "rb");
    if (fp == NULL) return 0;
    while (*total < MAX_FUNCIONARIOS &&
           fread(&lista[*total], sizeof(Funcionario), 1, fp) == 1)
        (*total)++;
    fclose(fp);
    return 1;
}

/* ------------------------------------------------------------------
 * Salva a lista completa de funcionarios
 * ------------------------------------------------------------------ */
int salvar_funcionarios(Funcionario *lista, int total) {
    FILE *fp = fopen(ARQUIVO_FUNCIONARIOS, "wb");
    if (fp == NULL) { printf("  ERRO ao abrir arquivo de funcionarios.\n"); return 0; }
    fwrite(lista, sizeof(Funcionario), total, fp);
    fclose(fp);
    return 1;
}

/* ------------------------------------------------------------------
 * Retorna o proximo ID disponivel
 * ------------------------------------------------------------------ */
int proximo_id_funcionario() {
    static Funcionario lista[MAX_FUNCIONARIOS];
    int total = 0;
    carregar_funcionarios(lista, &total);
    int max = 0;
    for (int i = 0; i < total; i++)
        if (lista[i].id > max) max = lista[i].id;
    return max + 1;
}

/* ------------------------------------------------------------------
 * Preenche campos de um funcionario via teclado
 * ------------------------------------------------------------------ */
static void preencher_funcionario(Funcionario *f) {
    printf("  Nome completo    : ");
    fgets(f->nome, sizeof(f->nome), stdin);
    trim_newline(f->nome);

    do {
        printf("  CPF (XXX.XXX.XXX-XX): ");
        char tmp_cpf[20];
        fgets(tmp_cpf, sizeof(tmp_cpf), stdin);
        trim_newline(tmp_cpf);
        strncpy(f->cpf, tmp_cpf, sizeof(f->cpf) - 1);
        f->cpf[sizeof(f->cpf) - 1] = '\0';
        if (!validar_cpf_formato(f->cpf))
            printf("  Formato invalido. Use XXX.XXX.XXX-XX\n");
    } while (!validar_cpf_formato(f->cpf));

    printf("  Cargo            : ");
    fgets(f->cargo, sizeof(f->cargo), stdin);
    trim_newline(f->cargo);

    do {
        printf("  E-mail           : ");
        fgets(f->email, sizeof(f->email), stdin);
        trim_newline(f->email);
        if (!validar_email(f->email))
            printf("  E-mail invalido.\n");
    } while (!validar_email(f->email));

    printf("  Telefone         : ");
    fgets(f->telefone, sizeof(f->telefone), stdin);
    trim_newline(f->telefone);

    do {
        printf("  Data de admissao (DD/MM/AAAA): ");
        char tmp_data[20];
        fgets(tmp_data, sizeof(tmp_data), stdin);
        trim_newline(tmp_data);
        strncpy(f->data_admissao, tmp_data, sizeof(f->data_admissao) - 1);
        f->data_admissao[sizeof(f->data_admissao) - 1] = '\0';
        if (!validar_data(f->data_admissao))
            printf("  Data invalida. Use DD/MM/AAAA\n");
    } while (!validar_data(f->data_admissao));
}

/* ------------------------------------------------------------------
 * Cadastra novo funcionario
 * ------------------------------------------------------------------ */
void novo_funcionario() {
    cabecalho("NOVO FUNCIONARIO");

    static Funcionario lista[MAX_FUNCIONARIOS];
    int total = 0;
    carregar_funcionarios(lista, &total);

    Funcionario f;
    memset(&f, 0, sizeof(f));
    f.id    = proximo_id_funcionario();
    f.ativo = 1;

    preencher_funcionario(&f);

    /* Verifica CPF duplicado */
    for (int i = 0; i < total; i++) {
        if (strcmp(lista[i].cpf, f.cpf) == 0 && lista[i].ativo) {
            printf("\n  CPF ja cadastrado (ID %d).\n", lista[i].id);
            pausar();
            return;
        }
    }

    if (total >= MAX_FUNCIONARIOS) {
        printf("\n  Limite de funcionarios atingido.\n");
        pausar();
        return;
    }
    lista[total++] = f;
    if (salvar_funcionarios(lista, total))
        printf("\n  Funcionario cadastrado com sucesso! ID: %d\n", f.id);
    pausar();
}

/* ------------------------------------------------------------------
 * Edita um funcionario existente
 * ------------------------------------------------------------------ */
void editar_funcionario() {
    cabecalho("EDITAR FUNCIONARIO");

    static Funcionario lista[MAX_FUNCIONARIOS];
    int total = 0;
    carregar_funcionarios(lista, &total);

    if (total == 0) { printf("  Nenhum funcionario cadastrado.\n"); pausar(); return; }

    char buf[20];
    printf("  ID do funcionario a editar: ");
    fgets(buf, sizeof(buf), stdin);
    int id = atoi(buf);

    int idx = -1;
    for (int i = 0; i < total; i++)
        if (lista[i].id == id) { idx = i; break; }

    if (idx < 0) { printf("  Funcionario nao encontrado.\n"); pausar(); return; }

    printf("\n  Editando: %s\n\n", lista[idx].nome);
    preencher_funcionario(&lista[idx]);

    if (salvar_funcionarios(lista, total))
        printf("\n  Funcionario atualizado com sucesso!\n");
    pausar();
}

/* ------------------------------------------------------------------
 * Lista todos os funcionarios ativos
 * ------------------------------------------------------------------ */
void listar_funcionarios() {
    cabecalho("LISTA DE FUNCIONARIOS");

    static Funcionario lista[MAX_FUNCIONARIOS];
    int total = 0;
    carregar_funcionarios(lista, &total);

    if (total == 0) { printf("  Nenhum funcionario cadastrado.\n"); pausar(); return; }

    printf("  %-4s %-30s %-15s %-25s %-12s\n",
           "ID", "Nome", "CPF", "Cargo", "Admissao");
    linha_separadora(90);

    int ativos = 0;
    for (int i = 0; i < total; i++) {
        if (!lista[i].ativo) continue;
        printf("  %-4d %-30.30s %-15s %-25.25s %-12s\n",
               lista[i].id, lista[i].nome, lista[i].cpf,
               lista[i].cargo, lista[i].data_admissao);
        ativos++;
    }
    linha_separadora(90);
    printf("  Total: %d funcionario(s) ativo(s)\n", ativos);
    pausar();
}

/* ------------------------------------------------------------------
 * Busca funcionario por CPF
 * ------------------------------------------------------------------ */
void buscar_funcionario_cpf() {
    cabecalho("BUSCAR FUNCIONARIO POR CPF");

    char cpf[15];
    printf("  CPF (XXX.XXX.XXX-XX): ");
    fgets(cpf, sizeof(cpf), stdin);
    trim_newline(cpf);

    static Funcionario lista[MAX_FUNCIONARIOS];
    int total = 0;
    carregar_funcionarios(lista, &total);

    for (int i = 0; i < total; i++) {
        if (strcmp(lista[i].cpf, cpf) == 0) {
            Funcionario *f = &lista[i];
            printf("\n  ID        : %d\n",  f->id);
            printf("  Nome      : %s\n",    f->nome);
            printf("  CPF       : %s\n",    f->cpf);
            printf("  Cargo     : %s\n",    f->cargo);
            printf("  E-mail    : %s\n",    f->email);
            printf("  Telefone  : %s\n",    f->telefone);
            printf("  Admissao  : %s\n",    f->data_admissao);
            printf("  Ativo     : %s\n",    f->ativo ? "Sim" : "Nao");
            pausar();
            return;
        }
    }
    printf("  Funcionario nao encontrado.\n");
    pausar();
}

/* ------------------------------------------------------------------
 * Busca funcionario por nome (busca parcial, case-insensitive)
 * ------------------------------------------------------------------ */
void buscar_funcionario_nome() {
    cabecalho("BUSCAR FUNCIONARIO POR NOME");

    char termo[100];
    printf("  Nome (parte): ");
    fgets(termo, sizeof(termo), stdin);
    trim_newline(termo);

    for (int i = 0; termo[i]; i++)
        termo[i] = (char)tolower((unsigned char)termo[i]);

    static Funcionario lista[MAX_FUNCIONARIOS];
    int total = 0, encontrados = 0;
    carregar_funcionarios(lista, &total);

    printf("  %-4s %-30s %-15s %-25s %-12s\n",
           "ID", "Nome", "CPF", "Cargo", "Admissao");
    linha_separadora(90);

    for (int i = 0; i < total; i++) {
        char nome_lower[100];
        strncpy(nome_lower, lista[i].nome, sizeof(nome_lower) - 1);
        nome_lower[sizeof(nome_lower) - 1] = '\0';
        for (int j = 0; nome_lower[j]; j++)
            nome_lower[j] = (char)tolower((unsigned char)nome_lower[j]);

        if (strstr(nome_lower, termo)) {
            printf("  %-4d %-30.30s %-15s %-25.25s %-12s\n",
                   lista[i].id, lista[i].nome, lista[i].cpf,
                   lista[i].cargo, lista[i].data_admissao);
            encontrados++;
        }
    }
    linha_separadora(90);
    printf("  %d resultado(s) encontrado(s).\n", encontrados);
    pausar();
}

/* ------------------------------------------------------------------
 * Desativa (exclusao logica) um funcionario
 * ------------------------------------------------------------------ */
void desativar_funcionario() {
    cabecalho("DESATIVAR FUNCIONARIO");

    char buf[20];
    printf("  ID do funcionario a desativar: ");
    fgets(buf, sizeof(buf), stdin);
    int id = atoi(buf);

    static Funcionario lista[MAX_FUNCIONARIOS];
    int total = 0;
    carregar_funcionarios(lista, &total);

    for (int i = 0; i < total; i++) {
        if (lista[i].id == id) {
            if (!lista[i].ativo) {
                printf("  Funcionario ja esta inativo.\n");
            } else if (confirmar("Desativar este funcionario?")) {
                lista[i].ativo = 0;
                salvar_funcionarios(lista, total);
                printf("  Funcionario desativado.\n");
            }
            pausar();
            return;
        }
    }
    printf("  Funcionario nao encontrado.\n");
    pausar();
}

/* ------------------------------------------------------------------
 * Submenu principal do modulo de funcionarios
 * ------------------------------------------------------------------ */
void menu_funcionarios() {
    int opcao;
    do {
        cabecalho("CADASTRO DE FUNCIONARIOS");
        printf("  [1] Novo funcionario\n");
        printf("  [2] Editar funcionario\n");
        printf("  [3] Listar funcionarios\n");
        printf("  [4] Buscar por CPF\n");
        printf("  [5] Buscar por nome\n");
        printf("  [6] Desativar funcionario\n");
        printf("  [0] Voltar\n\n");
        printf("  Opcao: ");

        char buf[10];
        fgets(buf, sizeof(buf), stdin);
        opcao = atoi(buf);

        switch (opcao) {
            case 1: novo_funcionario();        break;
            case 2: editar_funcionario();      break;
            case 3: listar_funcionarios();     break;
            case 4: buscar_funcionario_cpf();  break;
            case 5: buscar_funcionario_nome(); break;
            case 6: desativar_funcionario();   break;
            case 0: break;
            default:
                printf("  Opcao invalida.\n");
                pausar();
        }
    } while (opcao != 0);
}
