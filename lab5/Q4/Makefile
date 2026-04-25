default:
	mpicc -O3 -o prime_sieve prime_sieve.c -lm
run:
	mpirun -np 4 ./prime_sieve
clean:
	rm -f prime_sieve
