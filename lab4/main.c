#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

const int count = 100000000;
double avg_sum = 0.0;

void fill_array(int* array) {
	for (int i = 0; i < count; i++) {
		array[i] = rand();
	}
}

double find_max(int* array, int threads, int chunk) {
	int max = array[0];
	double start_time = omp_get_wtime();
	#pragma omp parallel num_threads(threads) shared(array, count, chunk) reduction(max: max) default(none)
	{
		#pragma omp for schedule(dynamic, chunk)
		for (int i = 0; i < count; i++) {
			max = (array[i] > max) ? array[i] : max;
		}
	}
	double end_time = omp_get_wtime();
	return (end_time - start_time);
}

void control_method(int threads, int seed, int chunk) {
	srand(seed);
	int *array = (int*) malloc(sizeof(int) * count);
	fill_array(array);
	double result = find_max(array, threads, chunk);
	printf("%f\n", result);
	avg_sum += result;
	free(array);
}

int main(int argc, char **argv) {
	if (argc != 4) {
		printf("Error! The arguments must look like ./labn threads seed\n");
		return -1;
	}	
	int threads = atoi(argv[1]);
	int seed = atoi(argv[2]);
	int chunk = atoi(argv[3]);

	for (int i = 0; i < 10; i++) {
		control_method(threads, seed + i, chunk);
	}
	avg_sum /= 10.0;
	printf("----------\nAVERAGE_SUM: %f\n", avg_sum);
	return 0;
}
