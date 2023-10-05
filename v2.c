#include "matrix.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

void matrix_multiplication(Matrix *A, Matrix *B, Matrix *result) {
  int size = A->size;
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      double sum = 0.0;
      for (int k = 0; k < size; k++) {
        sum += A->elements[i][k] * B->elements[k][j];
      }
      result->elements[i][j] = sum;
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
  Matrix *result_blas = create_matrix(size,size);

  initialize_matrix(A);
  initialize_matrix(B);

  struct timeval start, end;
  gettimeofday(&start, NULL);
  matrix_multiplication(A, B, result);
  gettimeofday(&end, NULL);
  double elapsed = (end.tv_sec - start.tv_sec) * 1000.0 +
                   (end.tv_usec - start.tv_usec) / 1000.0;
  printf("Size: %dx%d, Execution Time: %f ms\n", size, size, elapsed);

  matrix_multiplication_(A, B, result_blas);
  check_results(result, result_blas, size);

  free_matrix(A);
  free_matrix(B);
  free_matrix(result);
  free_matrix(result_blas);

  return 0;
}
