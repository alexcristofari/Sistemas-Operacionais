#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct {
    int **matriz;
    int *vetor;
    int *resultado;
    int n;
    int row_start;
    int row_end;
} ThreadData;

void *multiplica_matriz_vetor(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    for (int i = data->row_start; i < data->row_end; i++) {
        data->resultado[i] = 0;
        for (int j = 0; j < data->n; j++) {
            data->resultado[i] += data->matriz[i][j] * data->vetor[j];
        }
    }
    return NULL;
}

int **alocar_matriz(int n) {
    int **mat = malloc(n * sizeof(int *));
    if (mat == NULL) {
        perror("Erro ao alocar linhas da matriz");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < n; i++) {
        mat[i] = malloc(n * sizeof(int));
        if (mat[i] == NULL) {
            perror("Erro ao alocar colunas da matriz");
            exit(EXIT_FAILURE);
        }
    }
    return mat;
}

void liberar_matriz(int **mat, int n) {
    for (int i = 0; i < n; i++)
        free(mat[i]);
    free(mat);
}

int main() {
    int n, num_threads;

    printf("Informe o tamanho N da matriz e vetor: ");
    scanf("%d", &n);

    if (n <= 0) {
        printf("Tamanho inválido.\n");
        return 1;
    }

    printf("Informe o número de threads: ");
    scanf("%d", &num_threads);

    if (num_threads <= 0) num_threads = 1;
    if (num_threads > n) num_threads = n; 

    int **matriz = alocar_matriz(n);
    int *vetor = malloc(n * sizeof(int));
    int *resultado = malloc(n * sizeof(int));

    if (vetor == NULL || resultado == NULL) {
        perror("Erro ao alocar vetor ou resultado");
        exit(EXIT_FAILURE);
    }

    printf("Preencha a matriz %dx%d:\n", n, n);
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            scanf("%d", &matriz[i][j]);

    printf("Preencha o vetor de tamanho %d:\n", n);
    for (int i = 0; i < n; i++)
        scanf("%d", &vetor[i]);

    pthread_t threads[num_threads];
    ThreadData td[num_threads];
    int chunk = n / num_threads;

    for (int i = 0; i < num_threads; i++) {
        td[i].matriz = matriz;
        td[i].vetor = vetor;
        td[i].resultado = resultado;
        td[i].n = n;
        td[i].row_start = i * chunk;
        td[i].row_end = (i == num_threads - 1) ? n : (i + 1) * chunk;

        pthread_create(&threads[i], NULL, multiplica_matriz_vetor, &td[i]);
    }

    for (int i = 0; i < num_threads; i++)
        pthread_join(threads[i], NULL);

    printf("Resultado (matriz x vetor):\n");
    for (int i = 0; i < n; i++)
        printf("%d\n", resultado[i]);

    liberar_matriz(matriz, n);
    free(vetor);
    free(resultado);

    return 0;
}
