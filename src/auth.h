#ifndef AUTH_H
#define AUTH_H

/* Modulo de autenticacao do sistema EcoGest */

#define ARQUIVO_USUARIOS "data/usuarios.dat"
#define ARQUIVO_LOG      "data/log_acesso.txt"
#define MAX_TENTATIVAS   3
#define CHAVE_CRIPTO     "EcoGest2025"

typedef struct {
    int  id;
    char nome[100];
    char login[50];
    char senha_criptografada[100]; /* XOR com chave fixa */
    int  nivel;  /* 1=admin, 2=operador */
    int  ativo;
} Usuario;

void criptografar(const char *entrada, char *saida);
void descriptografar(const char *entrada, char *saida);
int  fazer_login(Usuario *usuario_logado);
void criar_usuario_padrao();
int  salvar_usuario(Usuario u);
int  listar_usuarios();
void menu_admin_usuarios(Usuario *logado);
void registrar_log(const char *evento, const char *login);

#endif /* AUTH_H */
