#ifndef UTILS_H
#define UTILS_H

#include "matrix.h"

void matrix_multiplication_blas(Matrix* A, Matrix* B, Matrix* result);
void check_results(Matrix* result, Matrix* result_blas, int size);

#endif // UTILS_H

