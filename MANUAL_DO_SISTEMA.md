# EcoGest — Manual do Sistema

**Sistema de Gestão Ambiental**  
Versão 1.0 | PIM IV — UNIP ADS | Linguagem C

---

## Visão Geral

O EcoGest é um sistema desktop para a startup **EcoGest Soluções Ambientais**, voltado ao cadastro e monitoramento de indústrias de médio e grande porte da região Norte do Brasil (Amazonas, Pará, Rondônia). O foco é o controle de tratamento de material particulado, gases tóxicos e efluentes líquidos, com geração de relatórios para solicitação de benefícios fiscais junto ao governo.

---

## Módulos do Sistema

### 1. Cadastro de Clientes

Gerencia as indústrias monitoradas pela EcoGest.

| Função | Descrição |
|--------|-----------|
| Novo cliente | Preenche todos os dados da empresa (CNPJ, endereço, segmento, etc.) |
| Editar | Localiza pelo ID e altera os dados |
| Listar | Exibe tabela com todos os clientes ativos |
| Buscar por CNPJ | Localização exata pelo CNPJ |
| Buscar por nome | Busca parcial na razão social ou nome da empresa |
| Desativar | Exclusão lógica — o registro permanece no banco |

**Validações aplicadas:**
- CNPJ: formato `XX.XXX.XXX/XXXX-XX`
- CEP: 8 dígitos numéricos
- E-mail: deve conter `@` e `.`
- Data de abertura: formato `DD/MM/AAAA`

---

### 2. Cadastro de Funcionários

Gerencia os colaboradores internos da EcoGest.

| Função | Descrição |
|--------|-----------|
| Novo funcionário | Nome, CPF, cargo, e-mail, telefone e data de admissão |
| Editar | Localiza pelo ID e altera os dados |
| Listar | Tabela com todos os funcionários ativos |
| Buscar por CPF | Localização exata pelo CPF |
| Desativar | Exclusão lógica |

**Validação:** CPF no formato `XXX.XXX.XXX-XX`.

---

### 3. Lançamento de Resíduos Mensais

Registra o volume de resíduos tratados por cada cliente em cada mês.

| Função | Descrição |
|--------|-----------|
| Novo lançamento | Informa cliente, mês/ano e volumes de cada tipo de resíduo |
| Editar lançamento | Ajusta os valores de um lançamento existente |
| Histórico por cliente | Exibe todos os lançamentos de um cliente com custo total |
| Resumo do mês atual | Panorama geral de todos os clientes no mês corrente |

**Dados registrados por lançamento:**
- Material particulado tratado (kg)
- Gases tóxicos tratados (m³)
- Efluentes líquidos (litros)
- Custo estimado (R$)
- Observações

> O sistema impede lançamentos duplicados para o mesmo cliente no mesmo mês/ano.

---

### 4. Relatórios

#### Relatórios por Cliente

| Código | Descrição |
|--------|-----------|
| R1 | Histórico semestral — últimos 6 meses de resíduos tratados |
| R2 | Gasto mensal acumulado do ano corrente |
| R3 | Ficha completa com dados cadastrais e todos os lançamentos |

#### Relatórios Globais

| Código | Descrição |
|--------|-----------|
| R4 | Ranking de indústrias por volume total de resíduos (semestral) |
| R5 | Indústrias com menor produção/tratamento no semestre |
| R6 | Aporte financeiro total semestral (soma de todos os custos) |
| R7 | Distribuição por estado/região (agrupado por UF) |

#### Opções de Saída

Ao gerar qualquer relatório, o sistema pergunta:

1. **Exibir na tela** — formatado em tabela ASCII
2. **Salvar em TXT** — na pasta `relatorios/`
3. **Salvar em CSV** — compatível com Excel, na pasta `relatorios/`

O nome do arquivo inclui o tipo e a data/hora da geração, ex.:
```
relatorios/relatorio_R4_ranking_20250615_143022.csv
```

#### Como Interpretar o Ranking (R4)

A coluna **Volume Total** soma kg de particulado + m³ de gases + litros de efluentes.  
As indústrias com maior índice têm maior atividade de tratamento e são elegíveis a maiores benefícios fiscais junto ao governo estadual e federal.

---

### 5. Administração do Sistema *(somente Administrador)*

| Função | Descrição |
|--------|-----------|
| Gerenciar usuários | Criar novos operadores ou admins; alterar senha própria |
| Log de acessos | Histórico de logins — sucessos, falhas e bloqueios |
| Backup dos dados | Copia todos os arquivos `.dat` para a pasta `backup/` |
| Sobre | Informações da versão e do projeto |

---

## Níveis de Acesso

| Nível | Descrição | Restrições |
|-------|-----------|------------|
| Administrador (1) | Acesso completo | Nenhuma |
| Operador (2) | Acesso operacional | Sem acesso ao módulo de Administração |
