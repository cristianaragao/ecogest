#ifndef RESIDUO_H
#define RESIDUO_H

/* Modulo de residuos mensais do sistema EcoGest */

#define ARQUIVO_RESIDUOS "data/residuos.dat"
#define MAX_RESIDUOS     5000

typedef struct {
    int   id;
    int   id_cliente;
    int   mes;              /* 1-12 */
    int   ano;              /* ex: 2025 */
    float particulado_kg;   /* material particulado tratado em kg */
    float gases_m3;         /* gases tratados em m³ */
    float efluentes_litros; /* efluentes liquidos em litros */
    float custo_estimado;   /* custo em reais */
    char  observacoes[300];
} Residuo;

void menu_residuos();
void novo_lancamento();
void editar_lancamento();
void historico_por_cliente();
void resumo_mes_atual();
int  carregar_residuos(Residuo *lista, int *total);
int  salvar_residuos(Residuo *lista, int total);
int  existe_lancamento(int id_cliente, int mes, int ano);
int  proximo_id_residuo();

#endif /* RESIDUO_H */
