# Gerenciador de Memória - Trabalho 2

## Descrição
Biblioteca de gerenciamento de memória em C que implementa alocação e liberação de memória em um pool pré-alocado, com três estratégias diferentes de alocação.

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

### mymemory_alloc(mymemory_t *memory, size_t size, allocation_strategy_t strategy)
Aloca um bloco de memória usando a estratégia especificada:
- **FIRST_FIT**: primeira lacuna suficientemente grande
- **BEST_FIT**: menor lacuna suficientemente grande
- **WORST_FIT**: maior lacuna disponível

### mymemory_free(mymemory_t *memory, void *ptr)
Libera uma alocação específica.

### mymemory_display(mymemory_t *memory)
Exibe todas as alocações atuais (endereço e tamanho).

### mymemory_stats(mymemory_t *memory)
Exibe estatísticas detalhadas:
- Número total de alocações
- Memória alocada/livre
- Maior bloco contíguo livre
- Número de fragmentos
- Taxa de utilização

### mymemory_cleanup(mymemory_t *memory)
Libera todos os recursos alocados.

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

## Estratégias de Alocação

### First Fit (Primeiro Ajuste)
Percorre a memória e aloca no primeiro espaço livre que seja grande o suficiente.
- **Vantagem**: Rápido
- **Desvantagem**: Pode causar fragmentação no início da memória

### Best Fit (Melhor Ajuste)
Procura o menor espaço livre que seja suficiente para a alocação.
- **Vantagem**: Minimiza desperdício de espaço
- **Desvantagem**: Pode criar muitos fragmentos pequenos

### Worst Fit (Pior Ajuste)
Aloca no maior espaço livre disponível.
- **Vantagem**: Deixa fragmentos maiores, potencialmente mais úteis
- **Desvantagem**: Fragmenta rapidamente grandes blocos

## Testes Implementados

O programa de teste (`main.c`) realiza:

1. **Testes por Estratégia**: Testa cada estratégia com:
   - Múltiplas alocações de tamanhos variados
   - Liberação alternada (criando fragmentação)
   - Novas alocações após fragmentação
   - Exibição de estatísticas em cada fase

2. **Teste de Estresse**: 100 operações aleatórias de alocação/liberação

## Restrições Atendidas

- `malloc()` e `free()` são usados APENAS para:
  - Alocação do pool de memória inicial
  - Estruturas de controle (mymemory_t e allocation_t)
- O alocador retorna endereços válidos dentro do pool
- Sem vazamentos de memória (verificável com Valgrind)

## Características da Implementação

- Lista encadeada mantida **ordenada por endereço** para facilitar busca de gaps
- Algoritmo eficiente para encontrar blocos livres
- Tratamento de casos especiais (memória cheia, ponteiros inválidos, etc.)
- Código limpo e bem comentado
- Sem warnings de compilação

## Como Funciona

1. O pool é alocado como um grande bloco contíguo
2. As alocações são rastreadas por uma lista encadeada
3. A lista é mantida ordenada por endereço de memória
4. Espaços livres são calculados como "gaps" entre alocações
5. Na liberação, o nó da lista é removido (mas a memória do pool permanece)

## Exemplo de Uso

```c
// Inicializa pool de 10KB
mymemory_t *mem = mymemory_init(10240);

// Aloca 100 bytes usando First Fit
void *ptr1 = mymemory_alloc(mem, 100, FIRST_FIT);

// Aloca 200 bytes usando Best Fit
void *ptr2 = mymemory_alloc(mem, 200, BEST_FIT);

// Libera primeira alocação
mymemory_free(mem, ptr1);

// Exibe estatísticas
mymemory_stats(mem);

// Limpa tudo
mymemory_cleanup(mem);
```

## Observações

- A implementação foi desenvolvida seguindo as especificações do trabalho
- Todo o código é original e comentado
- O programa compila sem erros ou warnings
- Testado e verificado com Valgrind para memory leaks