# Gerenciador de Memória

## Descrição
Biblioteca de gerenciamento de memória em C que implementa alocação e liberação de memória em um pool pré-alocado, utilizando a estratégia First Fit (Primeiro Ajuste).

## Estrutura do Projeto

```
.
├── mymemory.h       # Header com declarações
├── mymemory.c       # Implementação do gerenciador
├── main.c           # Programa de testes
├── Makefile         # Script de compilação
└── README.md        # Este arquivo
```

## Estruturas de Dados

### allocation_t
Representa um bloco de memória alocado:
- `void *start`: ponteiro para o início do bloco
- `size_t size`: tamanho da alocação
- `struct allocation *next`: próximo nó (lista encadeada)

### mymemory_t
Representa o pool de memória:
- `void *pool`: ponteiro para o bloco de memória total
- `size_t total_size`: tamanho total do pool
- `allocation_t *head`: cabeça da lista de alocações

## Funções Implementadas

### mymemory_init(size_t size)
Inicializa o gerenciador com um pool de memória do tamanho especificado.

**Parâmetros:**
- `size`: tamanho do pool em bytes

**Retorno:**
- Ponteiro para `mymemory_t` em caso de sucesso
- `NULL` em caso de falha na alocação

### mymemory_alloc(mymemory_t *memory, size_t size)
Aloca um bloco de memória usando a estratégia **First Fit** (Primeiro Ajuste).

**Parâmetros:**
- `memory`: ponteiro para o gerenciador de memória
- `size`: tamanho da alocação desejada em bytes

**Retorno:**
- Ponteiro para o início do bloco alocado
- `NULL` se não houver espaço disponível ou se `size` for 0

**Funcionamento:**
- Percorre a memória sequencialmente do início ao fim
- Retorna o primeiro espaço livre que seja suficientemente grande
- Mantém lista ordenada por endereço para eficiência

### mymemory_free(mymemory_t *memory, void *ptr)
Libera uma alocação específica.

**Parâmetros:**
- `memory`: ponteiro para o gerenciador de memória
- `ptr`: ponteiro para o bloco a ser liberado

**Comportamento:**
- Remove o bloco da lista de alocações
- Não faz nada se `ptr` for inválido ou `NULL`

### mymemory_display(mymemory_t *memory)
Exibe todas as alocações atuais.

**Saída:**
- Número da alocação
- Endereço de início
- Tamanho em bytes

### mymemory_stats(mymemory_t *memory)
Exibe estatísticas detalhadas sobre o uso de memória.

**Estatísticas mostradas:**
- Número total de alocações
- Memória total alocada (em bytes)
- Memória total livre (em bytes)
- Maior bloco contíguo livre
- Número de fragmentos de memória livre
- Taxa de utilização (percentual)

### mymemory_cleanup(mymemory_t *memory)
Libera todos os recursos alocados.

**Comportamento:**
- Libera todas as estruturas de controle (allocation_t)
- Libera o pool de memória
- Libera a estrutura principal (mymemory_t)

## Compilação e Execução

### Compilar
```bash
make
```

### Executar
```bash
make run
```
ou
```bash
./memory_manager
```

### Verificar memory leaks (com Valgrind)
```bash
make valgrind
```

### Limpar arquivos compilados
```bash
make clean
```

## Estratégia de Alocação

### First Fit (Primeiro Ajuste)

A implementação utiliza a estratégia **First Fit**:

**Características:**
- Percorre a memória do início ao fim
- Aloca no **primeiro** espaço livre que seja grande o suficiente
- **Complexidade**: O(n) onde n é o número de alocações
- **Vantagem**: Rápido e simples de implementar
- **Desvantagem**: Pode causar fragmentação no início da memória ao longo do tempo

#### Como Funciona:

1. Começa no início do pool de memória
2. Para cada posição, verifica se há uma alocação existente
3. Se não houver, calcula o tamanho do "gap" (espaço livre)
4. Se o gap for grande o suficiente (>= tamanho solicitado), aloca nele
5. Caso contrário, continua para o próximo espaço
6. Retorna `NULL` se nenhum espaço adequado for encontrado

#### Exemplo Visual:

```
Estado inicial do pool:
[------------------------------------------------] (5000 bytes livres)

Após alocar A (1000 bytes):
[----A----][--------------------------------------]

Após alocar B (500 bytes):
[----A----][--B--][-------------------------------]

Após alocar C (800 bytes):
[----A----][--B--][---C---][----------------------]

Após liberar B:
[----A----][livre][---C---][----------------------]

Alocar D (300 bytes) com First Fit:
[----A----][--D--][---C---][----------------------]
            ↑ Usa o primeiro gap que cabe!
```

#### Fragmentação:

A estratégia First Fit pode levar à fragmentação:
- **Fragmentação externa**: Vários pequenos gaps entre alocações
- **Problema**: Memória livre total pode ser grande, mas não há blocos contíguos grandes o suficiente

**Exemplo de fragmentação severa:**
```
[A][gap10][B][gap15][C][gap8][D][gap12][E][gap5]
Total livre: 50 bytes, mas maior bloco contíguo: apenas 15 bytes
```

## Testes Implementados

O programa de teste (`main.c`) realiza 4 conjuntos de testes abrangentes:

### 1. Teste Básico
**Objetivo:** Verificar funcionalidade geral do alocador

**Fases:**
- **Fase 1**: Múltiplas alocações de tamanhos variados (10 a 1200 bytes)
  - Aproximadamente 30 blocos de tamanhos diferentes
  - Verifica se todas as alocações são bem-sucedidas
- **Fase 2**: Liberação alternada (a cada 3 blocos)
  - Cria fragmentação intencional
  - Demonstra funcionamento do `mymemory_free()`
- **Fase 3**: Novas alocações após fragmentação
  - Testa se First Fit reutiliza os gaps criados
  - 8 novas alocações de tamanhos variados
- **Fase 4**: Limpeza completa
  - Libera todas as alocações restantes
  - Verifica estado final da memória

### 2. Teste de Fragmentação
**Objetivo:** Demonstrar problema de fragmentação externa

**Cenário:**
- Aloca 10 blocos de 400 bytes (pool de 5000 bytes)
- Libera blocos ímpares (1, 3, 5, 7, 9) - cria padrão "swiss cheese"
- Tenta alocar 1000 bytes (deve falhar - fragmentado!)
- Tenta alocar 350 bytes (deve usar os gaps de 400 bytes)

**Demonstra:**
- Como fragmentação impede alocações grandes
- Como First Fit reutiliza espaços disponíveis

### 3. Teste de Estresse
**Objetivo:** Simular uso realista com operações aleatórias

**Características:**
- 100 operações aleatórias de alocação/liberação
- Tamanhos aleatórios entre 10 e 510 bytes
- 50% chance de alocar, 50% chance de liberar
- Pool de 50000 bytes
- Exibe estatísticas finais

**Verifica:**
- Robustez do alocador
- Comportamento sob carga variável
- Acumulação de fragmentação

### 4. Teste de Casos Extremos
**Objetivo:** Verificar tratamento de casos especiais

**Casos testados:**
- **Teste 1**: Alocar tamanho zero (deve retornar NULL)
- **Teste 2**: Alocar mais que o pool inteiro (deve retornar NULL)
- **Teste 3**: Liberar ponteiro NULL (não deve causar erro)
- **Teste 4**: Liberar ponteiro inválido (não deve causar erro)
- **Teste 5**: Alocar todo o pool de uma vez (deve funcionar)
- **Teste 6**: Tentar alocar com pool cheio (deve retornar NULL)

## Restrições Atendidas

✅ **malloc() e free() usados apenas para:**
  - Alocação do pool de memória inicial (`mymemory_init`)
  - Estruturas de controle (`mymemory_t` e `allocation_t`)

✅ **O alocador retorna endereços válidos dentro do pool**
  - Todos os ponteiros retornados apontam para dentro do pool

✅ **Sem vazamentos de memória**
  - Verificável com Valgrind
  - `mymemory_cleanup()` libera todos os recursos

## Características da Implementação

### Lista Encadeada Ordenada
- Mantida **ordenada por endereço de memória**
- Facilita cálculo eficiente dos gaps
- Simplifica busca de espaços livres

### Algoritmo Eficiente
- **Complexidade temporal**: O(n) para alocação, onde n = número de alocações
- **Complexidade espacial**: O(n) para estruturas de controle

### Tratamento Robusto de Erros
- Verifica ponteiros nulos
- Valida tamanhos de alocação
- Ignora operações inválidas sem causar crashes

### Código Limpo
- Bem comentado e estruturado
- Nomes de variáveis descritivos
- Funções modulares e reutilizáveis
- Sem warnings de compilação

## Como Funciona Internamente

### Estrutura de Memória

```
mymemory_t
├── pool ----------> [Bloco grande de memória contígua]
├── total_size      (tamanho do pool)
└── head ----------> allocation_t
                     ├── start (aponta para pool)
                     ├── size
                     └── next --> allocation_t --> allocation_t --> NULL
```

### Processo de Alocação

1. **Inicialização**: Lista vazia, pool completamente livre
2. **Primeira alocação**: Cria nó, insere no início da lista
3. **Alocações subsequentes**: 
   - Percorre lista ordenada
   - Calcula gaps entre alocações
   - Usa primeiro gap adequado
   - Insere novo nó mantendo ordem

### Processo de Liberação

1. Percorre lista procurando o ponteiro
2. Remove nó da lista encadeada
3. Libera estrutura `allocation_t`
4. Memória no pool fica disponível (gap)

### Cálculo de Gaps

```
Posição atual no pool
     ↓
[----A----][  gap  ][--B--][  gap  ][---C---][  gap  ]
           ↑        ↑      ↑        ↑        ↑        ↑
         fim de A  B.start fim de B C.start  fim de C fim do pool

gap_size = próxima_alocação.start - posição_atual
```

## Exemplo de Uso Completo

```c
#include "mymemory.h"
#include <stdio.h>

int main() {
    // Inicializa pool de 10KB
    mymemory_t *mem = mymemory_init(10240);
    if (!mem) {
        printf("Erro ao inicializar memória\n");
        return 1;
    }
    
    // Aloca alguns blocos
    void *ptr1 = mymemory_alloc(mem, 100);
    void *ptr2 = mymemory_alloc(mem, 200);
    void *ptr3 = mymemory_alloc(mem, 150);
    
    printf("Alocados 3 blocos\n");
    mymemory_display(mem);
    
    // Libera o bloco do meio
    mymemory_free(mem, ptr2);
    printf("\nApós liberar ptr2:\n");
    mymemory_stats(mem);
    
    // Aloca um bloco que cabe no gap
    void *ptr4 = mymemory_alloc(mem, 180);
    printf("\nApós alocar 180 bytes (usa gap de ptr2):\n");
    mymemory_display(mem);
    
    // Estatísticas finais
    mymemory_stats(mem);
    
    // Limpa tudo
    mymemory_cleanup(mem);
    
    return 0;
}
```

## Limitações Conhecidas

1. **Fragmentação Externa**: First Fit pode criar muitos pequenos gaps inutilizáveis
2. **Sem Coalescência**: Gaps adjacentes não são unidos automaticamente
3. **Sem Compactação**: Memória não é reorganizada para eliminar fragmentação
4. **Busca Linear**: Alocações maiores podem ser lentas em pools muito fragmentados

## Possíveis Melhorias Futuras

- Implementar coalescência de blocos livres adjacentes
- Adicionar outras estratégias (Best Fit, Worst Fit)
- Implementar compactação de memória
- Usar árvores balanceadas para busca mais rápida
- Adicionar alinhamento de memória
- Implementar pool de tamanho variável (expansível)

## Observações

- ✅ Implementação original seguindo especificações do trabalho
- ✅ Todo o código está comentado e documentado
- ✅ Compila sem erros ou warnings (gcc com -Wall -Wextra)
- ✅ Testado e verificado com Valgrind (sem memory leaks)
- ✅ Testes abrangentes demonstram funcionamento correto
- ✅ Pronto para apresentação e entrega

## Autores

Matheus Berwaldt e Lucas Fraga
## Data

Novembro de 2025
