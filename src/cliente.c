/*
 * cliente.c — CRUD de clientes do sistema EcoGest
 * PIM IV — UNIP ADS | Linguagem C
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "cliente.h"
#include "utils.h"

/* ------------------------------------------------------------------
 * Carrega todos os clientes do arquivo binario
 * ------------------------------------------------------------------ */
int carregar_clientes(Cliente *lista, int *total) {
    *total = 0;
    FILE *fp = fopen(ARQUIVO_CLIENTES, "rb");
    if (fp == NULL) return 0;
    while (*total < MAX_CLIENTES &&
           fread(&lista[*total], sizeof(Cliente), 1, fp) == 1)
        (*total)++;
    fclose(fp);
    return 1;
}

/* ------------------------------------------------------------------
 * Salva toda a lista de clientes no arquivo binario
 * ------------------------------------------------------------------ */
int salvar_clientes(Cliente *lista, int total) {
    FILE *fp = fopen(ARQUIVO_CLIENTES, "wb");
    if (fp == NULL) { printf("  ERRO ao abrir arquivo de clientes.\n"); return 0; }
    fwrite(lista, sizeof(Cliente), total, fp);
    fclose(fp);
    return 1;
}

/* ------------------------------------------------------------------
 * Retorna o proximo ID disponivel
 * ------------------------------------------------------------------ */
int proximo_id_cliente() {
    Cliente lista[MAX_CLIENTES];
    int total = 0;
    carregar_clientes(lista, &total);
    int max = 0;
    for (int i = 0; i < total; i++)
        if (lista[i].id > max) max = lista[i].id;
    return max + 1;
}

/* ------------------------------------------------------------------
 * Busca cliente por ID e preenche a struct; retorna indice ou -1
 * ------------------------------------------------------------------ */
int buscar_cliente_por_id(int id, Cliente *c) {
    Cliente lista[MAX_CLIENTES];
    int total = 0;
    carregar_clientes(lista, &total);
    for (int i = 0; i < total; i++) {
        if (lista[i].id == id) {
            *c = lista[i];
            return i;
        }
    }
    return -1;
}

/* ------------------------------------------------------------------
 * Exibe linha de cabecalho da tabela de clientes
 * ------------------------------------------------------------------ */
static void cabecalho_tabela_clientes() {
    printf("  %-4s %-30s %-20s %-15s %-10s\n",
           "ID", "Empresa", "CNPJ", "Cidade", "UF");
    linha_separadora(82);
}

/* ------------------------------------------------------------------
 * Preenche os campos de um cliente via teclado
 * ------------------------------------------------------------------ */
static void preencher_cliente(Cliente *c) {
    char buf[200];

    printf("  Nome do responsavel : ");
    fgets(c->nome_responsavel, sizeof(c->nome_responsavel), stdin);
    trim_newline(c->nome_responsavel);

    printf("  Nome da empresa     : ");
    fgets(c->nome_empresa, sizeof(c->nome_empresa), stdin);
    trim_newline(c->nome_empresa);

    /* CNPJ com validacao */
    do {
        printf("  CNPJ (XX.XXX.XXX/XXXX-XX): ");
        fgets(c->cnpj, sizeof(c->cnpj), stdin);
        trim_newline(c->cnpj);
        if (!validar_cnpj_formato(c->cnpj))
            printf("  Formato invalido. Use XX.XXX.XXX/XXXX-XX\n");
    } while (!validar_cnpj_formato(c->cnpj));

    printf("  Razao social        : ");
    fgets(c->razao_social, sizeof(c->razao_social), stdin);
    trim_newline(c->razao_social);

    printf("  Nome fantasia       : ");
    fgets(c->nome_fantasia, sizeof(c->nome_fantasia), stdin);
    trim_newline(c->nome_fantasia);

    printf("  Telefone            : ");
    fgets(c->telefone, sizeof(c->telefone), stdin);
    trim_newline(c->telefone);

    printf("  Rua                 : ");
    fgets(c->rua, sizeof(c->rua), stdin);
    trim_newline(c->rua);

    printf("  Numero              : ");
    fgets(c->numero, sizeof(c->numero), stdin);
    trim_newline(c->numero);

    printf("  Bairro              : ");
    fgets(c->bairro, sizeof(c->bairro), stdin);
    trim_newline(c->bairro);

    printf("  Cidade              : ");
    fgets(c->cidade, sizeof(c->cidade), stdin);
    trim_newline(c->cidade);

    printf("  Estado (UF)         : ");
    fgets(c->estado, sizeof(c->estado), stdin);
    trim_newline(c->estado);

    /* CEP com validacao basica */
    do {
        printf("  CEP (somente digitos, 8): ");
        fgets(buf, sizeof(buf), stdin);
        trim_newline(buf);
        if (strlen(buf) == 8) {
            strncpy(c->cep, buf, sizeof(c->cep) - 1);
            break;
        }
        printf("  CEP deve ter 8 digitos.\n");
    } while (1);

    /* E-mail com validacao */
    do {
        printf("  E-mail              : ");
        fgets(c->email, sizeof(c->email), stdin);
        trim_newline(c->email);
        if (!validar_email(c->email))
            printf("  E-mail invalido.\n");
    } while (!validar_email(c->email));

    /* Data de abertura com validacao */
    do {
        printf("  Data de abertura (DD/MM/AAAA): ");
        fgets(c->data_abertura, sizeof(c->data_abertura), stdin);
        trim_newline(c->data_abertura);
        if (!validar_data(c->data_abertura))
            printf("  Data invalida. Use DD/MM/AAAA\n");
    } while (!validar_data(c->data_abertura));

    printf("  Segmento industrial : ");
    fgets(c->segmento, sizeof(c->segmento), stdin);
    trim_newline(c->segmento);
}

/* ------------------------------------------------------------------
 * Cadastra novo cliente
 * ------------------------------------------------------------------ */
void novo_cliente() {
    cabecalho("NOVO CLIENTE");

    Cliente lista[MAX_CLIENTES];
    int total = 0;
    carregar_clientes(lista, &total);

    Cliente c;
    memset(&c, 0, sizeof(c));
    c.id    = proximo_id_cliente();
    c.ativo = 1;

    preencher_cliente(&c);

    /* Verifica CNPJ duplicado */
    for (int i = 0; i < total; i++) {
        if (strcmp(lista[i].cnpj, c.cnpj) == 0 && lista[i].ativo) {
            printf("\n  CNPJ ja cadastrado (ID %d).\n", lista[i].id);
            pausar();
            return;
        }
    }

    lista[total++] = c;
    if (salvar_clientes(lista, total))
        printf("\n  Cliente cadastrado com sucesso! ID: %d\n", c.id);
    pausar();
}

/* ------------------------------------------------------------------
 * Edita um cliente existente
 * ------------------------------------------------------------------ */
void editar_cliente() {
    cabecalho("EDITAR CLIENTE");

    Cliente lista[MAX_CLIENTES];
    int total = 0;
    carregar_clientes(lista, &total);

    if (total == 0) { printf("  Nenhum cliente cadastrado.\n"); pausar(); return; }

    char buf[20];
    printf("  ID do cliente a editar: ");
    fgets(buf, sizeof(buf), stdin);
    int id = atoi(buf);

    int idx = -1;
    for (int i = 0; i < total; i++)
        if (lista[i].id == id) { idx = i; break; }

    if (idx < 0) { printf("  Cliente nao encontrado.\n"); pausar(); return; }

    printf("\n  Editando cliente: %s\n\n", lista[idx].nome_empresa);
    preencher_cliente(&lista[idx]);

    if (salvar_clientes(lista, total))
        printf("\n  Cliente atualizado com sucesso!\n");
    pausar();
}

/* ------------------------------------------------------------------
 * Lista todos os clientes ativos em tabela
 * ------------------------------------------------------------------ */
void listar_clientes() {
    cabecalho("LISTA DE CLIENTES");

    Cliente lista[MAX_CLIENTES];
    int total = 0;
    carregar_clientes(lista, &total);

    if (total == 0) { printf("  Nenhum cliente cadastrado.\n"); pausar(); return; }

    cabecalho_tabela_clientes();
    int ativos = 0;
    for (int i = 0; i < total; i++) {
        if (!lista[i].ativo) continue;
        printf("  %-4d %-30.30s %-20s %-15s %-10s\n",
               lista[i].id,
               lista[i].nome_empresa,
               lista[i].cnpj,
               lista[i].cidade,
               lista[i].estado);
        ativos++;
    }
    linha_separadora(82);
    printf("  Total: %d cliente(s) ativo(s)\n", ativos);
    pausar();
}

/* ------------------------------------------------------------------
 * Busca cliente por CNPJ
 * ------------------------------------------------------------------ */
void buscar_cliente_cnpj() {
    cabecalho("BUSCAR CLIENTE POR CNPJ");

    char cnpj[20];
    printf("  CNPJ: ");
    fgets(cnpj, sizeof(cnpj), stdin);
    trim_newline(cnpj);

    Cliente lista[MAX_CLIENTES];
    int total = 0;
    carregar_clientes(lista, &total);

    for (int i = 0; i < total; i++) {
        if (strcmp(lista[i].cnpj, cnpj) == 0) {
            Cliente *c = &lista[i];
            printf("\n  ID            : %d\n",  c->id);
            printf("  Empresa       : %s\n",    c->nome_empresa);
            printf("  Razao Social  : %s\n",    c->razao_social);
            printf("  CNPJ          : %s\n",    c->cnpj);
            printf("  Responsavel   : %s\n",    c->nome_responsavel);
            printf("  Cidade/UF     : %s/%s\n", c->cidade, c->estado);
            printf("  E-mail        : %s\n",    c->email);
            printf("  Segmento      : %s\n",    c->segmento);
            printf("  Ativo         : %s\n",    c->ativo ? "Sim" : "Nao");
            pausar();
            return;
        }
    }
    printf("  Cliente nao encontrado.\n");
    pausar();
}

/* ------------------------------------------------------------------
 * Busca cliente por nome ou razao social (busca parcial)
 * ------------------------------------------------------------------ */
void buscar_cliente_nome() {
    cabecalho("BUSCAR CLIENTE POR NOME");

    char termo[100];
    printf("  Nome/Razao Social (parte): ");
    fgets(termo, sizeof(termo), stdin);
    trim_newline(termo);

    /* Converte busca para minusculo */
    for (int i = 0; termo[i]; i++)
        termo[i] = (char)tolower((unsigned char)termo[i]);

    Cliente lista[MAX_CLIENTES];
    int total = 0, encontrados = 0;
    carregar_clientes(lista, &total);

    cabecalho_tabela_clientes();
    for (int i = 0; i < total; i++) {
        char nome_lower[150], razao_lower[150];
        strncpy(nome_lower,  lista[i].nome_empresa, sizeof(nome_lower) - 1);
        strncpy(razao_lower, lista[i].razao_social,  sizeof(razao_lower) - 1);
        for (int j = 0; nome_lower[j]; j++)
            nome_lower[j] = (char)tolower((unsigned char)nome_lower[j]);
        for (int j = 0; razao_lower[j]; j++)
            razao_lower[j] = (char)tolower((unsigned char)razao_lower[j]);

        if (strstr(nome_lower, termo) || strstr(razao_lower, termo)) {
            printf("  %-4d %-30.30s %-20s %-15s %-10s\n",
                   lista[i].id, lista[i].nome_empresa, lista[i].cnpj,
                   lista[i].cidade, lista[i].estado);
            encontrados++;
        }
    }
    linha_separadora(82);
    printf("  %d resultado(s) encontrado(s).\n", encontrados);
    pausar();
}

/* ------------------------------------------------------------------
 * Desativa (exclusao logica) um cliente
 * ------------------------------------------------------------------ */
void desativar_cliente() {
    cabecalho("DESATIVAR CLIENTE");

    char buf[20];
    printf("  ID do cliente a desativar: ");
    fgets(buf, sizeof(buf), stdin);
    int id = atoi(buf);

    Cliente lista[MAX_CLIENTES];
    int total = 0;
    carregar_clientes(lista, &total);

    for (int i = 0; i < total; i++) {
        if (lista[i].id == id) {
            if (!lista[i].ativo) {
                printf("  Cliente ja esta inativo.\n");
            } else if (confirmar("Desativar este cliente?")) {
                lista[i].ativo = 0;
                salvar_clientes(lista, total);
                printf("  Cliente desativado.\n");
            }
            pausar();
            return;
        }
    }
    printf("  Cliente nao encontrado.\n");
    pausar();
}

/* ------------------------------------------------------------------
 * Submenu principal do modulo de clientes
 * ------------------------------------------------------------------ */
void menu_clientes() {
    int opcao;
    do {
        cabecalho("CADASTRO DE CLIENTES");
        printf("  [1] Novo cliente\n");
        printf("  [2] Editar cliente\n");
        printf("  [3] Listar todos os clientes\n");
        printf("  [4] Buscar por CNPJ\n");
        printf("  [5] Buscar por nome/razao social\n");
        printf("  [6] Desativar cliente\n");
        printf("  [0] Voltar\n\n");
        printf("  Opcao: ");

        char buf[10];
        fgets(buf, sizeof(buf), stdin);
        opcao = atoi(buf);

        switch (opcao) {
            case 1: novo_cliente();        break;
            case 2: editar_cliente();      break;
            case 3: listar_clientes();     break;
            case 4: buscar_cliente_cnpj(); break;
            case 5: buscar_cliente_nome(); break;
            case 6: desativar_cliente();   break;
            case 0: break;
            default:
                printf("  Opcao invalida.\n");
                pausar();
        }
    } while (opcao != 0);
}
