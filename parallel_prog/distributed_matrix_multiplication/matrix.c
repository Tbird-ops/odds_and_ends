//
// Created by tbird on 10/29/24.
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

// RUNTIME CHANGING MACROS
#define VECTOR 0 // Set whether running vector matrix multiplication (1) or matrix matrix multiplication (0)
#define MAX_CHUNKS 10    // How many rows to send per REQUEST_WORK

// Send and receive tags for better status tracking
#define ROOT 0  // root process
#define REQUEST_WORK 1  // Worker tag
#define NO_MORE 2       // Master tag
// Incremental tags that are set down in functions
// DO NOT SET HERE, this message is for documentation
//#define RETURN_RESULTS <any number 3 or greater>    // Worker tag tracks element distribution
//#define SEND_WORK <any number 3 or greater>     // Master tag to track element distribution

// Map a 2D matrix in row major order to a 1D array
// RETURNS MALLOC MEMORY POINTER
int *generate_matrix_map(int row, int col) {
    int *result = (int *) malloc(sizeof(int) * row * col);
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            result[i * col + j] = 0;
        }
    }
    return result;
}

// Fill the matrix with of row x col size with random values
void fill_matrix(int *matrix, int row, int col) {
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            // Fill using row major ordering
            matrix[i * col + j] = (rand() % 5) + 1;
        }
    }
}

// Print out a matrix given its rows and cols
void print_matrix(int *matrix, int row, int col) {
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            // Print using row major ordering
            printf("%2d ", matrix[i * col + j]);
        }
        printf("\n");
    }
}

// Multiply two matrices (vectors being special cases)
// All provided matrices are 1D mappings of a 2D matrix.
// Row major format used
// RETURNS MALLOC MEMORY POINTER
int *multiply_matrix(int *matrix1, int row1, int col1, int *matrix2, int row2, int col2) {
    // Make a result matrix based on the rows in matrix 1 and the columns in matrix 2
    int *matrix3 = generate_matrix_map(row1, col2);

    // For row in resulting matrix
    for (int i = 0; i < row1; i++) {
        // For column in resulting matrix
        for (int j = 0; j < col2; j++) {
            // Initialize to 0
            matrix3[i * col2 + j] = 0;
            // Fill dot product result from row column combinations
            for (int k = 0; k < col1; k++) {
                // Do multiplication on positions. Both arrays originally in row major format
                matrix3[i * col2 + j] += matrix1[i * col1 + k] * matrix2[j + col2 * k];
            }
        }
    }
    return matrix3;
}

void master_process(int *matrix1, int row, int col, int *matrix2, int row2, int col2, int world_size) {
    int total_matrix_size = row * col; // Store full matrix size
    int work_sent = 0; // Store total work distributed
    int chunk_size = col; // 1 chunk of work = 1 row of data from matrix.
    int chunks = MAX_CHUNKS * chunk_size; // Number of chunks (or rows) to distribute per request
    int *result = generate_matrix_map(row, col2);

    // initial MPI tag for SEND_WORK
    // Tracks position of calculated elements in terms of rebuilding end matrix
    int SEND_WORK = 3;

    // Distribute tasks to other processes
    for (int i = 1; i < world_size; i++) {
        // Send initial work "chunks" of a matrix to a process
        // If there is no work when first starting, fail out.
        if (work_sent == total_matrix_size) {
            MPI_Send(NULL, 0, MPI_INT, i, NO_MORE, MPI_COMM_WORLD);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        // If the chunk causes work to overflow, only send remainder
        else if (work_sent + chunks > total_matrix_size) {
            MPI_Send(&matrix1[work_sent], total_matrix_size - work_sent, MPI_INT, i, SEND_WORK, MPI_COMM_WORLD);
            work_sent = total_matrix_size;
            SEND_WORK++;
        }
        // Otherwise send as normal
        else {
            MPI_Send(&matrix1[work_sent], chunks, MPI_INT, i, SEND_WORK, MPI_COMM_WORLD);
            work_sent += chunks;
            SEND_WORK++;
        }
    }

    // Start request distribution loop
    // While the number of workers finished is less than all workers
    int workers_finished = 0;
    while (workers_finished < world_size - 1) {
        MPI_Status status;

        // wait for messages about work
        // Don't load a local variable, just look at message status
        MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        int worker_id = status.MPI_SOURCE;

        // First condition: If there is more work, distribute another chunk
        if (status.MPI_TAG == REQUEST_WORK) {
            MPI_Recv(NULL, 0, MPI_INT, MPI_ANY_SOURCE, REQUEST_WORK, MPI_COMM_WORLD, &status);
            // If NO_MORE work, tell worker finish up
            if (work_sent == total_matrix_size) {
                MPI_Send(NULL, 0, MPI_INT, worker_id, NO_MORE, MPI_COMM_WORLD);
                workers_finished++;
            }
            // If the next "chunk" is too much, send what is left
            else if (work_sent + chunks > total_matrix_size) {
                MPI_Send(&matrix1[work_sent], total_matrix_size - work_sent, MPI_INT, worker_id, SEND_WORK,
                         MPI_COMM_WORLD);
                work_sent = total_matrix_size;
                SEND_WORK++;
            }
            // Otherwise, send chunk as normal
            else {
                MPI_Send(&matrix1[work_sent], chunks, MPI_INT, worker_id, SEND_WORK, MPI_COMM_WORLD);
                work_sent += chunks;
                SEND_WORK++;
            }
        }
        // Last check: If worker is RETURN_RESULTS, collect them
        // Reconstruction technique:
        // Each chunk of data sent to a worker had a unique tag. Original started at 3.
        // Upon return, the returned data matches the tag of the original sent message
        // Calculate position of array placment by zero indexing the tag (tag - 3),
        // multiplying it by the number max chunks aligns the starting position to a "chunk", and
        // multiplying the running product by the col2 makes sure that the data is aligned with the amount of
        // columns being returned from processing.
        else if (status.MPI_TAG >= 3) {
            int buf_size;
            int tag = status.MPI_TAG;
            int position = (status.MPI_TAG - 3) * MAX_CHUNKS * col2;
            MPI_Get_count(&status, MPI_INT, &buf_size);
            MPI_Recv(&result[position], buf_size, MPI_INT, worker_id, tag, MPI_COMM_WORLD, &status);
        }
    }
    // Print final matrix
    printf("ROOT: Final matrix from workers\n");
    print_matrix(result, row, col2);

    // printf("ROOT: freeing result\n");
    free(result);
}

int worker_process(int *matrix, int row, int col, int rank, int world_size) {
    MPI_Status status;
    int work_given = 0;

    // LOOP PRIMER
    // Receive next work or instruction (First received is initial work load)
    MPI_Probe(ROOT, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

    // Allocate chunk memory storage
    int buf_size;
    MPI_Get_count(&status, MPI_INT, &buf_size);
    int *chunk = malloc(buf_size * sizeof(int)); // Allocate enough room to accept row chunks


    // Start work loop
    while (1) {
        // When no more work, leave
        if (status.MPI_TAG == NO_MORE) {
            break;
        }
        // When tag == SEND_WORK
        if (status.MPI_TAG >= 3) {
            int tag = status.MPI_TAG;
            MPI_Get_count(&status, MPI_INT, &buf_size);
            MPI_Recv(chunk, buf_size, MPI_INT, ROOT, tag, MPI_COMM_WORLD, &status);
            // Add current buffer length to work given
            work_given += buf_size / row;

            // RESULT = product of matrix * vector
            // Chunk Matrix size = MAX_CHUNKS rows * N row size (columns) based off vector provided
            // Vector size = N row * 1 col from parameters
            // Result size = MAX_CHUNKS rows * 1 col
            int *result = multiply_matrix(chunk, MAX_CHUNKS, row, matrix, row, col);
            MPI_Send(result, (buf_size / row) * col, MPI_INT, ROOT, tag, MPI_COMM_WORLD);

            // printf("RANK %d freeing result\n", rank);
            free(result);

            // Restart by notifying root process of need for work
            MPI_Send(NULL, 0, MPI_INT, ROOT, REQUEST_WORK, MPI_COMM_WORLD);
            // Receive next work or instruction
            MPI_Probe(ROOT, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        }
    }
    // Display worker work completed
    // printf("Rank %d Total rows processed: %3d\n", rank, work_given);

    // printf("RANK %d freeing chunk\n", rank);
    free(chunk);

    return work_given;
}

int main(int argc, char *argv[]) {
    // Initialize random with time to fill matrix
    srand(time(NULL));

    // Initialize MPI environment
    MPI_Init(&argc, &argv);
    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    // Fail out if running with less than 2 nodes (root + worker)
    if (world_size < 2) {
        printf("World size must be greater than 2\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    // Get the rank of current process
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    // Get the name of the current processor
    int name_len;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    MPI_Get_processor_name(processor_name, &name_len);

    // MATRIX
    // Initialize size variables on all processes with commandline parameters
    if (argc < 2) {
        printf("Matrix multiplier requires rows and columns to be specified\n");
        printf("Run 'mpirun -np <number_of_procs> %s <row> <col>'\n", argv[0]);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    int row = atoi(argv[1]);
    int col = atoi(argv[2]);

    // VECTOR
    // Produce column vector based off matrix columns (make sure vector multiplication works)
    int row2 = col;
    int col2;
    if (VECTOR == 1) {
        col2 = 1;
    } else {
        col2 = row;
    }

    // Say hello
    printf("Hello world from processor %s, rank %d out of %d processors\n", processor_name, world_rank, world_size);

    // Make split based on root and worker nodes
    if (world_rank == 0) {
        clock_t begin, end;
        double cpu_time_used;

        begin = clock();
        // Generate original matrix with random numbers
        int *matrix = generate_matrix_map(row, col);
        fill_matrix(matrix, row, col);
        // printf("Matrix from ROOT\n");
        // print_matrix(matrix, row, col);

        // Generate original vector
        int *vector = generate_matrix_map(row2, col2);
        fill_matrix(vector, row2, col2);
        // printf("Vector from ROOT:\n");
        // print_matrix(vector, row2, col2);

        // Testing send shared vector to all sub processes.
        MPI_Barrier(MPI_COMM_WORLD);
        MPI_Bcast(vector, row2 * col2, MPI_INT, 0, MPI_COMM_WORLD);

        // Start master tasking.
        master_process(matrix, row, col, vector, row2, col2, world_size);

        // Clean up allocated memory
        // printf("RANK: %d, freeing matrix\n", world_rank);
        free(matrix);
        // printf("RANK: %d, freeing vector\n", world_rank);
        free(vector);

        // End timer and calc seconds
        end = clock();
        cpu_time_used = ((double) (end - begin) / CLOCKS_PER_SEC);

        // FINAL STATS printing
        // Join together before starting exit process
        MPI_Barrier(MPI_COMM_WORLD);
        int tasks_completed[world_size - 1];
        MPI_Status status;
        // To organize printout,
        for (int i = 0; i < world_size - 1; i++) {
            MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            MPI_Recv(&tasks_completed[status.MPI_SOURCE - 1], 1, MPI_INT, status.MPI_SOURCE, status.MPI_TAG,
                     MPI_COMM_WORLD,
                     &status);
        }
        for (int i = 0; i < world_size - 1; i++) {
            printf("Rank %d completed rows %d\n", i + 1, tasks_completed[i]);
        }
        printf("ROOT: Time %.3f\n", cpu_time_used);
    } else {
        // Allocate room for vector or matrix
        int *vector = malloc(row2 * col2 * sizeof(int));
        MPI_Barrier(MPI_COMM_WORLD);
        MPI_Bcast(vector, row2 * col2, MPI_INT, 0, MPI_COMM_WORLD);
        // printf("Process rank %d vector:\n", world_rank);
        // print_matrix(vector, row2, col2);

        int work_completed = worker_process(vector, row2, col2, world_rank, world_size);
        free(vector);

        MPI_Barrier(MPI_COMM_WORLD);
        MPI_Send(&work_completed, 1, MPI_INT, ROOT, 0, MPI_COMM_WORLD);
        // printf("Rank %2d on %s processor: %4d rows finished\n", world_rank, processor_name, work_completed);
    }

    // Get all processes to same point before exiting
    // Workers should be finished and waiting to close
    // Root should be printing out final matrix and then reaching this point
    // Close out of the MPI environment
    // printf("Rank: %d, finalized\n", world_rank);
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();

    return 0;
}
