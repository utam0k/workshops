#include "matrix.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Usage: %s [size]\n", argv[0]);
    return 1;
  }

  int size = atoi(argv[1]);
  Matrix *A = create_matrix(size, size);
  Matrix *B = create_matrix(size, size);
  Matrix *result_blas = create_matrix(size,size);

  initialize_matrix(A);
  initialize_matrix(B);

  struct timeval start, end;

  gettimeofday(&start, NULL);
  matrix_multiplication_blas(A, B, result_blas);
  gettimeofday(&end, NULL);

  double elapsed = (end.tv_sec - start.tv_sec) * 1000.0 +
                   (end.tv_usec - start.tv_usec) / 1000.0;
  printf("Size: %dx%d, Execution Time: %f ms\n", size, size, elapsed);

  free_matrix(result_blas);

  return 0;
}
