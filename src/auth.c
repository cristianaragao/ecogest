/*
 * auth.c — Autenticacao e gerenciamento de usuarios do EcoGest
 * PIM IV — UNIP ADS | Linguagem C
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "auth.h"
#include "utils.h"

/* ------------------------------------------------------------------
 * Criptografia XOR com chave fixa CHAVE_CRIPTO
 * ------------------------------------------------------------------ */
void criptografar(const char *entrada, char *saida) {
    int k = (int)strlen(CHAVE_CRIPTO);
    int n = (int)strlen(entrada);
    for (int i = 0; i < n; i++) {
        saida[i] = (char)(entrada[i] ^ CHAVE_CRIPTO[i % k]);
    }
    saida[n] = '\0';
}

void descriptografar(const char *entrada, char *saida) {
    /* XOR e simetrico: descriptografar == criptografar */
    criptografar(entrada, saida);
}

/* ------------------------------------------------------------------
 * Registra evento no log de acesso
 * ------------------------------------------------------------------ */
void registrar_log(const char *evento, const char *login) {
    FILE *fp = fopen(ARQUIVO_LOG, "a");
    if (fp == NULL) return;
    char dt[30];
    obter_data_hora(dt);
    fprintf(fp, "[%s] %s | usuario: %s\n", dt, evento, login);
    fclose(fp);
}

/* ------------------------------------------------------------------
 * Cria o usuario padrao admin/admin123 se o arquivo nao existir
 * ------------------------------------------------------------------ */
void criar_usuario_padrao() {
    FILE *fp = fopen(ARQUIVO_USUARIOS, "rb");
    if (fp != NULL) {
        fclose(fp);
        return; /* arquivo ja existe */
    }

    Usuario u;
    memset(&u, 0, sizeof(u));
    u.id    = 1;
    u.nivel = 1; /* admin */
    u.ativo = 1;
    strncpy(u.nome,  "Administrador", sizeof(u.nome) - 1);
    strncpy(u.login, "admin",         sizeof(u.login) - 1);

    char senha_plain[] = "admin123";
    criptografar(senha_plain, u.senha_criptografada);

    fp = fopen(ARQUIVO_USUARIOS, "wb");
    if (fp == NULL) {
        printf("  ERRO: nao foi possivel criar o arquivo de usuarios.\n");
        return;
    }
    fwrite(&u, sizeof(u), 1, fp);
    fclose(fp);
}

/* ------------------------------------------------------------------
 * Salva (acrescenta ou atualiza) um usuario no arquivo binario
 * ------------------------------------------------------------------ */
int salvar_usuario(Usuario u) {
    /* Carrega todos, substitui ou adiciona, salva tudo */
    FILE *fp = fopen(ARQUIVO_USUARIOS, "rb");
    Usuario lista[100];
    int total = 0;
    if (fp != NULL) {
        while (fread(&lista[total], sizeof(Usuario), 1, fp) == 1) total++;
        fclose(fp);
    }

    int encontrado = 0;
    for (int i = 0; i < total; i++) {
        if (lista[i].id == u.id) {
            lista[i]    = u;
            encontrado  = 1;
            break;
        }
    }
    if (!encontrado) {
        if (total >= 100) return 0;
        lista[total++] = u;
    }

    fp = fopen(ARQUIVO_USUARIOS, "wb");
    if (fp == NULL) return 0;
    fwrite(lista, sizeof(Usuario), total, fp);
    fclose(fp);
    return 1;
}

/* ------------------------------------------------------------------
 * Lista todos os usuarios cadastrados (apenas admin)
 * ------------------------------------------------------------------ */
int listar_usuarios() {
    FILE *fp = fopen(ARQUIVO_USUARIOS, "rb");
    if (fp == NULL) {
        printf("  Nenhum usuario cadastrado.\n");
        return 0;
    }

    Usuario u;
    int total = 0;
    printf("\n  %-4s %-30s %-20s %-10s %-6s\n",
           "ID", "Nome", "Login", "Nivel", "Ativo");
    linha_separadora(72);

    while (fread(&u, sizeof(u), 1, fp) == 1) {
        const char *nivel_str = (u.nivel == 1) ? "Admin" : "Operador";
        const char *ativo_str = u.ativo ? "Sim" : "Nao";
        printf("  %-4d %-30s %-20s %-10s %-6s\n",
               u.id, u.nome, u.login, nivel_str, ativo_str);
        total++;
    }
    fclose(fp);
    return total;
}

/* ------------------------------------------------------------------
 * Tela de login: retorna 1 se autenticado, 0 caso contrario
 * ------------------------------------------------------------------ */
int fazer_login(Usuario *usuario_logado) {
    char login[50], senha[50], senha_desc[100];
    int tentativas = 0;

    while (tentativas < MAX_TENTATIVAS) {
        cabecalho("LOGIN DO SISTEMA");

        if (tentativas > 0)
            printf("  Tentativa %d de %d\n\n", tentativas + 1, MAX_TENTATIVAS);

        printf("  Login : ");
        fgets(login, sizeof(login), stdin);
        trim_newline(login);

        printf("  Senha : ");
        ler_senha(senha, sizeof(senha));

        /* Busca usuario no arquivo */
        FILE *fp = fopen(ARQUIVO_USUARIOS, "rb");
        if (fp == NULL) {
            printf("\n  ERRO: arquivo de usuarios nao encontrado.\n");
            pausar();
            tentativas++;
            continue;
        }

        Usuario u;
        int ok = 0;
        while (fread(&u, sizeof(u), 1, fp) == 1) {
            if (!u.ativo) continue;
            if (strcmp(u.login, login) != 0) continue;

            descriptografar(u.senha_criptografada, senha_desc);
            if (strcmp(senha_desc, senha) == 0) {
                *usuario_logado = u;
                ok = 1;
                break;
            }
        }
        fclose(fp);

        if (ok) {
            registrar_log("LOGIN_OK", login);
            printf("\n  Bem-vindo, %s!\n", usuario_logado->nome);
            pausar();
            return 1;
        }

        registrar_log("LOGIN_FALHOU", login);
        printf("\n  Login ou senha incorretos.\n");
        pausar();
        tentativas++;
    }

    registrar_log("BLOQUEADO_3_TENTATIVAS", login);
    printf("\n  Numero maximo de tentativas atingido. Encerrando.\n");
    return 0;
}

/* ------------------------------------------------------------------
 * Submenu de administracao de usuarios (nivel admin)
 * ------------------------------------------------------------------ */
void menu_admin_usuarios(Usuario *logado) {
    if (logado->nivel != 1) {
        printf("\n  Acesso negado. Apenas administradores.\n");
        pausar();
        return;
    }

    int opcao;
    do {
        cabecalho("GERENCIAR USUARIOS");
        printf("  [1] Novo usuario\n");
        printf("  [2] Listar usuarios\n");
        printf("  [3] Alterar senha propria\n");
        printf("  [0] Voltar\n\n");
        printf("  Opcao: ");

        char buf[10];
        fgets(buf, sizeof(buf), stdin);
        opcao = atoi(buf);

        switch (opcao) {
            case 1: {
                cabecalho("NOVO USUARIO");
                Usuario novo;
                memset(&novo, 0, sizeof(novo));

                /* Calcula proximo ID */
                FILE *fp = fopen(ARQUIVO_USUARIOS, "rb");
                int max_id = 0;
                if (fp) {
                    Usuario tmp;
                    while (fread(&tmp, sizeof(tmp), 1, fp) == 1)
                        if (tmp.id > max_id) max_id = tmp.id;
                    fclose(fp);
                }
                novo.id    = max_id + 1;
                novo.ativo = 1;

                printf("  Nome completo : ");
                fgets(novo.nome, sizeof(novo.nome), stdin);
                trim_newline(novo.nome);

                printf("  Login         : ");
                fgets(novo.login, sizeof(novo.login), stdin);
                trim_newline(novo.login);

                char senha_plain[50];
                printf("  Senha         : ");
                ler_senha(senha_plain, sizeof(senha_plain));
                criptografar(senha_plain, novo.senha_criptografada);

                printf("  Nivel (1=Admin, 2=Operador): ");
                char nbuf[10];
                fgets(nbuf, sizeof(nbuf), stdin);
                novo.nivel = atoi(nbuf);
                if (novo.nivel != 1 && novo.nivel != 2) novo.nivel = 2;

                if (salvar_usuario(novo))
                    printf("\n  Usuario criado com sucesso! ID: %d\n", novo.id);
                else
                    printf("\n  ERRO ao salvar usuario.\n");
                pausar();
                break;
            }
            case 2:
                cabecalho("LISTA DE USUARIOS");
                listar_usuarios();
                pausar();
                break;
            case 3: {
                cabecalho("ALTERAR SENHA");
                char antiga[50], nova_senha[50], nova2[50];
                printf("  Senha atual : ");
                ler_senha(antiga, sizeof(antiga));

                char desc[100];
                descriptografar(logado->senha_criptografada, desc);
                if (strcmp(desc, antiga) != 0) {
                    printf("  Senha incorreta.\n");
                    pausar();
                    break;
                }

                printf("  Nova senha  : ");
                ler_senha(nova_senha, sizeof(nova_senha));
                printf("  Confirme    : ");
                ler_senha(nova2, sizeof(nova2));

                if (strcmp(nova_senha, nova2) != 0) {
                    printf("  Senhas nao conferem.\n");
                } else {
                    criptografar(nova_senha, logado->senha_criptografada);
                    salvar_usuario(*logado);
                    printf("  Senha alterada com sucesso!\n");
                }
                pausar();
                break;
            }
            case 0:
                break;
            default:
                printf("  Opcao invalida.\n");
                pausar();
        }
    } while (opcao != 0);
}
