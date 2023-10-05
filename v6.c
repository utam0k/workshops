#include "matrix.h"
#include "utils.h"
#include <immintrin.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define UNROLL (4)
#define BLOCK_SIZE 32

void block_multiply(Matrix *A, Matrix *B, Matrix *result, int i_start,
                    int j_start, int k_start) {
  for (int ii = i_start; ii < i_start + BLOCK_SIZE; ii += UNROLL) {
    for (int jj = j_start; jj < j_start + BLOCK_SIZE; jj += 4) {
      __m256d sums[UNROLL];
      for (int r = 0; r < UNROLL; r++) {
        sums[r] = _mm256_load_pd(&result->elements[ii + r][jj]);
      }

      for (int kk = k_start; kk < k_start + BLOCK_SIZE; kk++) {
        __m256d b = _mm256_load_pd(&B->elements[kk][jj]);
        for (int r = 0; r < UNROLL; r++) {
          __m256d a = _mm256_broadcast_sd(&A->elements[ii + r][kk]);
          sums[r] = _mm256_fmadd_pd(a, b, sums[r]);
        }
      }

      for (int r = 0; r < UNROLL; r++) {
        _mm256_store_pd(&result->elements[ii + r][jj], sums[r]);
      }
    }
  }
}

void matrix_multiplication(Matrix *A, Matrix *B, Matrix *result) {
  int size = A->size;
  #pragma omp parallel for
  for (int i = 0; i < size; i += BLOCK_SIZE) {
    for (int j = 0; j < size; j += BLOCK_SIZE) {
      for (int k = 0; k < size; k += BLOCK_SIZE) {
        block_multiply(A, B, result, i, j, k);
      }
    }
  }
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Usage: %s [size]\n", argv[0]);
    return 1;
  }

  int size = atoi(argv[1]);
  Matrix *A = create_matrix(size, size);
  Matrix *B = create_matrix(size, size);
  Matrix *result = create_matrix(size, size);
  Matrix *result_blas = create_matrix(size, size);

  initialize_matrix(A);
  initialize_matrix(B);

  struct timeval start, end;
  gettimeofday(&start, NULL);
  matrix_multiplication(A, B, result);
  gettimeofday(&end, NULL);
  double elapsed = (end.tv_sec - start.tv_sec) * 1000.0 +
                   (end.tv_usec - start.tv_usec) / 1000.0;
  printf("Size: %dx%d, Execution Time: %f ms\n", size, size, elapsed);

  matrix_multiplication_blas(A, B, result_blas);
  check_results(result, result_blas, size);

  free_matrix(A);
  free_matrix(B);
  free_matrix(result);
  free_matrix(result_blas);

  return 0;
}
