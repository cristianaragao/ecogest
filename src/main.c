/*
 * main.c — Ponto de entrada do sistema EcoGest
 * PIM IV — UNIP ADS | Linguagem C
 *
 * Compilacao:
 *   Linux  : gcc -Wall -std=c11 src/main.c src/auth.c src/cliente.c \
 *             src/funcionario.c src/residuo.c src/relatorio.c src/utils.c -o ecogest
 *   Windows: compile_windows.bat
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
    #include <direct.h>
#else
    #include <sys/stat.h>
#endif

#include "auth.h"
#include "cliente.h"
#include "funcionario.h"
#include "residuo.h"
#include "relatorio.h"
#include "utils.h"

/* ------------------------------------------------------------------
 * Menu de administracao do sistema (nivel admin)
 * ------------------------------------------------------------------ */
static void menu_administracao(Usuario *logado) {
    if (logado->nivel != 1) {
        printf("\n  Acesso negado. Apenas administradores.\n");
        pausar();
        return;
    }

    int opcao;
    do {
        cabecalho("ADMINISTRACAO DO SISTEMA");
        printf("  [1] Gerenciar usuarios do sistema\n");
        printf("  [2] Visualizar log de acessos\n");
        printf("  [3] Backup dos dados\n");
        printf("  [4] Sobre o sistema\n");
        printf("  [0] Voltar\n\n");
        printf("  Opcao: ");

        char buf[10];
        fgets(buf, sizeof(buf), stdin);
        opcao = atoi(buf);

        switch (opcao) {
            case 1:
                menu_admin_usuarios(logado);
                break;

            case 2: {
                cabecalho("LOG DE ACESSOS");
                FILE *fp = fopen(ARQUIVO_LOG, "r");
                if (fp == NULL) {
                    printf("  Nenhum registro de acesso encontrado.\n");
                } else {
                    char linha[256];
                    while (fgets(linha, sizeof(linha), fp))
                        printf("  %s", linha);
                    fclose(fp);
                }
                pausar();
                break;
            }

            case 3: {
                cabecalho("BACKUP DOS DADOS");
                /* Cria pasta backup/ */
#ifdef _WIN32
                _mkdir("backup");
#else
                mkdir("backup", 0755);
#endif
                const char *arquivos[] = {
                    "data/usuarios.dat",
                    "data/clientes.dat",
                    "data/funcionarios.dat",
                    "data/residuos.dat",
                    "data/log_acesso.txt"
                };
                int n = (int)(sizeof(arquivos) / sizeof(arquivos[0]));
                int copiados = 0;

                for (int i = 0; i < n; i++) {
                    FILE *orig = fopen(arquivos[i], "rb");
                    if (orig == NULL) continue;

                    /* Monta nome do destino: backup/arquivo.dat */
                    char dest[200];
                    const char *barra = strrchr(arquivos[i], '/');
                    sprintf(dest, "backup/%s", barra ? barra + 1 : arquivos[i]);

                    FILE *bkp = fopen(dest, "wb");
                    if (bkp == NULL) { fclose(orig); continue; }

                    unsigned char tmpbuf[4096];
                    size_t lido;
                    while ((lido = fread(tmpbuf, 1, sizeof(tmpbuf), orig)) > 0)
                        fwrite(tmpbuf, 1, lido, bkp);

                    fclose(orig);
                    fclose(bkp);
                    printf("  Copiado: %s -> %s\n", arquivos[i], dest);
                    copiados++;
                }
                printf("\n  Backup concluido: %d arquivo(s) copiado(s).\n", copiados);
                pausar();
                break;
            }

            case 4:
                cabecalho("SOBRE O SISTEMA");
                printf("  ╔══════════════════════════════════════════╗\n");
                printf("  ║       EcoGest — Gestao Ambiental         ║\n");
                printf("  ║       Versao 1.0                         ║\n");
                printf("  ╠══════════════════════════════════════════╣\n");
                printf("  ║  Projeto Integrado Multidisciplinar IV   ║\n");
                printf("  ║  Curso: ADS — UNIP                       ║\n");
                printf("  ╠══════════════════════════════════════════╣\n");
                printf("  ║  Descricao:                              ║\n");
                printf("  ║  Sistema de cadastro e monitoramento     ║\n");
                printf("  ║  de industrias da regiao Norte do        ║\n");
                printf("  ║  Brasil, com controle de tratamento de   ║\n");
                printf("  ║  material particulado, gases toxicos e   ║\n");
                printf("  ║  efluentes liquidos.                     ║\n");
                printf("  ╚══════════════════════════════════════════╝\n");
                pausar();
                break;

            case 0:
                break;
            default:
                printf("  Opcao invalida.\n");
                pausar();
        }
    } while (opcao != 0);
}

/* ------------------------------------------------------------------
 * Menu principal apos login
 * ------------------------------------------------------------------ */
static void menu_principal(Usuario *logado) {
    int opcao;
    const char *nivel_str = (logado->nivel == 1) ? "ADMIN" : "OPERADOR";

    do {
        limpar_tela();
        printf("  ╔══════════════════════════════════════════╗\n");
        printf("  ║         ECOGEST — Gestao Ambiental       ║\n");
        printf("  ║         UNIP ADS — PIM IV  v1.0          ║\n");
        printf("  ╠══════════════════════════════════════════╣\n");
        printf("  ║  Usuario: %-15s Nivel: %-8s  ║\n", logado->nome, nivel_str);
        printf("  ╠══════════════════════════════════════════╣\n");
        printf("  ║  [1] Cadastro de Clientes                ║\n");
        printf("  ║  [2] Cadastro de Funcionarios            ║\n");
        printf("  ║  [3] Lancamento de Residuos Mensais      ║\n");
        printf("  ║  [4] Relatorios                          ║\n");
        printf("  ║  [5] Administracao do Sistema            ║\n");
        printf("  ║  [0] Sair                                ║\n");
        printf("  ╚══════════════════════════════════════════╝\n\n");
        printf("  Opcao: ");

        char buf[10];
        fgets(buf, sizeof(buf), stdin);
        opcao = atoi(buf);

        switch (opcao) {
            case 1: menu_clientes();              break;
            case 2: menu_funcionarios();          break;
            case 3: menu_residuos();              break;
            case 4: menu_relatorios();            break;
            case 5: menu_administracao(logado);   break;
            case 0:
                registrar_log("LOGOUT", logado->login);
                printf("\n  Ate logo, %s!\n\n", logado->nome);
                break;
            default:
                printf("  Opcao invalida.\n");
                pausar();
        }
    } while (opcao != 0);
}

/* ------------------------------------------------------------------
 * Ponto de entrada principal
 * ------------------------------------------------------------------ */
int main(void) {
    /* Garante que os diretorios necessarios existem */
    criar_diretorios();

    /* Cria usuario padrao admin/admin123 na primeira execucao */
    criar_usuario_padrao();

    /* Tela de login com limite de tentativas */
    Usuario usuario_logado;
    if (!fazer_login(&usuario_logado)) {
        return 1; /* encerra apos 3 tentativas falhas */
    }

    /* Menu principal do sistema */
    menu_principal(&usuario_logado);

    return 0;
}
