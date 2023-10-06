#include "matrix.h"
#include "utils.h"
#include <immintrin.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define UNROLL (4)
#define BLOCK_SIZE 32

void block_multiply(Matrix *A, Matrix *B, Matrix *result, int i_start,
                    int j_start, int k_start) {
  for (int i = 0; i < BLOCK_SIZE; i += UNROLL) {
    for (int j = 0; j < BLOCK_SIZE; j += 4) {
      __m256d sums[UNROLL];
      for (int r = 0; r < UNROLL; r++) {
        sums[r] =
            _mm256_load_pd(&result->elements[i_start + i + r][j_start + j]);
      }

      for (int k = 0; k < BLOCK_SIZE; k++) {
        __m256d b = _mm256_load_pd(&B->elements[k_start + k][j_start + j]);
        for (int r = 0; r < UNROLL; r++) {
          __m256d a =
              _mm256_broadcast_sd(&A->elements[i_start + i + r][k_start + k]);
          sums[r] = _mm256_fmadd_pd(a, b, sums[r]);
        }
      }

      for (int r = 0; r < UNROLL; r++) {
        _mm256_store_pd(&result->elements[i_start + i + r][j_start + j],
                        sums[r]);
      }
    }
  }
}

void matrix_multiplication(Matrix *A, Matrix *B, Matrix *local_result,
                           int start, int end) {
  int size = A->size;
#pragma omp parallel for
  for (int i = start; i < end; i += BLOCK_SIZE) {
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
  struct timeval start_time, end_time;

  int world_rank, world_size;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  int per_process = size / world_size;

  A = create_matrix(size, size);
  B = create_matrix(size, size);
  if (world_rank == 0) {
    total_result = create_matrix(size, size);
    initialize_matrix(A);
    initialize_matrix(B);
  }

  gettimeofday(&start_time, NULL);

  MPI_Bcast(&(B->linear[0]), size * size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

  Matrix *sub_A = create_matrix(per_process, size);
  MPI_Scatter(&(A->linear[world_rank * per_process]), per_process * size,
              MPI_DOUBLE, sub_A->linear, per_process * size, MPI_DOUBLE, 0,
              MPI_COMM_WORLD);

  // Do the multiplication
  local_result = create_matrix(per_process, size);
  matrix_multiplication(sub_A, B, local_result, 0, per_process);

  // Gather the results
  int mpi_block_size = size * per_process;
  int start = world_rank * per_process;
  int start_pos = start * size;
  MPI_Gather(&(local_result->linear[0]), mpi_block_size, MPI_DOUBLE,
             &(total_result->linear[start_pos]), mpi_block_size, MPI_DOUBLE, 0,
             MPI_COMM_WORLD);

  gettimeofday(&end_time, NULL);

  if (world_rank == 0) {
    double delta = ((end_time.tv_sec - start_time.tv_sec) * 1000000u +
                    end_time.tv_usec - start_time.tv_usec) /
                   1.e6;
    printf("Size: %dx%d, Execution Time: %f ms\n", size, size, delta * 1000);

    Matrix *result_blas = create_matrix(size, size);
    matrix_multiplication_blas(A, B, result_blas);
    check_results(total_result, result_blas, size);
    free_matrix(result_blas);
  }

  MPI_Finalize();
  return 0;
}
