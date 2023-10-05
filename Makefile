SIZE:=64
MPI_NP:=2
OMP_NUM_THREADS:=2

PHONY: run-v1
run-v1:
	time python3 v1.py $(SIZE)

PHONY: run-v2
run-v2:
	gcc -O1 -o v2 v2.c matrix.c utils.c -lcblas -lblas
	time ./v2 $(SIZE)

PHONY: run-v3
run-v3:
	gcc -O1 -march=native -o v3 v3.c matrix.c utils.c -lcblas -lblas
	time ./v3 $(SIZE)

PHONY: run-v4
run-v4:
	gcc -O1 -march=native -o v4 v4.c matrix.c utils.c -lcblas -lblas
	time ./v4 $(SIZE)

PHONY: run-v5
run-v5:
	gcc -O1 -march=native -o v5 v5.c matrix.c utils.c -lcblas -lblas
	time ./v5 $(SIZE)

PHONY: run-v6
run-v6:
	gcc -O1 -march=native -fopenmp -o v6 v6.c matrix.c utils.c -lcblas -lblas
	OMP_NUM_THREADS=$(OMP_NUM_THREADS) time ./v6 $(SIZE)

PHONY: run-v7
run-v7:
	mpicc -march=native -fopenmp -o v7 v7.c matrix.c utils.c -lm -lcblas -lblas
	OMP_NUM_THREADS=$(OMP_NUM_THREADS) time mpirun -np $(MPI_NP) ./v7 $(SIZE)

PHONY: run-v8
run-v8:
	mpicc -march=native -fopenmp -o v8 v8.c matrix.c utils.c -lm -lcblas -lblas
	OMP_NUM_THREADS=$(OMP_NUM_THREADS) time mpirun -np $(MPI_NP) ./v8 $(SIZE)

PHONY: run-v9
run-v9:
	gcc -O1 -march=native -o blas blas.c matrix.c utils.c -lcblas -lblas
	time ./blas $(SIZE)

PHONY: run-v10
run-v10:
	time python3 v10.py $(SIZE)

