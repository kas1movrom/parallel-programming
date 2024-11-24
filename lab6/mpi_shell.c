#include <stdlib.h> 
#include <stdio.h> 
#include <mpi.h> 
#include <limits.h> 
  
void shellSort(int *array, int size) { 
    int temp; 
    for (int gap = size / 2; gap > 0; gap /= 2) { 
        for (int i = 0; i < gap; ++i) { 
            for (int j = i + gap; j < size; j += gap) { 
                temp = array[j]; 
                int k = j - gap; 
                while (k >= i && array[k] > temp) { 
                    array[k + gap] = array[k]; 
                    k -= gap; 
                } 
                array[k + gap] = temp; 
            } 
        } 
    } 
} 
  
void mergeSortedBlocks(const int *input, int *output, int blockSize, int 
totalSize, int numProcesses) { 
    int *indices = (int *) malloc(numProcesses * sizeof(int)); 
  
    for (int i = 0; i < numProcesses; ++i) { 
        indices[i] = i * blockSize; 
    } 
  
    for (int i = 0; i < totalSize; ++i) { 
        int minValue = INT_MAX; 
        int minIndex = -1; 
  
        for (int j = 0; j < numProcesses; ++j) { 
            if (indices[j] < (j + 1) * blockSize && input[indices[j]] < minValue) { 
                minValue = input[indices[j]]; 
                minIndex = j; 
            } 
        } 
        output[i] = minValue; 
        indices[minIndex]++; 
    } 
  
    free(indices); 
} 
  
void initializeInputArray(int *input, int size, int rank) { 
    if (rank == 0) { 
        for (int i = 0; i < size; ++i) { 
            input[i] = rand(); 
        } 
    } 
} 
  
void allocateMemory(int **array, int size) { 
    *array = (int *) malloc(size * sizeof(int)); 
    if (*array == NULL) { 
        fprintf(stderr, "Memory allocation failed\n"); 
        MPI_Abort(MPI_COMM_WORLD, 1); 
    } 
} 
  
int main(int argc, char **argv) { 
    MPI_Init(&argc, &argv); 
  
    if (argc != 1) { 
        printf("Wrong usage\n"); 
        MPI_Finalize(); 
        return 0; 
    } 
  
    int randomSeed = 9235475; 
    for (int k = 0; k < 10; k++) { 
        srand(randomSeed + k); 
  
        int size, rank; 
        MPI_Comm_rank(MPI_COMM_WORLD, &rank); 
        MPI_Comm_size(MPI_COMM_WORLD, &size); 
  
        int requestedCount = (int) 10e5; 
        int totalCount = requestedCount + (size - requestedCount % size); 
        int blockSize = totalCount / size; 
  
        int *input = NULL, *block = NULL, *result = NULL; 
  
        if (rank == 0) { 
            allocateMemory(&input, totalCount); 
            allocateMemory(&result, totalCount); 
        } 
  
        allocateMemory(&block, blockSize); 
  
        if (rank == 0) { 
            initializeInputArray(input, totalCount, rank); 
        } 
  
        MPI_Scatter(input, blockSize, MPI_INT, block, blockSize, MPI_INT, 0, 
MPI_COMM_WORLD); 
  
        double startTime = MPI_Wtime(); 
        shellSort(block, blockSize); 
        MPI_Gather(block, blockSize, MPI_INT, input, blockSize, MPI_INT, 0, 
MPI_COMM_WORLD); 
  
        MPI_Barrier(MPI_COMM_WORLD); 
  
        if (rank == 0) { 
            mergeSortedBlocks(input, result, blockSize, totalCount, size); 
        } 
  
        double endTime = MPI_Wtime(); 
  
        if (rank == 0) { 
            free(input); 
            free(result); 
        } 
  
        free(block); 
  
        if (rank == 0) { 
            FILE *logFile = fopen("log.txt", "a"); 
            fprintf(logFile, "%f\n", endTime - startTime); 
            fclose(logFile); 
        } 
    } 
  
    MPI_Finalize(); 
    return 0; 
} 
