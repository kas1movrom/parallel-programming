#include <stdlib.h>
#include <stdio.h>

#include <mpi.h>

const int count = 10000000;
const int attempts = 10;
int random_seed = 932594;

double average = 0.0;

void fill_array(int* array) {
	for (int i = 0; i < count; ++i) {
		array[i] = rand();
	}
}

int find_local_max(int* array, int wstart, int wend) {
	int local_max = array[wstart];
	for (int i = wstart; i < wend; ++i) {
		if (array[i] > local_max) {
			local_max = array[i];
		}
	}
	return local_max;
}

int main(int argc, char** argv) {
	int size, rank;
	double begin, end;
	
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	int* array = (int*)malloc(sizeof(int) * count);
	if (array == NULL) {
		MPI_Abort(MPI_COMM_WORLD, 1);
		return 1;
	}

	for (int i = 0; i < attempts; ++i) {
		if (rank == 0) {
			srand(random_seed + i);
			fill_array(array);
		}

		MPI_Bcast(array, count, MPI_INT, 0, MPI_COMM_WORLD);

		const int wstart = rank * count / size;
		const int wend = (rank + 1) * count / size;

		MPI_Barrier(MPI_COMM_WORLD);

		if (rank == 0) {
			begin = MPI_Wtime();
		}

		int max;
		int local_max = find_local_max(array, wstart, wend);
		MPI_Reduce(&local_max, &max, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);

		if (rank == 0) {
			end = MPI_Wtime();
			average += (end - begin);
			printf("%lf\n", end - begin);
		} 
	}
	average /= 10;
	printf("Average: %lf\n", average);	

	free(array);
	MPI_Finalize();
	return 0;
}
