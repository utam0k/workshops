import random
import time
import sys

def initialize_matrix(rows, cols):
    return [[random.random() for _ in range(cols)] for _ in range(rows)]

def matrix_multiplication(A, B):
    rows_A, cols_A = len(A), len(A[0])
    rows_B, cols_B = len(B), len(B[0])
    if cols_A != rows_B:
        raise ValueError("Matrix dimensions do not match for multiplication.")
    result = [[0 for _ in range(cols_B)] for _ in range(rows_A)]
    for i in range(rows_A):
        for j in range(cols_B):
            for k in range(cols_A):
                result[i][j] += A[i][k] * B[k][j]
    return result

if len(sys.argv) != 2:
    print("Usage: python script_name.py matrix_size")
    sys.exit(1)

size = int(sys.argv[1])
A = initialize_matrix(size, size)
B = initialize_matrix(size, size)
start_time = time.time()
result = matrix_multiplication(A, B)
end_time = time.time()
print(f"Size: {size}x{size}, Execution Time: {(end_time - start_time) * 1000} ms")

