#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define N 3  // Tamanho do buffer
#define NUM_PROD 3
#define NUM_CONS 2

typedef int tipo_dado;
tipo_dado buffer[N];
int proxima_insercao = 0;
int proxima_remocao = 0;

// Semáforos
sem_t exclusao_mutua;
sem_t espera_vaga;
sem_t espera_dado;

// Função de produção
tipo_dado produzir() {
    tipo_dado valor = rand() % 100;
    printf("[PRODUTOR] Produziu: %d\n", valor);
    return valor;
}

// Função de consumo
void consumir(tipo_dado dado) {
    printf("[CONSUMIDOR] Consumiu: %d\n", dado);
}

// Produtor
void* produtor(void* arg) {
    while (1) {
        tipo_dado item = produzir();
        sem_wait(&espera_vaga);
        sem_wait(&exclusao_mutua);

        buffer[proxima_insercao] = item;
        printf(">> Inserido em %d: %d\n", proxima_insercao, item);
        proxima_insercao = (proxima_insercao + 1) % N;

        sem_post(&exclusao_mutua);
        sem_post(&espera_dado);

        sleep(1);  // Simula tempo de produção
    }
    return NULL;
}

// Consumidor
void* consumidor(void* arg) {
    while (1) {
        sem_wait(&espera_dado);
        sem_wait(&exclusao_mutua);

        tipo_dado item = buffer[proxima_remocao];
        printf("<< Removido de %d: %d\n", proxima_remocao, item);
        proxima_remocao = (proxima_remocao + 1) % N;

        sem_post(&exclusao_mutua);
        sem_post(&espera_vaga);

        consumir(item);
        sleep(2);  // Simula tempo de consumo
    }
    return NULL;
}

int main() {
    srand(time(NULL));

    pthread_t prod_threads[NUM_PROD];
    pthread_t cons_threads[NUM_CONS];

    // Inicializa semáforos
    sem_init(&exclusao_mutua, 0, 1);
    sem_init(&espera_vaga, 0, N);
    sem_init(&espera_dado, 0, 0);

    // Cria threads de produtores
    for (int i = 0; i < NUM_PROD; i++) {
        pthread_create(&prod_threads[i], NULL, produtor, NULL);
    }

    // Cria threads de consumidores
    for (int i = 0; i < NUM_CONS; i++) {
        pthread_create(&cons_threads[i], NULL, consumidor, NULL);
    }

    // Espera as threads (nunca acontece neste exemplo)
    for (int i = 0; i < NUM_PROD; i++) {
        pthread_join(prod_threads[i], NULL);
    }
    for (int i = 0; i < NUM_CONS; i++) {
        pthread_join(cons_threads[i], NULL);
    }

    return 0;
}

