#include <stdio.h>
#include <stdlib.h>

#include <omp.h>

const int count = 100000;
const int thread_numb = 4;
const int attempts = 10;

double avg_time = 0.0;

void fill_array(int* array) {
	for (int i = 0; i < count; i++) {
		array[i] = rand();
	}
}

void shell_sort(int* array, int threads, double* time_array) {
	double start_time = omp_get_wtime();
	#pragma omp parallel num_threads(threads) shared(count, array, threads) default(none)
	{
		for (int s = count / 2; s > 0; s /= 2) {
			#pragma omp for
			for (int i = s; i < count; i++) {
				for (int j = i - s; j >= 0 && array[j] > array[j + s]; j-= s) {
					int tmp = array[j];
					array[j] = array[j + s];
					array[j + s] = tmp;
				}
			}
		}
	}
	double end_time = omp_get_wtime();
	*time_array = end_time - start_time;
}

void control_method(int threads, int random_seed, double* time_array) {
	srand(random_seed);
	int* array = (int*)malloc(sizeof(int) * count);
	fill_array(array);
	shell_sort(array, threads, time_array);
	free(array);
}

int main() {
	double time_array[thread_numb][attempts];
	for (int i = thread_numb; i >= 1; i--) {
		int random_seed = 920214;
		for (int j = 0; j < attempts; j++) {
			control_method(i, random_seed + j, &time_array[i - 1][j]);
			printf("time: %f\n", time_array[i - 1][j]);
			avg_time += time_array[i - 1][j];	
		}
		avg_time /= attempts;
		printf("____________________________\nAVERAGE TIME: %f\n", avg_time);
		avg_time = 0.0;
	}
}
