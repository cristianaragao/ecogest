#ifndef RELATORIO_H
#define RELATORIO_H

/* Modulo de relatorios do sistema EcoGest */

void menu_relatorios();

/* Relatorios individuais por cliente */
void relatorio_semestral_cliente(int id_cliente);
void relatorio_gasto_anual_cliente(int id_cliente);
void relatorio_ficha_cliente(int id_cliente);

/* Relatorios globais */
void relatorio_ranking_residuos(int semestre, int ano);
void relatorio_menor_producao(int semestre, int ano);
void relatorio_aporte_financeiro(int semestre, int ano);
void relatorio_por_estado();

/* Exportacao */
void exportar_txt(const char *nome_arquivo, const char *conteudo);
void exportar_csv(const char *nome_arquivo, int tipo_relatorio, int param);

#endif /* RELATORIO_H */
