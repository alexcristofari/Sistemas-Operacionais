#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define TAM_BUFFER 5
#define NUM_ITENS 10

int buffer[TAM_BUFFER];
int count = 0;         // número de itens no buffer
int in = 0, out = 0;   // índices para produzir/consumir

pthread_mutex_t mutex;
pthread_cond_t cheio, vazio;

void* produtor(void* arg) {
    for (int i = 1; i <= NUM_ITENS; i++) {
        pthread_mutex_lock(&mutex);

        while (count == TAM_BUFFER) {
            // Buffer cheio, espera
            pthread_cond_wait(&vazio, &mutex);
        }

        // Insere item
        buffer[in] = i;
        printf("Produtor: produziu %d\n", i);
        in = (in + 1) % TAM_BUFFER;
        count++;

        // Sinaliza que o buffer tem algo
        pthread_cond_signal(&cheio);
        pthread_mutex_unlock(&mutex);

        usleep(100000); // simula tempo de produção
    }

    pthread_exit(NULL);
}

void* consumidor(void* arg) {
    for (int i = 1; i <= NUM_ITENS; i++) {
        pthread_mutex_lock(&mutex);

        while (count == 0) {
            // Buffer vazio, espera
            pthread_cond_wait(&cheio, &mutex);
        }

        // Remove item
        int item = buffer[out];
        printf("Consumidor: consumiu %d\n", item);
        out = (out + 1) % TAM_BUFFER;
        count--;

        // Sinaliza que o buffer tem espaço
        pthread_cond_signal(&vazio);
        pthread_mutex_unlock(&mutex);

        usleep(150000); // simula tempo de consumo
    }

    pthread_exit(NULL);
}

int main() {
    pthread_t thProdutor, thConsumidor;

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cheio, NULL);
    pthread_cond_init(&vazio, NULL);

    pthread_create(&thProdutor, NULL, produtor, NULL);
    pthread_create(&thConsumidor, NULL, consumidor, NULL);

    pthread_join(thProdutor, NULL);
    pthread_join(thConsumidor, NULL);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cheio);
    pthread_cond_destroy(&vazio);

    printf("Fim do programa produtor-consumidor.\n");
    return 0;
}

