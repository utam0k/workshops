#include "matrix.h"
#include "utils.h"
#include <immintrin.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define UNROLL (4)
#define BLOCK_SIZE 32

void block_multiply(Matrix *A, Matrix *B, Matrix *result, int i_start, int j_start, int k_start) {
  for (int i = 0; i < BLOCK_SIZE; i += UNROLL) {
    for (int j = 0; j < BLOCK_SIZE; j += 4) {
      __m256d sums[UNROLL];
      for (int r = 0; r < UNROLL; r++) {
        sums[r] = _mm256_load_pd(&result->elements[i_start + i + r][j_start + j]);
      }

      for (int k = 0; k < BLOCK_SIZE; k++) {
        __m256d b = _mm256_load_pd(&B->elements[k_start + k][j_start + j]);
        for (int r = 0; r < UNROLL; r++) {
          __m256d a = _mm256_broadcast_sd(&A->elements[i_start + i + r][k_start + k]);
          sums[r] = _mm256_fmadd_pd(a, b, sums[r]);
        }
      }

      for (int r = 0; r < UNROLL; r++) {
        _mm256_store_pd(&result->elements[i_start + i + r][j_start + j], sums[r]);
      }
    }
  }
}

void matrix_multiplication(Matrix *A, Matrix *B, Matrix *local_result, int start_row, int end_row ) {
  int size = A->size;
#pragma omp parallel for
  for (int i = start_row; i < end_row; i += BLOCK_SIZE) {
    for (int j = 0; j < size; j += BLOCK_SIZE) {
      for (int k = 0; k < size; k += BLOCK_SIZE) {
        block_multiply(A, B, local_result, i, j, k);
      }
    }
  }
}

int main(int argc, char **argv) {
  MPI_Init(NULL, NULL);

  int size = atoi(argv[1]);

  Matrix *A, *B, *total_result, *local_result;
  struct timeval start, end;

  int world_rank, world_size;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  A = create_matrix(size, size);
  B = create_matrix(size, size);
  total_result = create_matrix(size, size);
  if (world_rank == 0) {
    initialize_matrix(A);
    initialize_matrix(B);
  }

  gettimeofday(&start, NULL);

  MPI_Bcast(&(A->linear[0]), size * size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  MPI_Bcast(&(B->linear[0]), size * size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

  // Do the multiplication
  int rows_per_process = size / world_size;
  int start_row = world_rank * rows_per_process;
  int end_row = (world_rank + 1) * rows_per_process;
  local_result = create_matrix(size, size);
  matrix_multiplication(A, B, local_result, start_row, end_row);


  /* Gather the results */
  int mpi_block_size = size * rows_per_process;
  int start_pos = start_row * size;
  MPI_Gather(&(local_result->linear[start_pos]), mpi_block_size, MPI_DOUBLE, &(total_result->linear[start_pos]), mpi_block_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

  gettimeofday(&end, NULL);

  if (world_rank == 0) {
    double delta = ((end.tv_sec - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;
    printf("Size: %dx%d, Execution Time: %f ms\n", size, size, delta * 1000);

    Matrix *result_blas = create_matrix(size, size);
    matrix_multiplication_blas(A, B, result_blas);
    check_results(total_result, result_blas, size);
    free_matrix(result_blas);
  }

  MPI_Finalize();
  return 0;
}
