#ifndef MATRIX_H
#define MATRIX_H

typedef struct {
    double** elements;
    double* linear;
    int size;
} Matrix;

Matrix* create_matrix(int rows, int cols);
void free_matrix(Matrix* matrix);
void initialize_matrix(Matrix* matrix);

#endif // MATRIX_H

