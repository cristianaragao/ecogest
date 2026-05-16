/*
 * utils.c — Funcoes utilitarias do sistema EcoGest
 * PIM IV — UNIP ADS | Linguagem C
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#ifdef _WIN32
    #include <conio.h>
    #include <direct.h>
    #define MKDIR(d) _mkdir(d)
#else
    #include <termios.h>
    #include <unistd.h>
    #include <sys/stat.h>
    #define MKDIR(d) mkdir(d, 0755)
#endif

#include "utils.h"

/* Remove \n ao final de uma string lida com fgets */
void trim_newline(char *str) {
    int len = (int)strlen(str);
    if (len > 0 && str[len - 1] == '\n') str[len - 1] = '\0';
    if (len > 1 && str[len - 2] == '\r') str[len - 2] = '\0';
}

/* Limpa a tela conforme o SO */
void limpar_tela() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

/* Aguarda o usuario pressionar ENTER */
void pausar() {
    printf("\n  Pressione ENTER para continuar...");
    fflush(stdout);
    /* Descarta qualquer caractere pendente no buffer antes de esperar */
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/* Exibe cabecalho formatado com titulo centralizado */
void cabecalho(const char *titulo) {
    limpar_tela();
    printf("  ╔══════════════════════════════════════════╗\n");
    printf("  ║         ECOGEST — Gestao Ambiental       ║\n");
    printf("  ║         UNIP ADS — PIM IV  v1.0          ║\n");
    printf("  ╠══════════════════════════════════════════╣\n");
    printf("  ║ %-40s ║\n", titulo);
    printf("  ╚══════════════════════════════════════════╝\n\n");
}

/* Imprime linha de tracos com o tamanho especificado */
void linha_separadora(int tamanho) {
    printf("  ");
    for (int i = 0; i < tamanho; i++) printf("-");
    printf("\n");
}

/* Pergunta S/N ao usuario; retorna 1 para sim, 0 para nao */
int confirmar(const char *mensagem) {
    char resp[10];
    printf("  %s (S/N): ", mensagem);
    fgets(resp, sizeof(resp), stdin);
    trim_newline(resp);
    return (toupper((unsigned char)resp[0]) == 'S') ? 1 : 0;
}

/* Preenche buffer com data e hora atual no formato DD/MM/AAAA HH:MM:SS */
void obter_data_hora(char *buffer) {
    time_t agora = time(NULL);
    struct tm *t  = localtime(&agora);
    sprintf(buffer, "%02d/%02d/%04d %02d:%02d:%02d",
            t->tm_mday, t->tm_mon + 1, t->tm_year + 1900,
            t->tm_hour, t->tm_min, t->tm_sec);
}

/* Valida formato de CNPJ: XX.XXX.XXX/XXXX-XX */
int validar_cnpj_formato(const char *cnpj) {
    if (strlen(cnpj) != 18) return 0;
    /* posicoes dos separadores: 2='.', 6='.', 10='/', 15='-' */
    if (cnpj[2]  != '.') return 0;
    if (cnpj[6]  != '.') return 0;
    if (cnpj[10] != '/') return 0;
    if (cnpj[15] != '-') return 0;
    /* demais posicoes devem ser digitos */
    for (int i = 0; i < 18; i++) {
        if (i == 2 || i == 6 || i == 10 || i == 15) continue;
        if (!isdigit((unsigned char)cnpj[i])) return 0;
    }
    return 1;
}

/* Valida formato de CPF: XXX.XXX.XXX-XX */
int validar_cpf_formato(const char *cpf) {
    if (strlen(cpf) != 14) return 0;
    if (cpf[3]  != '.') return 0;
    if (cpf[7]  != '.') return 0;
    if (cpf[11] != '-') return 0;
    for (int i = 0; i < 14; i++) {
        if (i == 3 || i == 7 || i == 11) continue;
        if (!isdigit((unsigned char)cpf[i])) return 0;
    }
    return 1;
}

/* Valida e-mail: deve conter '@' e '.' apos o '@' */
int validar_email(const char *email) {
    const char *at = strchr(email, '@');
    if (at == NULL) return 0;
    const char *dot = strchr(at, '.');
    if (dot == NULL || dot == at + 1) return 0;
    return 1;
}

/* Valida data no formato DD/MM/AAAA */
int validar_data(const char *data) {
    if (strlen(data) != 10) return 0;
    if (data[2] != '/' || data[5] != '/') return 0;
    for (int i = 0; i < 10; i++) {
        if (i == 2 || i == 5) continue;
        if (!isdigit((unsigned char)data[i])) return 0;
    }
    int dia = atoi(data);
    int mes = atoi(data + 3);
    int ano = atoi(data + 6);
    if (mes < 1 || mes > 12) return 0;
    if (dia < 1 || dia > 31) return 0;
    if (ano < 1900 || ano > 2100) return 0;
    return 1;
}

/* Formata valor float como moeda brasileira: R$ 1.250,00 */
void formatar_moeda(float valor, char *saida) {
    long long inteiro  = (long long)valor;
    int centavos = (int)((valor - (float)inteiro) * 100.0f + 0.5f);

    /* Adiciona pontos de milhar */
    char buf[50];
    sprintf(buf, "%lld", inteiro);
    int len = (int)strlen(buf);
    char com_pontos[60];
    int j = 0;
    for (int i = 0; i < len; i++) {
        if (i > 0 && (len - i) % 3 == 0) com_pontos[j++] = '.';
        com_pontos[j++] = buf[i];
    }
    com_pontos[j] = '\0';

    sprintf(saida, "R$ %s,%02d", com_pontos, centavos);
}

/* Le senha ocultando os caracteres com '*' */
void ler_senha(char *buffer, int max) {
#ifdef _WIN32
    int i = 0;
    char c;
    while (i < max - 1 && (c = (char)getch()) != '\r') {
        if (c == '\b' && i > 0) {
            i--;
            printf("\b \b");
        } else if (c != '\b') {
            buffer[i++] = c;
            printf("*");
        }
    }
    buffer[i] = '\0';
    printf("\n");
#else
    struct termios old_t, new_t;
    tcgetattr(STDIN_FILENO, &old_t);
    new_t = old_t;
    new_t.c_lflag &= ~(ECHO | ICANON); /* desliga eco e modo canonico */
    tcsetattr(STDIN_FILENO, TCSANOW, &new_t);

    int i = 0;
    char c;
    while (i < max - 1 && read(STDIN_FILENO, &c, 1) == 1 && c != '\n') {
        if ((c == 127 || c == '\b') && i > 0) {
            i--;
            printf("\b \b");
        } else if (c != 127 && c != '\b') {
            buffer[i++] = c;
            printf("*");
        }
    }
    buffer[i] = '\0';
    printf("\n");

    tcsetattr(STDIN_FILENO, TCSANOW, &old_t);
#endif
}

/* Cria os diretorios data/ e relatorios/ se nao existirem */
void criar_diretorios() {
    MKDIR("data");
    MKDIR("relatorios");
}
