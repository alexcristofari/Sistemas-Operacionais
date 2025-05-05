#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct {
    int **A, **B, **C;
    int row_start, row_end, cols;
} ThreadData;

void *soma_matriz(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    for (int i = data->row_start; i < data->row_end; i++) {
        for (int j = 0; j < data->cols; j++) {
            data->C[i][j] = data->A[i][j] + data->B[i][j];
        }
    }
    return NULL;
}

int **alocar_matriz(int rows, int cols) {
    int **mat = malloc(rows * sizeof(int *));
    for (int i = 0; i < rows; i++)
        mat[i] = malloc(cols * sizeof(int));
    return mat;
}

void liberar_matriz(int **mat, int rows) {
    for (int i = 0; i < rows; i++)
        free(mat[i]);
    free(mat);
}

void preencher_matriz(int **mat, int rows, int cols, const char *nome) {
    printf("Preenchendo a matriz %s:\n", nome);
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            scanf("%d", &mat[i][j]);
}

void imprimir_matriz(int **mat, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++)
            printf("%d ", mat[i][j]);
        printf("\n");
    }
}

int main() {
    int rows, cols, num_threads;
    printf("Informe as dimensões da matriz (NxM): ");
    scanf("%d%d", &rows, &cols);
    printf("Informe o número de threads: ");
    scanf("%d", &num_threads);

    int **A = alocar_matriz(rows, cols);
    int **B = alocar_matriz(rows, cols);
    int **C = alocar_matriz(rows, cols);

    preencher_matriz(A, rows, cols, "A");
    preencher_matriz(B, rows, cols, "B");

    pthread_t threads[num_threads];
    ThreadData td[num_threads];
    int chunk = rows / num_threads;

    for (int i = 0; i < num_threads; i++) {
        td[i].A = A;
        td[i].B = B;
        td[i].C = C;
        td[i].row_start = i * chunk;
        td[i].row_end = (i == num_threads - 1) ? rows : (i + 1) * chunk;
        td[i].cols = cols;
        pthread_create(&threads[i], NULL, soma_matriz, &td[i]);
    }

    for (int i = 0; i < num_threads; i++)
        pthread_join(threads[i], NULL);

    printf("Resultado da soma:\n");
    imprimir_matriz(C, rows, cols);

    liberar_matriz(A, rows);
    liberar_matriz(B, rows);
    liberar_matriz(C, rows);
    return 0;
}
