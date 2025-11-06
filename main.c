#include <stdio.h>
#include "mymemory.h"

int main() {
    // aloca pool de memória
    mymemory_t *memory = mymemory_init(1000);

    // int *v = malloc(sizeof(int)*32);
    int *v = (int*)mymemory_alloc(memory, sizeof(int) * 32);
    if (!v) {
        printf("Falha ao alocar v (32 ints)\n");
        mymemory_cleanup(memory);
        return 1;
    }

    for (int i = 0; i < 32; i++)
        v[i] = i + 1;

    int *v2 = (int*)mymemory_alloc(memory, sizeof(int) * 16);
    if (!v2) {
        printf("Falha ao alocar v2 (16 ints)\n");
        // continua mesmo assim para testar free e cleanup
    }

    // free(v);
    mymemory_free(memory, v);

    char *str = (char*)mymemory_alloc(memory, sizeof(char) * 16);
    if (!str) {
        printf("Falha ao alocar str (16 chars)\n");
    } else {
        sprintf(str, "ola mundo");
        printf("str: %s\n", str);
    }

    // opcional: visualizar alocações e estatísticas
    mymemory_display(memory);
    mymemory_stats(memory);

    // libera o pool de memória
    mymemory_cleanup(memory);

    return 0;
}