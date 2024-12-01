#include <mpi.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int is_prime(int n) {
    for (int i = 2; i * i <= n; ++i) {
        if (n % i == 0) return 0;
    }
    return 1;
}

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    if (argc != 3) {
        printf("Use: ./lab [N_start] [N_end]\n");
        MPI_Finalize();
        return 0;
    }

    int A = atoi(argv[1]);
    int B = atoi(argv[2]);

    if (A > B) {
        int tmp = A;
        A = B;
        B = tmp;
    }

    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int block = (B - A) / size;
    int ost = (B - A) % size;

    int range[2]; 
    MPI_Status st;

    if (rank == 0) {
        for (int i = 1; i < size; ++i) {
            range[0] = A + i * block + (i < ost ? i : ost);
            range[1] = A + (i + 1) * block + (i < ost ? i + 1 : ost);
            MPI_Send(range, 2, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
        range[0] = A;
        range[1] = A + block + (ost > 0 ? 1 : 0);
    } else {
        MPI_Recv(range, 2, MPI_INT, 0, 0, MPI_COMM_WORLD, &st);
    }

    int *local_primes = (int*)malloc((range[1] - range[0]) * sizeof(int));
    int local_count = 0;

    double begin = MPI_Wtime();

    #pragma omp parallel for shared(range, local_primes) reduction(+:local_count)
    for (int i = range[0]; i < range[1]; ++i) {
        if (is_prime(i)) {
            local_primes[local_count] = i;
            local_count++;
        }
    }

    double end = MPI_Wtime();

    int *all_primes = NULL;
    int *recvcounts = NULL;
    int *displs = NULL;

    if (rank == 0) {
        all_primes = (int*)malloc((B - A) * sizeof(int));
        recvcounts = (int*)malloc(size * sizeof(int));
        displs = (int*)malloc(size * sizeof(int));

        int total_count = 0;
        for (int i = 0; i < size; ++i) {
            recvcounts[i] = (B - A) / size + (i < ost ? 1 : 0); 
            displs[i] = total_count;  
            total_count += recvcounts[i];
        }
    }

    MPI_Gather(&local_count, 1, MPI_INT, recvcounts, 1, MPI_INT, 0, MPI_COMM_WORLD);

    MPI_Gatherv(local_primes, local_count, MPI_INT, all_primes, recvcounts, displs, MPI_INT, 0, MPI_COMM_WORLD);

    double gend = MPI_Wtime();

    if (rank == 0) {
        printf("Range: %d to %d\n", A, B);
        printf("Primes found: ");
        for (int i = 0; i < recvcounts[0]; ++i) {
            printf("%d ", all_primes[i]);
        }
        printf("\n");
        printf("Total time: %f seconds\n", gend - begin);

        FILE *fp = fopen("log.txt", "a");
        fprintf(fp, "%d %f\n", size, gend - begin);
        fclose(fp);
    }

    free(local_primes);
    if (rank == 0) {
        free(all_primes);
        free(recvcounts);
        free(displs);
    }

    MPI_Finalize();
    return 0;
}

