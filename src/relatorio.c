/*
 * relatorio.c — Geracao de relatorios do sistema EcoGest
 * PIM IV — UNIP ADS | Linguagem C
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "relatorio.h"
#include "cliente.h"
#include "residuo.h"
#include "utils.h"

/* ------------------------------------------------------------------
 * Utilitarios internos
 * ------------------------------------------------------------------ */

static const char *nome_mes_r(int m) {
    static const char *meses[] = {
        "", "Jan","Fev","Mar","Abr","Mai","Jun",
        "Jul","Ago","Set","Out","Nov","Dez"
    };
    if (m < 1 || m > 12) return "?";
    return meses[m];
}

/* Retorna os meses do semestre: semestre 1 = Jan-Jun, 2 = Jul-Dez */
static void meses_semestre(int semestre, int *inicio, int *fim) {
    if (semestre == 1) { *inicio = 1; *fim = 6; }
    else               { *inicio = 7; *fim = 12; }
}

/* Gera nome de arquivo com timestamp */
static void nome_arquivo_relatorio(const char *tipo, const char *ext, char *saida) {
    time_t agora = time(NULL);
    struct tm *t  = localtime(&agora);
    sprintf(saida, "relatorios/relatorio_%s_%04d%02d%02d_%02d%02d%02d.%s",
            tipo,
            t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
            t->tm_hour, t->tm_min, t->tm_sec,
            ext);
}

/* ------------------------------------------------------------------
 * Exporta conteudo texto para arquivo TXT
 * ------------------------------------------------------------------ */
void exportar_txt(const char *nome_arquivo, const char *conteudo) {
    FILE *fp = fopen(nome_arquivo, "w");
    if (fp == NULL) {
        printf("  ERRO ao criar arquivo TXT.\n");
        return;
    }
    fprintf(fp, "%s", conteudo);
    fclose(fp);
    printf("  Arquivo salvo: %s\n", nome_arquivo);
}

/* ------------------------------------------------------------------
 * Pergunta ao usuario como deseja obter o relatorio
 * Retorna: 1=tela, 2=TXT, 3=CSV
 * ------------------------------------------------------------------ */
static int opcao_saida() {
    printf("\n  Saida do relatorio:\n");
    printf("  [1] Exibir na tela\n");
    printf("  [2] Salvar em TXT\n");
    printf("  [3] Salvar em CSV\n");
    printf("  Opcao: ");
    char buf[10];
    fgets(buf, sizeof(buf), stdin);
    return atoi(buf);
}

/* ------------------------------------------------------------------
 * R1 — Historico semestral do cliente (ultimos 6 meses)
 * ------------------------------------------------------------------ */
void relatorio_semestral_cliente(int id_cliente) {
    Cliente c;
    if (buscar_cliente_por_id(id_cliente, &c) < 0) {
        printf("  Cliente nao encontrado.\n");
        pausar();
        return;
    }

    Residuo lista[MAX_RESIDUOS];
    int total = 0;
    carregar_residuos(lista, &total);

    /* Determina os ultimos 6 meses a partir de hoje */
    time_t agora = time(NULL);
    struct tm *t  = localtime(&agora);
    int mes_atual = t->tm_mon + 1;
    int ano_atual = t->tm_year + 1900;

    /* Buffer para o relatorio */
    char rel[65536];
    int  pos = 0;

    pos += snprintf(rel + pos, sizeof(rel) - (size_t)pos,
        "===========================================\n"
        " RELATORIO SEMESTRAL — CLIENTE\n"
        " %s\n"
        " CNPJ: %s\n"
        "===========================================\n\n"
        " Periodo | Partic.(kg) | Gases(m3) | Efl.(L)   | Custo(R$)\n"
        "-----------------------------------------------------------\n",
        c.nome_empresa, c.cnpj);

    int encontrados = 0;
    float tot_custo = 0;

    /* Percorre os ultimos 6 meses */
    for (int k = 5; k >= 0; k--) {
        int mes = mes_atual - k;
        int ano = ano_atual;
        if (mes <= 0) { mes += 12; ano--; }

        for (int i = 0; i < total; i++) {
            if (lista[i].id_cliente == id_cliente &&
                lista[i].mes == mes && lista[i].ano == ano) {
                char moeda[30];
                formatar_moeda(lista[i].custo_estimado, moeda);
                pos += snprintf(rel + pos, sizeof(rel) - (size_t)pos,
                    " %s/%d | %11.2f | %9.2f | %9.2f | %s\n",
                    nome_mes_r(mes), ano,
                    lista[i].particulado_kg,
                    lista[i].gases_m3,
                    lista[i].efluentes_litros,
                    moeda);
                tot_custo += lista[i].custo_estimado;
                encontrados++;
            }
        }
    }

    char moeda_total[30];
    formatar_moeda(tot_custo, moeda_total);
    pos += snprintf(rel + pos, sizeof(rel) - (size_t)pos,
        "-----------------------------------------------------------\n"
        " Total: %d lancamento(s) | Custo total: %s\n",
        encontrados, moeda_total);

    int saida = opcao_saida();
    if (saida == 1) {
        printf("%s", rel);
        pausar();
    } else if (saida == 2) {
        char arq[200];
        nome_arquivo_relatorio("R1_semestral", "txt", arq);
        exportar_txt(arq, rel);
        pausar();
    } else if (saida == 3) {
        char arq[200];
        nome_arquivo_relatorio("R1_semestral", "csv", arq);
        exportar_csv(arq, 1, id_cliente);
    }
}

/* ------------------------------------------------------------------
 * R2 — Gasto mensal acumulado do ano por cliente
 * ------------------------------------------------------------------ */
void relatorio_gasto_anual_cliente(int id_cliente) {
    Cliente c;
    if (buscar_cliente_por_id(id_cliente, &c) < 0) {
        printf("  Cliente nao encontrado.\n");
        pausar();
        return;
    }

    time_t agora = time(NULL);
    struct tm *t  = localtime(&agora);
    int ano_atual = t->tm_year + 1900;

    Residuo lista[MAX_RESIDUOS];
    int total = 0;
    carregar_residuos(lista, &total);

    char rel[4096];
    int  pos = 0;
    pos += snprintf(rel + pos, sizeof(rel) - (size_t)pos,
        "===========================================\n"
        " GASTO MENSAL — ANO %d\n"
        " %s\n"
        "===========================================\n\n"
        " Mes       | Custo (R$)\n"
        "------------------------------\n",
        ano_atual, c.nome_empresa);

    float total_ano = 0;
    for (int m = 1; m <= 12; m++) {
        for (int i = 0; i < total; i++) {
            if (lista[i].id_cliente == id_cliente &&
                lista[i].mes == m && lista[i].ano == ano_atual) {
                char moeda[30];
                formatar_moeda(lista[i].custo_estimado, moeda);
                pos += snprintf(rel + pos, sizeof(rel) - (size_t)pos, " %-9s | %s\n", nome_mes_r(m), moeda);
                total_ano += lista[i].custo_estimado;
            }
        }
    }
    char moeda_total[30];
    formatar_moeda(total_ano, moeda_total);
    pos += snprintf(rel + pos, sizeof(rel) - (size_t)pos,
        "------------------------------\n"
        " TOTAL ANO | %s\n", moeda_total);

    int saida = opcao_saida();
    if (saida == 1) { printf("%s", rel); pausar(); }
    else if (saida == 2) {
        char arq[200]; nome_arquivo_relatorio("R2_gasto_anual", "txt", arq);
        exportar_txt(arq, rel); pausar();
    } else if (saida == 3) {
        char arq[200]; nome_arquivo_relatorio("R2_gasto_anual", "csv", arq);
        exportar_csv(arq, 2, id_cliente);
    }
}

/* ------------------------------------------------------------------
 * R3 — Ficha completa do cliente com todos os lancamentos
 * ------------------------------------------------------------------ */
void relatorio_ficha_cliente(int id_cliente) {
    Cliente c;
    if (buscar_cliente_por_id(id_cliente, &c) < 0) {
        printf("  Cliente nao encontrado.\n");
        pausar();
        return;
    }

    char rel[65536];
    int  pos = 0;
    pos += snprintf(rel + pos, sizeof(rel) - pos,
        "===========================================\n"
        " FICHA COMPLETA DO CLIENTE\n"
        "===========================================\n"
        " ID           : %d\n"
        " Empresa       : %s\n"
        " Razao Social  : %s\n"
        " CNPJ          : %s\n"
        " Responsavel   : %s\n"
        " Endereco      : %s, %s — %s/%s CEP %s\n"
        " Telefone      : %s\n"
        " E-mail        : %s\n"
        " Segmento      : %s\n"
        " Abertura      : %s\n"
        "-------------------------------------------\n"
        " LANCAMENTOS DE RESIDUOS\n"
        "-------------------------------------------\n"
        " Periodo     | Partic.(kg) | Gases(m3) | Efl.(L)   | Custo(R$)\n"
        "----------------------------------------------------------------\n",
        c.id, c.nome_empresa, c.razao_social, c.cnpj, c.nome_responsavel,
        c.rua, c.numero, c.cidade, c.estado, c.cep,
        c.telefone, c.email, c.segmento, c.data_abertura);

    Residuo lista[MAX_RESIDUOS];
    int total = 0;
    carregar_residuos(lista, &total);
    float tot = 0;
    int n = 0;

    for (int i = 0; i < total; i++) {
        if (lista[i].id_cliente != id_cliente) continue;
        char moeda[30];
        formatar_moeda(lista[i].custo_estimado, moeda);
        pos += snprintf(rel + pos, sizeof(rel) - (size_t)pos,
            " %s/%4d     | %11.2f | %9.2f | %9.2f | %s\n",
            nome_mes_r(lista[i].mes), lista[i].ano,
            lista[i].particulado_kg, lista[i].gases_m3,
            lista[i].efluentes_litros, moeda);
        tot += lista[i].custo_estimado;
        n++;
    }
    char moeda_total[30];
    formatar_moeda(tot, moeda_total);
    pos += snprintf(rel + pos, sizeof(rel) - (size_t)pos,
        "----------------------------------------------------------------\n"
        " Total: %d lancamento(s) | Custo acumulado: %s\n", n, moeda_total);

    int saida = opcao_saida();
    if (saida == 1) { printf("%s", rel); pausar(); }
    else if (saida == 2) {
        char arq[200]; nome_arquivo_relatorio("R3_ficha", "txt", arq);
        exportar_txt(arq, rel); pausar();
    } else if (saida == 3) {
        char arq[200]; nome_arquivo_relatorio("R3_ficha", "csv", arq);
        exportar_csv(arq, 3, id_cliente);
    }
}

/* ------------------------------------------------------------------
 * R4 — Ranking de industrias por volume total de residuos (semestral)
 * ------------------------------------------------------------------ */

typedef struct {
    int   id_cliente;
    char  nome[150];
    float volume_total; /* soma de particulado + gases + efluentes */
    float custo_total;
} RankItem;

void relatorio_ranking_residuos(int semestre, int ano) {
    int inicio, fim;
    meses_semestre(semestre, &inicio, &fim);

    Cliente clientes[MAX_CLIENTES];
    int tot_cli = 0;
    carregar_clientes(clientes, &tot_cli);

    Residuo residuos[MAX_RESIDUOS];
    int tot_res = 0;
    carregar_residuos(residuos, &tot_res);

    RankItem rank[MAX_CLIENTES];
    int n = 0;

    for (int i = 0; i < tot_cli; i++) {
        if (!clientes[i].ativo) continue;
        rank[n].id_cliente  = clientes[i].id;
        rank[n].volume_total = 0;
        rank[n].custo_total  = 0;
        strncpy(rank[n].nome, clientes[i].nome_empresa, sizeof(rank[n].nome) - 1);
        rank[n].nome[sizeof(rank[n].nome) - 1] = '\0';

        for (int j = 0; j < tot_res; j++) {
            if (residuos[j].id_cliente == clientes[i].id &&
                residuos[j].ano == ano &&
                residuos[j].mes >= inicio && residuos[j].mes <= fim) {
                rank[n].volume_total += residuos[j].particulado_kg
                                      + residuos[j].gases_m3
                                      + residuos[j].efluentes_litros;
                rank[n].custo_total  += residuos[j].custo_estimado;
            }
        }
        n++;
    }

    /* Ordenacao por volume (bubble sort simples) */
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (rank[j].volume_total < rank[j + 1].volume_total) {
                RankItem tmp = rank[j];
                rank[j]      = rank[j + 1];
                rank[j + 1]  = tmp;
            }
        }
    }

    char rel[131072];
    int  pos = 0;
    pos += snprintf(rel + pos, sizeof(rel) - (size_t)pos,
        "===========================================\n"
        " RANKING — VOLUME DE RESIDUOS TRATADOS\n"
        " Semestre %d/%d  (Meses %d a %d)\n"
        "===========================================\n\n"
        " # | %-30s | Volume Total  | Custo Total\n"
        "---+--------------------------------+---------------+------------------\n",
        semestre, ano, inicio, fim, "Empresa");

    for (int i = 0; i < n; i++) {
        char moeda[30];
        formatar_moeda(rank[i].custo_total, moeda);
        pos += snprintf(rel + pos, sizeof(rel) - (size_t)pos,
            " %-2d| %-30.30s | %13.2f | %s\n",
            i + 1, rank[i].nome, rank[i].volume_total, moeda);
    }
    pos += snprintf(rel + pos, sizeof(rel) - (size_t)pos, "\n  Unidade de volume: kg/m3/L somados\n");

    int saida = opcao_saida();
    if (saida == 1) { printf("%s", rel); pausar(); }
    else if (saida == 2) {
        char arq[200]; nome_arquivo_relatorio("R4_ranking", "txt", arq);
        exportar_txt(arq, rel); pausar();
    } else if (saida == 3) {
        char arq[200]; nome_arquivo_relatorio("R4_ranking", "csv", arq);
        exportar_csv(arq, 4, semestre * 10000 + ano);
    }
}

/* ------------------------------------------------------------------
 * R5 — Industrias com menor producao no semestre
 * ------------------------------------------------------------------ */
void relatorio_menor_producao(int semestre, int ano) {
    int inicio, fim;
    meses_semestre(semestre, &inicio, &fim);

    Cliente clientes[MAX_CLIENTES];
    int tot_cli = 0;
    carregar_clientes(clientes, &tot_cli);

    Residuo residuos[MAX_RESIDUOS];
    int tot_res = 0;
    carregar_residuos(residuos, &tot_res);

    RankItem rank[MAX_CLIENTES];
    int n = 0;

    for (int i = 0; i < tot_cli; i++) {
        if (!clientes[i].ativo) continue;
        rank[n].id_cliente   = clientes[i].id;
        rank[n].volume_total  = 0;
        rank[n].custo_total   = 0;
        strncpy(rank[n].nome, clientes[i].nome_empresa, sizeof(rank[n].nome) - 1);
        rank[n].nome[sizeof(rank[n].nome) - 1] = '\0';

        for (int j = 0; j < tot_res; j++) {
            if (residuos[j].id_cliente == clientes[i].id &&
                residuos[j].ano == ano &&
                residuos[j].mes >= inicio && residuos[j].mes <= fim) {
                rank[n].volume_total += residuos[j].particulado_kg
                                      + residuos[j].gases_m3
                                      + residuos[j].efluentes_litros;
            }
        }
        n++;
    }

    /* Ordena crescente (menor primeiro) */
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (rank[j].volume_total > rank[j + 1].volume_total) {
                RankItem tmp = rank[j];
                rank[j]      = rank[j + 1];
                rank[j + 1]  = tmp;
            }
        }
    }

    char rel[131072];
    int  pos = 0;
    pos += snprintf(rel + pos, sizeof(rel) - (size_t)pos,
        "===========================================\n"
        " MENOR PRODUCAO/TRATAMENTO — SEMESTRE %d/%d\n"
        "===========================================\n\n"
        " # | %-30s | Volume Total\n"
        "---+--------------------------------+---------------\n",
        semestre, ano, "Empresa");

    for (int i = 0; i < n; i++) {
        pos += snprintf(rel + pos, sizeof(rel) - (size_t)pos, " %-2d| %-30.30s | %13.2f\n",
                       i + 1, rank[i].nome, rank[i].volume_total);
    }

    int saida = opcao_saida();
    if (saida == 1) { printf("%s", rel); pausar(); }
    else if (saida == 2) {
        char arq[200]; nome_arquivo_relatorio("R5_menor_prod", "txt", arq);
        exportar_txt(arq, rel); pausar();
    } else if (saida == 3) {
        char arq[200]; nome_arquivo_relatorio("R5_menor_prod", "csv", arq);
        exportar_csv(arq, 5, semestre * 10000 + ano);
    }
}

/* ------------------------------------------------------------------
 * R6 — Aporte financeiro total semestral
 * ------------------------------------------------------------------ */
void relatorio_aporte_financeiro(int semestre, int ano) {
    int inicio, fim;
    meses_semestre(semestre, &inicio, &fim);

    Residuo lista[MAX_RESIDUOS];
    int total = 0;
    carregar_residuos(lista, &total);

    char rel[4096];
    int  pos = 0;
    float total_geral = 0;

    pos += snprintf(rel + pos, sizeof(rel) - (size_t)pos,
        "===========================================\n"
        " APORTE FINANCEIRO SEMESTRAL\n"
        " Semestre %d/%d  (Meses %d a %d)\n"
        "===========================================\n\n"
        " Mes      | Custo Total\n"
        "----------+------------------\n",
        semestre, ano, inicio, fim);

    for (int m = inicio; m <= fim; m++) {
        float custo_mes = 0;
        for (int i = 0; i < total; i++) {
            if (lista[i].ano == ano && lista[i].mes == m)
                custo_mes += lista[i].custo_estimado;
        }
        char moeda[30];
        formatar_moeda(custo_mes, moeda);
        pos += snprintf(rel + pos, sizeof(rel) - (size_t)pos, " %-9s| %s\n", nome_mes_r(m), moeda);
        total_geral += custo_mes;
    }
    char moeda_total[30];
    formatar_moeda(total_geral, moeda_total);
    pos += snprintf(rel + pos, sizeof(rel) - (size_t)pos,
        "----------+------------------\n"
        " TOTAL    | %s\n", moeda_total);

    int saida = opcao_saida();
    if (saida == 1) { printf("%s", rel); pausar(); }
    else if (saida == 2) {
        char arq[200]; nome_arquivo_relatorio("R6_aporte", "txt", arq);
        exportar_txt(arq, rel); pausar();
    } else if (saida == 3) {
        char arq[200]; nome_arquivo_relatorio("R6_aporte", "csv", arq);
        exportar_csv(arq, 6, semestre * 10000 + ano);
    }
}

/* ------------------------------------------------------------------
 * R7 — Distribuicao por estado (UF)
 * ------------------------------------------------------------------ */
void relatorio_por_estado() {
    Cliente clientes[MAX_CLIENTES];
    int tot_cli = 0;
    carregar_clientes(clientes, &tot_cli);

    Residuo residuos[MAX_RESIDUOS];
    int tot_res = 0;
    carregar_residuos(residuos, &tot_res);

    /* Coleta UFs distintas */
    char ufs[50][3];
    int  n_ufs = 0;
    for (int i = 0; i < tot_cli; i++) {
        if (!clientes[i].ativo) continue;
        int achou = 0;
        for (int j = 0; j < n_ufs; j++) {
            if (strcmp(ufs[j], clientes[i].estado) == 0) { achou = 1; break; }
        }
        if (!achou && n_ufs < 50) {
            strncpy(ufs[n_ufs++], clientes[i].estado, 2);
            ufs[n_ufs - 1][2] = '\0';
        }
    }

    char rel[4096];
    int  pos = 0;
    pos += snprintf(rel + pos, sizeof(rel) - (size_t)pos,
        "===========================================\n"
        " DISTRIBUICAO POR ESTADO/REGIAO\n"
        "===========================================\n\n"
        " UF | Clientes | Volume Total  | Custo Total\n"
        "----+----------+---------------+------------------\n");

    for (int u = 0; u < n_ufs; u++) {
        int   n_cli   = 0;
        float vol     = 0;
        float custo   = 0;

        for (int i = 0; i < tot_cli; i++) {
            if (!clientes[i].ativo) continue;
            if (strcmp(clientes[i].estado, ufs[u]) != 0) continue;
            n_cli++;
            for (int j = 0; j < tot_res; j++) {
                if (residuos[j].id_cliente == clientes[i].id) {
                    vol   += residuos[j].particulado_kg
                           + residuos[j].gases_m3
                           + residuos[j].efluentes_litros;
                    custo += residuos[j].custo_estimado;
                }
            }
        }
        char moeda[30];
        formatar_moeda(custo, moeda);
        pos += snprintf(rel + pos, sizeof(rel) - (size_t)pos, " %-2s | %-8d | %13.2f | %s\n",
                       ufs[u], n_cli, vol, moeda);
    }

    int saida = opcao_saida();
    if (saida == 1) { printf("%s", rel); pausar(); }
    else if (saida == 2) {
        char arq[200]; nome_arquivo_relatorio("R7_estados", "txt", arq);
        exportar_txt(arq, rel); pausar();
    } else if (saida == 3) {
        char arq[200]; nome_arquivo_relatorio("R7_estados", "csv", arq);
        exportar_csv(arq, 7, 0);
    }
}

/* ------------------------------------------------------------------
 * Exporta relatorio em CSV
 * tipo: 1=semestral_cli, 2=gasto_anual, 3=ficha, 4=ranking,
 *       5=menor_prod, 6=aporte, 7=por_estado
 * param: id_cliente ou semestre*10000+ano dependendo do tipo
 * ------------------------------------------------------------------ */
void exportar_csv(const char *nome_arquivo, int tipo_relatorio, int param) {
    FILE *fp = fopen(nome_arquivo, "w");
    if (fp == NULL) {
        printf("  ERRO ao criar arquivo CSV.\n");
        return;
    }

    Residuo residuos[MAX_RESIDUOS];
    int tot_res = 0;
    carregar_residuos(residuos, &tot_res);

    Cliente clientes[MAX_CLIENTES];
    int tot_cli = 0;
    carregar_clientes(clientes, &tot_cli);

    switch (tipo_relatorio) {
        case 1: /* semestral por cliente */
        case 2: /* gasto anual */
        case 3: /* ficha completa */ {
            int id_cli = param;
            fprintf(fp, "ID;Mes;Ano;Particulado_kg;Gases_m3;Efluentes_L;Custo_R$;Observacoes\n");
            for (int i = 0; i < tot_res; i++) {
                if (residuos[i].id_cliente != id_cli) continue;
                fprintf(fp, "%d;%d;%d;%.2f;%.2f;%.2f;%.2f;%s\n",
                        residuos[i].id, residuos[i].mes, residuos[i].ano,
                        residuos[i].particulado_kg, residuos[i].gases_m3,
                        residuos[i].efluentes_litros, residuos[i].custo_estimado,
                        residuos[i].observacoes);
            }
            break;
        }
        case 4: /* ranking */
        case 5: { /* menor producao */
            int semestre = param / 10000;
            int ano      = param % 10000;
            int ini, fim;
            meses_semestre(semestre, &ini, &fim);

            fprintf(fp, "ID_Cliente;Empresa;Volume_Total;Custo_Total\n");
            for (int i = 0; i < tot_cli; i++) {
                if (!clientes[i].ativo) continue;
                float vol = 0, custo = 0;
                for (int j = 0; j < tot_res; j++) {
                    if (residuos[j].id_cliente == clientes[i].id &&
                        residuos[j].ano == ano &&
                        residuos[j].mes >= ini && residuos[j].mes <= fim) {
                        vol   += residuos[j].particulado_kg
                               + residuos[j].gases_m3
                               + residuos[j].efluentes_litros;
                        custo += residuos[j].custo_estimado;
                    }
                }
                fprintf(fp, "%d;%s;%.2f;%.2f\n",
                        clientes[i].id, clientes[i].nome_empresa, vol, custo);
            }
            break;
        }
        case 6: { /* aporte financeiro */
            int semestre = param / 10000;
            int ano      = param % 10000;
            int ini, fim;
            meses_semestre(semestre, &ini, &fim);

            fprintf(fp, "Mes;Ano;Custo_Total\n");
            for (int m = ini; m <= fim; m++) {
                float custo = 0;
                for (int j = 0; j < tot_res; j++) {
                    if (residuos[j].ano == ano && residuos[j].mes == m)
                        custo += residuos[j].custo_estimado;
                }
                fprintf(fp, "%d;%d;%.2f\n", m, ano, custo);
            }
            break;
        }
        case 7: { /* por estado */
            fprintf(fp, "UF;Clientes;Volume_Total;Custo_Total\n");
            char ufs[50][3]; int n_ufs = 0;
            for (int i = 0; i < tot_cli; i++) {
                if (!clientes[i].ativo) continue;
                int achou = 0;
                for (int j = 0; j < n_ufs; j++)
                    if (strcmp(ufs[j], clientes[i].estado) == 0) { achou = 1; break; }
                if (!achou && n_ufs < 50) {
                    strncpy(ufs[n_ufs++], clientes[i].estado, 2);
                    ufs[n_ufs - 1][2] = '\0';
                }
            }
            for (int u = 0; u < n_ufs; u++) {
                int n = 0; float vol = 0, custo = 0;
                for (int i = 0; i < tot_cli; i++) {
                    if (!clientes[i].ativo || strcmp(clientes[i].estado, ufs[u]) != 0) continue;
                    n++;
                    for (int j = 0; j < tot_res; j++) {
                        if (residuos[j].id_cliente == clientes[i].id) {
                            vol   += residuos[j].particulado_kg + residuos[j].gases_m3 + residuos[j].efluentes_litros;
                            custo += residuos[j].custo_estimado;
                        }
                    }
                }
                fprintf(fp, "%s;%d;%.2f;%.2f\n", ufs[u], n, vol, custo);
            }
            break;
        }
        default:
            fprintf(fp, "Tipo de relatorio nao suportado para CSV.\n");
    }

    fclose(fp);
    printf("  Arquivo CSV salvo: %s\n", nome_arquivo);
    pausar();
}

/* ------------------------------------------------------------------
 * Submenu principal de relatorios
 * ------------------------------------------------------------------ */
void menu_relatorios() {
    int opcao;
    do {
        cabecalho("RELATORIOS");
        printf("  --- Relatorios por Cliente ---\n");
        printf("  [1] R1: Historico semestral do cliente\n");
        printf("  [2] R2: Gasto mensal acumulado do ano\n");
        printf("  [3] R3: Ficha completa do cliente\n");
        printf("  --- Relatorios Globais ---\n");
        printf("  [4] R4: Ranking por volume de residuos\n");
        printf("  [5] R5: Industrias com menor producao\n");
        printf("  [6] R6: Aporte financeiro semestral\n");
        printf("  [7] R7: Distribuicao por estado\n");
        printf("  [0] Voltar\n\n");
        printf("  Opcao: ");

        char buf[10];
        fgets(buf, sizeof(buf), stdin);
        opcao = atoi(buf);

        char buf2[20];
        int id_cli, semestre, ano;

        switch (opcao) {
            case 1:
            case 2:
            case 3:
                cabecalho("SELECIONAR CLIENTE");
                printf("  ID do cliente: ");
                fgets(buf2, sizeof(buf2), stdin);
                id_cli = atoi(buf2);
                if (opcao == 1) relatorio_semestral_cliente(id_cli);
                else if (opcao == 2) relatorio_gasto_anual_cliente(id_cli);
                else relatorio_ficha_cliente(id_cli);
                break;
            case 4:
            case 5:
            case 6:
                cabecalho("SELECIONAR SEMESTRE/ANO");
                printf("  Semestre (1 ou 2): ");
                fgets(buf2, sizeof(buf2), stdin);
                semestre = atoi(buf2);
                printf("  Ano: ");
                fgets(buf2, sizeof(buf2), stdin);
                ano = atoi(buf2);
                if (semestre != 1 && semestre != 2) { printf("  Semestre invalido.\n"); pausar(); break; }
                if (opcao == 4) relatorio_ranking_residuos(semestre, ano);
                else if (opcao == 5) relatorio_menor_producao(semestre, ano);
                else relatorio_aporte_financeiro(semestre, ano);
                break;
            case 7:
                relatorio_por_estado();
                break;
            case 0:
                break;
            default:
                printf("  Opcao invalida.\n");
                pausar();
        }
    } while (opcao != 0);
}
