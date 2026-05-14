# EcoGest — Compilação e Execução

**Sistema de Gestão Ambiental**  
Versão 1.0 | PIM IV — UNIP ADS | Linguagem C

---

## Requisitos

| Sistema | Requisito |
|---------|-----------|
| Linux   | GCC 4.8+ com suporte a C11 |
| Windows | MinGW (GCC para Windows) com suporte a C11 |

Sem dependências externas — apenas a biblioteca padrão C (stdlib).

### Instalar o GCC

**Linux (Debian/Ubuntu):**
```bash
sudo apt update && sudo apt install gcc make
```

**Linux (Fedora/RHEL):**
```bash
sudo dnf install gcc make
```

**Windows:**  
Baixe e instale o [MinGW-w64](https://www.mingw-w64.org/) e adicione a pasta `bin/` ao PATH do sistema.  
Verifique a instalação com: `gcc --version`

---

## Como Compilar

### Linux — via Makefile (recomendado)

Na raiz do projeto, execute:

```bash
make
```

### Linux — compilação manual

```bash
gcc -Wall -std=c11 src/main.c src/auth.c src/cliente.c src/funcionario.c \
    src/residuo.c src/relatorio.c src/utils.c -o ecogest
```

### Windows — script automático (recomendado)

Dê duplo clique no arquivo `compile_windows.bat`, ou execute no terminal:

```bat
compile_windows.bat
```

### Windows — compilação manual

```bat
gcc -Wall -std=c11 src/main.c src/auth.c src/cliente.c src/funcionario.c src/residuo.c src/relatorio.c src/utils.c -o ecogest.exe
```

---

## Como Executar

### Linux
```bash
./ecogest
```

### Windows
```bat
ecogest.exe
```

Na primeira execução o sistema cria automaticamente:
- A pasta `data/` com o arquivo de usuários e o usuário padrão
- A pasta `relatorios/` para exportação de relatórios

---

## Usuário Padrão

| Campo | Valor |
|-------|-------|
| Login | `admin` |
| Senha | `admin123` |
| Nível | Administrador |

> **Atenção:** Altere a senha após o primeiro acesso pelo menu **Administração → Gerenciar Usuários → Alterar senha própria**.

---

## Estrutura de Arquivos

```
ecogest/
├── src/                  Código-fonte C
├── data/                 Dados persistidos (criado automaticamente)
├── relatorios/           Relatórios exportados (criado automaticamente)
├── backup/               Cópias de segurança (criado ao fazer backup)
├── Makefile              Compilação Linux
├── compile_windows.bat   Compilação Windows
├── COMPILACAO_E_EXECUCAO.md
└── MANUAL_DO_SISTEMA.md
```

---

## Como Fazer Backup

1. Acesse o menu principal → **[5] Administração do Sistema**
2. Escolha **[3] Backup dos dados**
3. Os arquivos serão copiados para `backup/`:
   - `usuarios.dat`
   - `clientes.dat`
   - `funcionarios.dat`
   - `residuos.dat`
   - `log_acesso.txt`

Para restaurar, copie os arquivos de `backup/` de volta para `data/`.
