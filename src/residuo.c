/*
 * residuo.c — Lancamento mensal de residuos do sistema EcoGest
 * PIM IV — UNIP ADS | Linguagem C
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "residuo.h"
#include "cliente.h"
#include "utils.h"

/* ------------------------------------------------------------------
 * Carrega todos os registros de residuos
 * ------------------------------------------------------------------ */
int carregar_residuos(Residuo *lista, int *total) {
    *total = 0;
    FILE *fp = fopen(ARQUIVO_RESIDUOS, "rb");
    if (fp == NULL) return 0;
    while (*total < MAX_RESIDUOS &&
           fread(&lista[*total], sizeof(Residuo), 1, fp) == 1)
        (*total)++;
    fclose(fp);
    return 1;
}

/* ------------------------------------------------------------------
 * Salva toda a lista de residuos
 * ------------------------------------------------------------------ */
int salvar_residuos(Residuo *lista, int total) {
    FILE *fp = fopen(ARQUIVO_RESIDUOS, "wb");
    if (fp == NULL) { printf("  ERRO ao abrir arquivo de residuos.\n"); return 0; }
    fwrite(lista, sizeof(Residuo), total, fp);
    fclose(fp);
    return 1;
}

/* ------------------------------------------------------------------
 * Verifica se ja existe lancamento para cliente/mes/ano
 * ------------------------------------------------------------------ */
int existe_lancamento(int id_cliente, int mes, int ano) {
    Residuo lista[MAX_RESIDUOS];
    int total = 0;
    carregar_residuos(lista, &total);
    for (int i = 0; i < total; i++)
        if (lista[i].id_cliente == id_cliente &&
            lista[i].mes == mes && lista[i].ano == ano)
            return 1;
    return 0;
}

/* ------------------------------------------------------------------
 * Proximo ID disponivel
 * ------------------------------------------------------------------ */
int proximo_id_residuo() {
    Residuo lista[MAX_RESIDUOS];
    int total = 0;
    carregar_residuos(lista, &total);
    int max = 0;
    for (int i = 0; i < total; i++)
        if (lista[i].id > max) max = lista[i].id;
    return max + 1;
}

static const char *nome_mes(int m) {
    static const char *meses[] = {
        "", "Janeiro","Fevereiro","Marco","Abril","Maio","Junho",
        "Julho","Agosto","Setembro","Outubro","Novembro","Dezembro"
    };
    if (m < 1 || m > 12) return "?";
    return meses[m];
}

/* ------------------------------------------------------------------
 * Novo lancamento mensal de residuos
 * ------------------------------------------------------------------ */
void novo_lancamento() {
    cabecalho("NOVO LANCAMENTO DE RESIDUOS");

    char buf[50];
    int id_cli;

    printf("  ID do cliente (ou 0 para buscar por CNPJ): ");
    fgets(buf, sizeof(buf), stdin);
    id_cli = atoi(buf);

    Cliente c;
    if (id_cli == 0) {
        char cnpj[20];
        printf("  CNPJ do cliente: ");
        fgets(cnpj, sizeof(cnpj), stdin);
        trim_newline(cnpj);

        Cliente lista_c[MAX_CLIENTES];
        int tot_c = 0;
        carregar_clientes(lista_c, &tot_c);
        int found = 0;
        for (int i = 0; i < tot_c; i++) {
            if (strcmp(lista_c[i].cnpj, cnpj) == 0) {
                c      = lista_c[i];
                id_cli = c.id;
                found  = 1;
                break;
            }
        }
        if (!found) { printf("  Cliente nao encontrado.\n"); pausar(); return; }
    } else {
        if (buscar_cliente_por_id(id_cli, &c) < 0) {
            printf("  Cliente nao encontrado.\n");
            pausar();
            return;
        }
    }

    printf("\n  Cliente: %s (CNPJ: %s)\n", c.nome_empresa, c.cnpj);

    int mes, ano;
    printf("  Mes  (1-12): ");
    fgets(buf, sizeof(buf), stdin);
    mes = atoi(buf);

    printf("  Ano        : ");
    fgets(buf, sizeof(buf), stdin);
    ano = atoi(buf);

    if (mes < 1 || mes > 12 || ano < 2000 || ano > 2100) {
        printf("  Mes ou ano invalido.\n");
        pausar();
        return;
    }

    if (existe_lancamento(id_cli, mes, ano)) {
        printf("  ERRO: Ja existe lancamento para este cliente em %s/%d.\n",
               nome_mes(mes), ano);
        pausar();
        return;
    }

    Residuo r;
    memset(&r, 0, sizeof(r));
    r.id         = proximo_id_residuo();
    r.id_cliente = id_cli;
    r.mes        = mes;
    r.ano        = ano;

    printf("  Material particulado tratado (kg): ");
    fgets(buf, sizeof(buf), stdin);
    r.particulado_kg = (float)atof(buf);

    printf("  Gases tratados (m3)              : ");
    fgets(buf, sizeof(buf), stdin);
    r.gases_m3 = (float)atof(buf);

    printf("  Efluentes liquidos (litros)      : ");
    fgets(buf, sizeof(buf), stdin);
    r.efluentes_litros = (float)atof(buf);

    printf("  Custo estimado (R$)              : ");
    fgets(buf, sizeof(buf), stdin);
    r.custo_estimado = (float)atof(buf);

    printf("  Observacoes                      : ");
    fgets(r.observacoes, sizeof(r.observacoes), stdin);
    trim_newline(r.observacoes);

    Residuo lista[MAX_RESIDUOS];
    int total = 0;
    carregar_residuos(lista, &total);

    if (total >= MAX_RESIDUOS) {
        printf("\n  Limite de lancamentos atingido.\n");
        pausar();
        return;
    }
    lista[total++] = r;

    if (salvar_residuos(lista, total))
        printf("\n  Lancamento registrado com sucesso! ID: %d\n", r.id);
    pausar();
}

/* ------------------------------------------------------------------
 * Edita um lancamento existente
 * ------------------------------------------------------------------ */
void editar_lancamento() {
    cabecalho("EDITAR LANCAMENTO");

    Residuo lista[MAX_RESIDUOS];
    int total = 0;
    carregar_residuos(lista, &total);

    if (total == 0) { printf("  Nenhum lancamento registrado.\n"); pausar(); return; }

    char buf[20];
    printf("  ID do lancamento a editar: ");
    fgets(buf, sizeof(buf), stdin);
    int id = atoi(buf);

    int idx = -1;
    for (int i = 0; i < total; i++)
        if (lista[i].id == id) { idx = i; break; }

    if (idx < 0) { printf("  Lancamento nao encontrado.\n"); pausar(); return; }

    Residuo *r = &lista[idx];
    printf("\n  Editando lancamento ID %d — %s/%d\n\n", r->id, nome_mes(r->mes), r->ano);

    printf("  Material particulado (kg) [%.2f]: ", r->particulado_kg);
    fgets(buf, sizeof(buf), stdin);
    if (strlen(buf) > 1) r->particulado_kg = (float)atof(buf);

    printf("  Gases (m3) [%.2f]: ", r->gases_m3);
    fgets(buf, sizeof(buf), stdin);
    if (strlen(buf) > 1) r->gases_m3 = (float)atof(buf);

    printf("  Efluentes (litros) [%.2f]: ", r->efluentes_litros);
    fgets(buf, sizeof(buf), stdin);
    if (strlen(buf) > 1) r->efluentes_litros = (float)atof(buf);

    printf("  Custo (R$) [%.2f]: ", r->custo_estimado);
    fgets(buf, sizeof(buf), stdin);
    if (strlen(buf) > 1) r->custo_estimado = (float)atof(buf);

    printf("  Observacoes [%s]: ", r->observacoes);
    char obs[300];
    fgets(obs, sizeof(obs), stdin);
    trim_newline(obs);
    if (strlen(obs) > 0) strncpy(r->observacoes, obs, sizeof(r->observacoes) - 1);

    if (salvar_residuos(lista, total))
        printf("\n  Lancamento atualizado com sucesso!\n");
    pausar();
}

/* ------------------------------------------------------------------
 * Exibe historico de lancamentos de um cliente
 * ------------------------------------------------------------------ */
void historico_por_cliente() {
    cabecalho("HISTORICO POR CLIENTE");

    char buf[20];
    printf("  ID do cliente: ");
    fgets(buf, sizeof(buf), stdin);
    int id_cli = atoi(buf);

    Cliente c;
    if (buscar_cliente_por_id(id_cli, &c) < 0) {
        printf("  Cliente nao encontrado.\n");
        pausar();
        return;
    }

    printf("\n  Cliente: %s\n\n", c.nome_empresa);
    printf("  %-4s %-12s %-15s %-12s %-18s %-12s\n",
           "ID", "Periodo", "Partic.(kg)", "Gases(m3)", "Efluentes(L)", "Custo(R$)");
    linha_separadora(80);

    Residuo lista[MAX_RESIDUOS];
    int total = 0, encontrados = 0;
    float total_custo = 0;
    carregar_residuos(lista, &total);

    for (int i = 0; i < total; i++) {
        if (lista[i].id_cliente != id_cli) continue;
        char periodo[20];
        sprintf(periodo, "%s/%d", nome_mes(lista[i].mes), lista[i].ano);
        printf("  %-4d %-12s %-15.2f %-12.2f %-18.2f %-12.2f\n",
               lista[i].id, periodo,
               lista[i].particulado_kg, lista[i].gases_m3,
               lista[i].efluentes_litros, lista[i].custo_estimado);
        total_custo += lista[i].custo_estimado;
        encontrados++;
    }
    linha_separadora(80);

    char moeda[30];
    formatar_moeda(total_custo, moeda);
    printf("  %d lancamento(s) | Custo total: %s\n", encontrados, moeda);
    pausar();
}

/* ------------------------------------------------------------------
 * Resumo de todos os lancamentos do mes e ano atual
 * ------------------------------------------------------------------ */
void resumo_mes_atual() {
    cabecalho("RESUMO DO MES ATUAL");

    time_t agora = time(NULL);
    struct tm *t  = localtime(&agora);
    int mes_atual = t->tm_mon + 1;
    int ano_atual = t->tm_year + 1900;

    printf("  Periodo: %s/%d\n\n", nome_mes(mes_atual), ano_atual);
    printf("  %-4s %-30s %-15s %-12s %-18s %-12s\n",
           "ID", "Empresa", "Partic.(kg)", "Gases(m3)", "Efluentes(L)", "Custo(R$)");
    linha_separadora(94);

    Residuo lista[MAX_RESIDUOS];
    int total = 0;
    carregar_residuos(lista, &total);

    float tot_part = 0, tot_gas = 0, tot_efl = 0, tot_custo = 0;
    int encontrados = 0;

    for (int i = 0; i < total; i++) {
        if (lista[i].mes != mes_atual || lista[i].ano != ano_atual) continue;

        Cliente c;
        memset(&c, 0, sizeof(c));
        buscar_cliente_por_id(lista[i].id_cliente, &c);

        printf("  %-4d %-30.30s %-15.2f %-12.2f %-18.2f %-12.2f\n",
               lista[i].id_cliente, c.nome_empresa,
               lista[i].particulado_kg, lista[i].gases_m3,
               lista[i].efluentes_litros, lista[i].custo_estimado);

        tot_part  += lista[i].particulado_kg;
        tot_gas   += lista[i].gases_m3;
        tot_efl   += lista[i].efluentes_litros;
        tot_custo += lista[i].custo_estimado;
        encontrados++;
    }
    linha_separadora(94);

    char moeda[30];
    formatar_moeda(tot_custo, moeda);
    printf("  Totais: Partic.=%.2f kg | Gases=%.2f m3 | Efl.=%.2f L | Custo=%s\n",
           tot_part, tot_gas, tot_efl, moeda);
    printf("  %d lancamento(s) no mes\n", encontrados);
    pausar();
}

/* ------------------------------------------------------------------
 * Submenu principal do modulo de residuos
 * ------------------------------------------------------------------ */
void menu_residuos() {
    int opcao;
    do {
        cabecalho("LANCAMENTO DE RESIDUOS MENSAIS");
        printf("  [1] Novo lancamento mensal\n");
        printf("  [2] Editar lancamento\n");
        printf("  [3] Historico por cliente\n");
        printf("  [4] Resumo do mes atual\n");
        printf("  [0] Voltar\n\n");
        printf("  Opcao: ");

        char buf[10];
        fgets(buf, sizeof(buf), stdin);
        opcao = atoi(buf);

        switch (opcao) {
            case 1: novo_lancamento();       break;
            case 2: editar_lancamento();     break;
            case 3: historico_por_cliente(); break;
            case 4: resumo_mes_atual();      break;
            case 0: break;
            default:
                printf("  Opcao invalida.\n");
                pausar();
        }
    } while (opcao != 0);
}
