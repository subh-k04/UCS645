default:
	mpicc -O3 -o broadcast_race broadcast_race.c -lm
run:
	mpirun -np 4 ./broadcast_race
clean:
	rm -f broadcast_race
