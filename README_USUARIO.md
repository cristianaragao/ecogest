# EcoGest — Manual do Usuário

**Sistema de Gestão Ambiental**  
Versão 1.0 | PIM IV — UNIP ADS | Linguagem C

---

## Como Compilar

### Linux (GCC)

```bash
# Na raiz do projeto:
make

# Ou manualmente:
gcc -Wall -std=c11 src/main.c src/auth.c src/cliente.c src/funcionario.c \
    src/residuo.c src/relatorio.c src/utils.c -o ecogest
```

### Windows (MinGW/GCC)

Execute o arquivo `compile_windows.bat` com um duplo clique, ou no terminal:

```bat
compile_windows.bat
```

Certifique-se de que o GCC (MinGW) está instalado e adicionado ao PATH.

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

Na primeira execução, o sistema criará automaticamente os diretórios `data/` e `relatorios/`, além do usuário padrão de administrador.

---

## Usuário Padrão

| Campo | Valor    |
|-------|----------|
| Login | `admin`  |
| Senha | `admin123` |
| Nível | Administrador |

> **Atenção:** Altere a senha após o primeiro acesso pelo menu **Administração → Gerenciar Usuários → Alterar senha própria**.

---

## Módulos do Sistema

### 1. Cadastro de Clientes
Gerencia as indústrias monitoradas pela EcoGest.

- **Novo cliente:** preenche todos os dados (CNPJ, endereço, segmento, etc.)
- **Editar:** localiza pelo ID e altera os dados
- **Listar:** exibe tabela com todos os clientes ativos
- **Buscar por CNPJ:** localização exata
- **Buscar por nome:** busca parcial na razão social ou nome da empresa
- **Desativar:** exclusão lógica (o registro permanece no banco)

Validações: CNPJ (formato `XX.XXX.XXX/XXXX-XX`), CEP (8 dígitos), e-mail (deve conter `@` e `.`), data (formato `DD/MM/AAAA`).

---

### 2. Cadastro de Funcionários
Gerencia os colaboradores da EcoGest.

- **Novo funcionário:** nome, CPF, cargo, e-mail, telefone, data de admissão
- **Editar / Listar / Buscar por CPF / Desativar**

Validação: CPF no formato `XXX.XXX.XXX-XX`.

---

### 3. Lançamento de Resíduos Mensais
Registra o volume de resíduos tratados por cada cliente em cada mês.

- **Novo lançamento:** busca o cliente pelo ID ou CNPJ, informa mês/ano e volumes
  - Material particulado (kg)
  - Gases tóxicos (m³)
  - Efluentes líquidos (litros)
  - Custo estimado (R$)
- **Editar lançamento:** ajusta os valores de um lançamento existente
- **Histórico por cliente:** exibe todos os lançamentos de um cliente com custo total
- **Resumo do mês atual:** panorama geral do mês corrente

> O sistema impede lançamentos duplicados (mesmo cliente + mesmo mês/ano).

---

### 4. Relatórios

#### Por Cliente
| Código | Descrição |
|--------|-----------|
| R1 | Histórico semestral (últimos 6 meses) |
| R2 | Gasto mensal acumulado do ano |
| R3 | Ficha completa com todos os lançamentos |

#### Globais
| Código | Descrição |
|--------|-----------|
| R4 | Ranking de indústrias por volume total (semestral) |
| R5 | Indústrias com menor produção/tratamento |
| R6 | Aporte financeiro total semestral |
| R7 | Distribuição por estado/região |

#### Opções de saída
Ao gerar qualquer relatório, o sistema pergunta:
1. **Exibir na tela** — formatado em tabela ASCII
2. **Salvar em TXT** — na pasta `relatorios/`
3. **Salvar em CSV** — compatível com Excel, na pasta `relatorios/`

O nome do arquivo inclui tipo e data/hora, ex.: `relatorios/relatorio_R4_ranking_20250615_143022.csv`

#### Como interpretar o Ranking (R4)
A coluna **Volume Total** soma kg de particulado + m³ de gases + litros de efluentes.  
As indústrias com maior índice têm maior atividade de tratamento e são elegíveis a maiores benefícios fiscais.

---

### 5. Administração do Sistema *(somente Admin)*

- **Gerenciar usuários:** criar novos operadores/admins, alterar senha própria
- **Log de acessos:** exibe histórico de logins (sucesso, falhas, bloqueios)
- **Backup:** copia todos os arquivos `.dat` para a pasta `backup/`
- **Sobre:** informações do sistema

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

Para restaurar, basta copiar os arquivos de volta para `data/`.

---

## Estrutura de Arquivos

```
ecogest/
├── src/           Código-fonte C
├── data/          Dados persistidos (criado automaticamente)
├── relatorios/    Relatórios exportados (criado automaticamente)
├── backup/        Cópias de segurança (criado ao fazer backup)
├── Makefile       Compilação Linux
└── compile_windows.bat  Compilação Windows
```

---

## Requisitos Técnicos

- GCC 4.8+ com suporte a C11 (`-std=c11`)
- Linux ou Windows com MinGW
- Sem dependências externas além da biblioteca padrão C (stdlib)
