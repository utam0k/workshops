#include "utils.h"
#include <cblas.h>
#include <stdio.h>
#include <stdlib.h>

void matrix_multiplication_blas(Matrix* A, Matrix* B, Matrix* result) {
    int size = A->size;
    cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans,
                size, size, size, 1.0, A->linear, size, B->linear, size, 0.0, result->linear, size);
}

void check_results(Matrix* result, Matrix* result_blas, int size) {
    for(int i = 0; i < size; i++) {
        for(int j = 0; j < size; j++) {
            if(abs(result->elements[i][j] - result_blas->elements[i][j]) > 1e-6) {
                printf("Mismatch found at (%d, %d): %f vs %f\n", i, j, result->elements[i][j], result_blas->elements[i][j]);
            }
        }
    }
}

