import numpy as np
import time
import sys

def initialize_matrix(rows, cols):
    return np.random.rand(rows, cols)

if len(sys.argv) != 2:
    print("Usage: python script_name.py matrix_size")
    sys.exit(1)

size = int(sys.argv[1])
A = initialize_matrix(size, size)
B = initialize_matrix(size, size)
start_time = time.time()
result = np.dot(A, B)  # Use numpy's dot function for matrix multiplication
end_time = time.time()
print(f"Size: {size}x{size}, Execution Time: {(end_time - start_time) * 1000:.2f} ms")

