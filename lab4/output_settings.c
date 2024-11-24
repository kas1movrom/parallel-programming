#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main() {
	printf("%d\n", _OPENMP);
	printf("%d\n", omp_get_num_procs());
	printf("%d\n", omp_get_max_threads());
	printf("%d\n", omp_get_dynamic());
	printf("%e\n", omp_get_wtick());
	printf("%d\n", omp_get_nested());
	printf("%d\n", omp_get_max_active_levels());
	omp_sched_t kind;
	int chunk_size;
	omp_get_schedule(&kind, &chunk_size);
	printf("%d - %d\n", kind, chunk_size);
}
