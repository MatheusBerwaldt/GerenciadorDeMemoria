#include "mymemory.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Função para teste básico do gerenciador
void test_basic() {
    printf("\n");
    printf("========================================\n");
    printf("TESTE BÁSICO - First Fit\n");
    printf("========================================\n");
    
    // Inicializa o pool de memória (10KB)
    mymemory_t *memory = mymemory_init(10240);
    if (!memory) {
        printf("Erro ao inicializar memória\n");
        return;
    }
    
    // Array de tamanhos variados para teste
    size_t test_sizes[] = {10, 25, 50, 100, 250, 500, 1000, 
                           15, 35, 75, 125, 300, 600, 1200,
                           20, 40, 60, 150, 350, 700, 800,
                           45, 85, 175, 275, 400, 550, 900};
    int num_sizes = sizeof(test_sizes) / sizeof(test_sizes[0]);
    
    // Armazena ponteiros das alocações
    void *allocations[50];
    int alloc_count = 0;
    
    printf("\n--- Fase 1: Alocações iniciais ---\n");
    
    // Faz várias alocações
    for (int i = 0; i < num_sizes && alloc_count < 50; i++) {
        void *ptr = mymemory_alloc(memory, test_sizes[i]);
        if (ptr) {
            allocations[alloc_count++] = ptr;
            printf("✓ Alocado %zu bytes em %p\n", test_sizes[i], ptr);
        } else {
            printf("✗ Falha ao alocar %zu bytes (memória insuficiente)\n", test_sizes[i]);
        }
    }
    
    mymemory_stats(memory);
    
    printf("\n--- Fase 2: Liberação alternada ---\n");
    
    // Libera algumas alocações de forma alternada para criar fragmentação
    for (int i = 0; i < alloc_count; i += 3) {
        printf("Liberando alocação %d (%p)\n", i, allocations[i]);
        mymemory_free(memory, allocations[i]);
        allocations[i] = NULL;
    }
    
    mymemory_stats(memory);
    
    printf("\n--- Fase 3: Novas alocações após fragmentação ---\n");
    
    // Tenta fazer novas alocações (First Fit usará os primeiros gaps disponíveis)
    size_t new_sizes[] = {30, 80, 200, 450, 900, 100, 50, 25};
    for (int i = 0; i < 8; i++) {
        void *ptr = mymemory_alloc(memory, new_sizes[i]);
        if (ptr) {
            printf("✓ Alocado %zu bytes em %p\n", new_sizes[i], ptr);
            if (alloc_count < 50) {
                allocations[alloc_count++] = ptr;
            }
        } else {
            printf("✗ Falha ao alocar %zu bytes\n", new_sizes[i]);
        }
    }
    
    mymemory_display(memory);
    mymemory_stats(memory);
    
    printf("\n--- Fase 4: Limpeza ---\n");
    
    // Libera todas as alocações restantes
    for (int i = 0; i < alloc_count; i++) {
        if (allocations[i]) {
            mymemory_free(memory, allocations[i]);
        }
    }
    
    mymemory_stats(memory);
    
    // Limpa tudo
    mymemory_cleanup(memory);
    printf("Memória liberada com sucesso!\n");
}

// Teste de fragmentação
void test_fragmentation() {
    printf("\n");
    printf("========================================\n");
    printf("TESTE DE FRAGMENTAÇÃO\n");
    printf("========================================\n");
    
    mymemory_t *memory = mymemory_init(5000);
    if (!memory) {
        printf("Erro ao inicializar memória\n");
        return;
    }
    
    void *ptrs[10];
    
    printf("\n--- Alocando 10 blocos de 400 bytes ---\n");
    for (int i = 0; i < 10; i++) {
        ptrs[i] = mymemory_alloc(memory, 400);
        if (ptrs[i]) {
            printf("Bloco %d alocado em %p\n", i, ptrs[i]);
        }
    }
    
    mymemory_stats(memory);
    
    printf("\n--- Liberando blocos ímpares (criando fragmentação) ---\n");
    for (int i = 1; i < 10; i += 2) {
        printf("Liberando bloco %d\n", i);
        mymemory_free(memory, ptrs[i]);
        ptrs[i] = NULL;
    }
    
    mymemory_stats(memory);
    
    printf("\n--- Tentando alocar blocos grandes ---\n");
    void *big = mymemory_alloc(memory, 1000);
    if (big) {
        printf("✓ Conseguiu alocar 1000 bytes\n");
    } else {
        printf("✗ Não conseguiu alocar 1000 bytes (fragmentado!)\n");
    }
    
    printf("\n--- Tentando alocar blocos que cabem nos gaps ---\n");
    for (int i = 0; i < 5; i++) {
        void *small = mymemory_alloc(memory, 350);
        if (small) {
            printf("✓ Alocado 350 bytes em %p\n", small);
        } else {
            printf("✗ Falha ao alocar 350 bytes\n");
        }
    }
    
    mymemory_display(memory);
    mymemory_stats(memory);
    
    mymemory_cleanup(memory);
}

// Teste de estresse
void stress_test() {
    printf("\n");
    printf("========================================\n");
    printf("TESTE DE ESTRESSE\n");
    printf("========================================\n");
    
    mymemory_t *memory = mymemory_init(50000);
    if (!memory) {
        printf("Erro ao inicializar memória\n");
        return;
    }
    
    void *allocations[100];
    int alloc_count = 0;
    
    srand(time(NULL));
    
    printf("\nRealizando 100 operações aleatórias...\n");
    
    // Faz 100 operações aleatórias de alocação/liberação
    for (int i = 0; i < 100; i++) {
        int operation = rand() % 2;
        
        if (operation == 0 || alloc_count == 0) {
            // Aloca
            size_t size = (rand() % 500) + 10;
            void *ptr = mymemory_alloc(memory, size);
            if (ptr && alloc_count < 100) {
                allocations[alloc_count++] = ptr;
                if (i % 20 == 0) {
                    printf("[%d] Alocado %zu bytes (total: %d alocações)\n", i, size, alloc_count);
                }
            }
        } else {
            // Libera
            int idx = rand() % alloc_count;
            if (allocations[idx]) {
                if (i % 20 == 0) {
                    printf("[%d] Liberado índice %d (total: %d alocações)\n", i, idx, alloc_count - 1);
                }
                mymemory_free(memory, allocations[idx]);
                allocations[idx] = allocations[--alloc_count];
            }
        }
    }
    
    printf("\n--- Resultado do Teste de Estresse ---\n");
    mymemory_stats(memory);
    
    // Limpa tudo
    for (int i = 0; i < alloc_count; i++) {
        if (allocations[i]) {
            mymemory_free(memory, allocations[i]);
        }
    }
    
    mymemory_cleanup(memory);
}

// Teste de casos extremos
void test_edge_cases() {
    printf("\n");
    printf("========================================\n");
    printf("TESTE DE CASOS EXTREMOS\n");
    printf("========================================\n");
    
    mymemory_t *memory = mymemory_init(1000);
    if (!memory) {
        printf("Erro ao inicializar memória\n");
        return;
    }
    
    printf("\n--- Teste 1: Alocar tamanho zero ---\n");
    void *ptr1 = mymemory_alloc(memory, 0);
    if (!ptr1) {
        printf("✓ Corretamente retornou NULL para tamanho 0\n");
    }
    
    printf("\n--- Teste 2: Alocar mais que o pool todo ---\n");
    void *ptr2 = mymemory_alloc(memory, 2000);
    if (!ptr2) {
        printf("✓ Corretamente retornou NULL para alocação muito grande\n");
    }
    
    printf("\n--- Teste 3: Liberar ponteiro NULL ---\n");
    mymemory_free(memory, NULL);
    printf("✓ Função não causou erro com ponteiro NULL\n");
    
    printf("\n--- Teste 4: Liberar ponteiro inválido ---\n");
    int dummy = 42;
    mymemory_free(memory, &dummy);
    printf("✓ Função não causou erro com ponteiro inválido\n");
    
    printf("\n--- Teste 5: Alocar todo o pool ---\n");
    void *ptr3 = mymemory_alloc(memory, 1000);
    if (ptr3) {
        printf("✓ Conseguiu alocar todo o pool\n");
        mymemory_stats(memory);
        
        printf("\n--- Teste 6: Tentar alocar com pool cheio ---\n");
        void *ptr4 = mymemory_alloc(memory, 10);
        if (!ptr4) {
            printf("✓ Corretamente retornou NULL com pool cheio\n");
        }
        
        mymemory_free(memory, ptr3);
    }
    
    mymemory_cleanup(memory);
}

int main() {
    printf("╔═══════════════════════════════════════════════════╗\n");
    printf("║   GERENCIADOR DE MEMÓRIA - TRABALHO 2            ║\n");
    printf("║   Programação de Software Básico                 ║\n");
    printf("║   Estratégia: FIRST FIT                          ║\n");
    printf("╚═══════════════════════════════════════════════════╝\n");
    
    // Executa todos os testes
    test_basic();
    test_fragmentation();
    stress_test();
    test_edge_cases();
    
    printf("\n");
    printf("========================================\n");
    printf("TODOS OS TESTES CONCLUÍDOS!\n");
    printf("========================================\n");
    
    return 0;
}