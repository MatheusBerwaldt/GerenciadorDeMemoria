
#ifndef MYMEMORY_H
#define MYMEMORY_H

#include <stddef.h>

// Estrutura que representa um bloco de memória alocado
typedef struct allocation {
    void *start;
    size_t size;
    struct allocation *next;
} allocation_t;

// Estrutura que representa o pool total de memória
typedef struct {
    void *pool;              // ponteiro para o bloco de memória real
    size_t total_size;       // tamanho total do pool
    allocation_t *head;      // ponteiro para lista encadeada de alocações
} mymemory_t;

// Funções principais
mymemory_t* mymemory_init(size_t size);
void* mymemory_alloc(mymemory_t *memory, size_t size);
void mymemory_free(mymemory_t *memory, void *ptr);
void mymemory_display(mymemory_t *memory);
void mymemory_stats(mymemory_t *memory);
void mymemory_cleanup(mymemory_t *memory);

#endif // MYMEMORY_H