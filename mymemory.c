#include "mymemory.h"
#include <stdlib.h>
#include <stdio.h>

// Inicializa o gerenciador de memória com um pool de tamanho especificado
mymemory_t* mymemory_init(size_t size) {
    mymemory_t *memory = (mymemory_t*)malloc(sizeof(mymemory_t));
    if (!memory) {
        return NULL;
    }
    
    memory->pool = malloc(size);
    if (!memory->pool) {
        free(memory);
        return NULL;
    }
    
    memory->total_size = size;
    memory->head = NULL;
    
    return memory;
}

// Aloca memória usando estratégia First Fit
void* mymemory_alloc(mymemory_t *memory, size_t size) {
    if (!memory || size == 0) {
        return NULL;
    }
    
    void *alloc_start = NULL;
    void *current_pos = memory->pool;
    allocation_t *current_alloc = memory->head;
    
    // Procura o primeiro espaço livre que cabe (First Fit)
    while (current_pos < memory->pool + memory->total_size) {
        if (current_alloc && current_alloc->start == current_pos) {
            // Há uma alocação aqui, pula para depois dela
            current_pos = current_alloc->start + current_alloc->size;
            current_alloc = current_alloc->next;
        } else {
            // Encontrou um gap - calcula o tamanho dele
            void *next_alloc_start = current_alloc ? current_alloc->start : memory->pool + memory->total_size;
            size_t gap_size = next_alloc_start - current_pos;
            
            // Se o gap é grande o suficiente, usa ele
            if (gap_size >= size) {
                alloc_start = current_pos;
                break;
            }
            
            // Gap muito pequeno, continua procurando
            current_pos = next_alloc_start;
        }
    }
    
    // Não encontrou espaço suficiente
    if (!alloc_start) {
        return NULL;
    }
    
    // Cria nova alocação
    allocation_t *new_alloc = (allocation_t*)malloc(sizeof(allocation_t));
    if (!new_alloc) {
        return NULL;
    }
    
    new_alloc->start = alloc_start;
    new_alloc->size = size;
    new_alloc->next = NULL;
    
    // Insere na lista ordenada por endereço
    if (!memory->head || memory->head->start > alloc_start) {
        new_alloc->next = memory->head;
        memory->head = new_alloc;
    } else {
        allocation_t *current = memory->head;
        while (current->next && current->next->start < alloc_start) {
            current = current->next;
        }
        new_alloc->next = current->next;
        current->next = new_alloc;
    }
    
    return alloc_start;
}

// Libera uma alocação
void mymemory_free(mymemory_t *memory, void *ptr) {
    if (!memory || !ptr) {
        return;
    }
    
    allocation_t *current = memory->head;
    allocation_t *prev = NULL;
    
    while (current) {
        if (current->start == ptr) {
            if (prev) {
                prev->next = current->next;
            } else {
                memory->head = current->next;
            }
            free(current);
            return;
        }
        prev = current;
        current = current->next;
    }
}

// Exibe todas as alocações atuais
void mymemory_display(mymemory_t *memory) {
    if (!memory) {
        printf("Memória não inicializada\n");
        return;
    }
    
    printf("\n=== Alocações Atuais ===\n");
    
    if (!memory->head) {
        printf("Nenhuma alocação presente\n");
        return;
    }
    
    allocation_t *current = memory->head;
    int count = 1;
    
    while (current) {
        printf("Alocação %d: Início=%p, Tamanho=%zu bytes\n", 
               count++, current->start, current->size);
        current = current->next;
    }
}

// Exibe estatísticas sobre a memória
void mymemory_stats(mymemory_t *memory) {
    if (!memory) {
        printf("Memória não inicializada\n");
        return;
    }
    
    printf("\n=== Estatísticas de Memória ===\n");
    
    // Conta alocações e memória alocada
    int num_allocations = 0;
    size_t allocated_memory = 0;
    allocation_t *current = memory->head;
    
    while (current) {
        num_allocations++;
        allocated_memory += current->size;
        current = current->next;
    }
    
    // Calcula memória livre e fragmentação
    size_t free_memory = memory->total_size - allocated_memory;
    size_t largest_free_block = 0;
    int num_free_fragments = 0;
    
    void *pos = memory->pool;
    allocation_t *alloc = memory->head;
    
    while (pos < memory->pool + memory->total_size) {
        if (alloc && alloc->start == pos) {
            pos = alloc->start + alloc->size;
            alloc = alloc->next;
        } else {
            void *next_alloc_start = alloc ? alloc->start : memory->pool + memory->total_size;
            size_t gap_size = next_alloc_start - pos;
            
            if (gap_size > 0) {
                num_free_fragments++;
                if (gap_size > largest_free_block) {
                    largest_free_block = gap_size;
                }
            }
            pos = next_alloc_start;
        }
    }
    
    printf("Número total de alocações: %d\n", num_allocations);
    printf("Memória total alocada: %zu bytes\n", allocated_memory);
    printf("Memória total livre: %zu bytes\n", free_memory);
    printf("Maior bloco contíguo livre: %zu bytes\n", largest_free_block);
    printf("Número de fragmentos livres: %d\n", num_free_fragments);
    printf("Taxa de utilização: %.2f%%\n", 
           (allocated_memory * 100.0) / memory->total_size);
}

// Libera todos os recursos
void mymemory_cleanup(mymemory_t *memory) {
    if (!memory) {
        return;
    }
    
    // Libera todas as alocações da lista
    allocation_t *current = memory->head;
    while (current) {
        allocation_t *next = current->next;
        free(current);
        current = next;
    }
    
    // Libera o pool de memória
    free(memory->pool);
    
    // Libera a estrutura principal
    free(memory);
}