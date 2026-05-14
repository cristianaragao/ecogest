#ifndef UTILS_H
#define UTILS_H

/* Utilitarios gerais do sistema EcoGest */

void limpar_tela();
void pausar();
void cabecalho(const char *titulo);
void linha_separadora(int tamanho);
int  confirmar(const char *mensagem);
void obter_data_hora(char *buffer);

int  validar_cnpj_formato(const char *cnpj);
int  validar_cpf_formato(const char *cpf);
int  validar_email(const char *email);
int  validar_data(const char *data);

void formatar_moeda(float valor, char *saida);
void ler_senha(char *buffer, int max);
void criar_diretorios();
void trim_newline(char *str);

#endif /* UTILS_H */
