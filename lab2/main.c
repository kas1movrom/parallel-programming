#include <stdio.h>
#include <stdlib.h>

#include <omp.h>

const int count = 10000000;
const int thread_numb = 4;
const int attempts = 10;
const int target = 16;
double avg_time = 0.0;

void fill_array(int* array) {
	for (int i = 0; i < count; i++) {
		array[i] = rand() % 1000;
	}
}

double find_match(int* array, int threads, double* time) {
	int index = -1;
	char flag = 0;
	double start_time = omp_get_wtime();
	#pragma omp parallel num_threads(threads) shared(index, count, array, target, flag, threads) default(none)
	{
		#pragma omp for
		for (int i = 0; i < count; i++) {
			if (flag == 1) {
				#pragma omp cancel for
			}
			if (array[i] == target) {
				index = i;
				flag = 1;
			}
		}
	}
	double end_time = omp_get_wtime();
	*time = (end_time - start_time);
	return index;
}

void control_method(int threads, int random_seed, int* result_index, double* time) {
	srand(random_seed);
	int* array = (int*)malloc(sizeof(int) * count);
	fill_array(array);
	*result_index = find_match(array, threads, time);
	free(array);
}

int main() {
	int indexes_array[thread_numb][attempts];
	double time_array[thread_numb][attempts];

	for (int i = 1; i <= thread_numb; i++) {
		int random_seed = 920214;
		for (int j = 0; j < attempts; j++) {
			control_method(i, random_seed + j, 
				       &indexes_array[i - 1][j], &time_array[i - 1][j]);
			printf("time: %f\n", time_array[i - 1][j]);
			avg_time += time_array[i - 1][j];	
		}
		avg_time /= attempts;
		printf("_________________________\nAVERAGE TIME: %f\n", avg_time);
		avg_time = 0.0;
	}	
}
