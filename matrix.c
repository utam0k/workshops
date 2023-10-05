#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

Matrix* create_matrix(int rows, int cols) {
    Matrix* matrix = (Matrix*)malloc(sizeof(Matrix));
    if (!matrix) {
        perror("Failed to malloc for Matrix");
        return NULL;
    }

    matrix->elements = (double**)malloc(rows * sizeof(double*));
    if (!matrix->elements) {
        perror("Failed to malloc for Matrix elements");
        free(matrix);
        return NULL;
    }

    int result = posix_memalign((void**)&matrix->linear, 32, sizeof(double) * rows * cols);
    if (result != 0) {
        perror("Failed to allocate aligned memory for matrix data");
        free(matrix->elements);
        free(matrix);
        return NULL;
    }

    for (int i = 0; i < rows; i++) {
        matrix->elements[i] = &(matrix->linear[i * cols]);
    }
    
    matrix->size = cols;  // Storing the number of columns in the "size" field
    return matrix;
}



void free_matrix(Matrix* matrix) {
    free(matrix->linear);
    free(matrix->elements);
    free(matrix);
}

void initialize_matrix(Matrix* matrix) {
    int size = matrix->size;
    for (int i = 0; i < size * size; i++) {
        matrix->linear[i] = (double)rand() / RAND_MAX * 2.0 - 1.0;
    }
}

