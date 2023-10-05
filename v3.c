#include "matrix.h"
#include "utils.h"
#include <immintrin.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

void matrix_multiplication(Matrix *A, Matrix *B, Matrix *result) {
  int size = A->size;
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j += 4) {
      // Initialize sum with zeros using AVX2
      __m256d sum = _mm256_setzero_pd();

      for (int k = 0; k < size; k++) {
        // Broadcast A[i][k] to all elements of a 256-bit vector using AVX2
        __m256d a = _mm256_broadcast_sd(&A->elements[i][k]);

        // Load 4 consecutive values from B[k][j] using AVX2
        __m256d b = _mm256_load_pd(&B->elements[k][j]);

        // Multiply the vectors and accumulate the result using AVX2
        sum = _mm256_add_pd(sum, _mm256_mul_pd(a, b));
      }

      // Store the result back to the result matrix using AVX2
      _mm256_store_pd(&result->elements[i][j], sum);
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
