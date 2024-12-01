#include <stdio.h>
#include <stdlib.h>

#include <omp.h>

const int count = 10000000;
int max = -1;
double avg_sum = 0.0;

void fill_array(int* array) {
	for (int i = 0; i < count; i++) {
		array[i] = rand();
	}
}

double sort_array(int* array, int threads) {
	double start_time = omp_get_wtime();
	#pragma omp parallel num_threads(threads) shared(array, count) reduction(max: max) default(none) 
	{
		#pragma omp for
		for (int i = 0; i < count; i++) {
			if (array[i] > max) {
				max = array[i];
			}
		}	
	}
	double end_time = omp_get_wtime();
	return (end_time - start_time);
}

void control_method(int threads, int random_seed) {
	srand(random_seed);
	int* array = (int*)malloc(sizeof(int) * count);
	fill_array(array);
	double result = sort_array(array, threads);
	printf("%f\n", result);
	avg_sum += result;
	free(array);
}



int main() {
	printf("%d\n", omp_get_max_threads());

	for (int i = 1; i <= 4; i++) {
		int random_seed = 920215;
		for (int j = 0; j < 50; j++) {
			control_method(i, random_seed + j);	
		}
		avg_sum /= 50.0;
		printf("--------------------------------\nAVERAGE_SUM: %f\n", avg_sum);
		avg_sum = 0.0;
	}

	return 0;
}
