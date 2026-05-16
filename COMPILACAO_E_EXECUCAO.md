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
gcc -Wall -std=c11 -static src/main.c src/auth.c src/cliente.c src/funcionario.c src/residuo.c src/relatorio.c src/utils.c -o EcoGest.exe
```

> **Importante:** a flag `-static` embute todas as dependências dentro do `.exe`. Sem ela, o programa pode não funcionar em computadores que não tenham o MinGW instalado.

---

## Distribuição para Usuário Final

Após compilar, o usuário final recebe **apenas a pasta com o executável** — sem precisar instalar nada:

```
EcoGest/
└── EcoGest.exe   ← tudo embutido, basta dar duplo clique
```

As pastas `data/` e `relatorios/` são criadas automaticamente na primeira execução, na mesma pasta do `.exe`.

**Como entregar:**
1. Compile com o `compile_windows.bat` (já usa `-static`)
2. Copie o `EcoGest.exe` gerado para uma pasta limpa
3. Compacte em `.zip` e envie — o usuário descompacta e executa

### Gerar `.exe` a partir do Linux (cross-compile)

Se você desenvolve no Linux mas quer gerar o `.exe` para Windows:

```bash
# Instala o cross-compiler
sudo apt install mingw-w64

# Compila para Windows com linking estático
x86_64-w64-mingw32-gcc -Wall -std=c11 -static \
    src/main.c src/auth.c src/cliente.c src/funcionario.c \
    src/residuo.c src/relatorio.c src/utils.c \
    -o EcoGest.exe
```

---

## Como Executar

### Linux

**Opção 1 — compilar e executar em um comando:**
```bash
make run
```

**Opção 2 — executar o binário já compilado:**
```bash
./ecogest
```

> **Importante:** execute sempre a partir da raiz do projeto (onde está o `Makefile`).  
> O sistema cria as pastas `data/` e `relatorios/` no diretório de trabalho atual.  
> Executar de outro diretório fará o sistema criar os arquivos no lugar errado.

**Requisitos do terminal Linux:**
- Terminal com suporte a UTF-8 (padrão em distribuições modernas)
- Variável `TERM` configurada (ex: `xterm-256color`, `xterm`, `linux`)
- Verifique com: `echo $TERM` e `echo $LANG`

**Dependências de sistema (Ubuntu/Debian):**
```bash
sudo apt update && sudo apt install gcc make
```

**Passo a passo completo do zero:**
```bash
# 1. Clone ou acesse o diretório do projeto
cd /caminho/para/ecogest

# 2. Compile
make

# 3. Execute
./ecogest
```

**Permissão de execução (se necessário):**
```bash
chmod +x ecogest
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
