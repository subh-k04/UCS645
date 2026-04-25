default:
	mpicc -O3 -o daxpy daxpy.c -lm
run:
	mpirun -np 4 ./daxpy
clean:
	rm -f daxpy
