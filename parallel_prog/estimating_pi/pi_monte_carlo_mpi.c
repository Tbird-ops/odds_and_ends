//
// Created by tristan on 11/24/24.
//
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <mpi.h>

/**
 * @brief Small struct to hold key data relating to monte carlo estimation
 */
typedef struct {
    long plots_in_circle; /**< Number of generated plots within radius 1. */
    long total_plots; /**< Total number of iterations performed. */
    double pi; /**< Estimated pi value. 4*plots_in_circle/total_plots. */
} Pi_monte_carlo;

/**
 * @brief Monte Carlo Pi Estimation sim function
 * @param data Pi_monte_carlo struct pointer
 * @param iterations Number of iterations provided by user
 * @param rank Rank of process that called this function
 */
void monte_carlo(Pi_monte_carlo* data, long iterations, int rank) {
    long local_in_circle = 0;
    long local_total = 0;
    long global_in_circle = 0;
    long global_total = 0;
    for (long i = 0; i < iterations; i++) {
        // Generate a coordinate pair between 0 and 1
        double x = (double)((rand() % 10000)+1) / 10000;
        double y = (double)((rand() % 10000)+1) / 10000;

        // calculate distance from origin
        // using principles from the pythagorean theorem
        double origin_distance = x * x + y * y;

        // See if point plotted is inside of circle radius
        if (origin_distance <= 1) {
            local_in_circle++;
        }

        // Increase the count for total number of plots
        local_total++;
    }

    // Have all process reduce locals to a global values in root process
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Reduce(&local_in_circle, &global_in_circle, 1, MPI_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Reduce(&local_total, &global_total, 1, MPI_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    // Have root process put together collective computations in memory
    if(rank == 0) {
        if(data == NULL) {
            printf("No data provided to call monte_carlo_mpi\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        else {
            data->plots_in_circle = global_in_circle;
            data->total_plots = global_total;
            data->pi = (double) (4*global_in_circle) / (double) global_total;
        }
    }
}

/**
 * @brief Setup simulation, time simulation run, and print results
 * @param argc Number of commandline arguments
 * @param argv Pointer to commandline arguments
 * @return Success value
 */
int main(int argc, char *argv[]) {
    // Verify user provided iteration count
    if (argc != 2) {
        printf("Usage: %s <num_iterations>\n", argv[0]);
        exit(1);
    }

    // Initialize MPI environment
    MPI_Init(&argc, &argv);
    // Get world size
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    // Warn about lacking parallelism at worldsize < 2
    if(world_size < 2) {
        printf("World size should be greater than 2 for parallel computation\n");
    }
    // Get process rank
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // ALL MPI PROCESSES HAVE BEEN ESTABLISHED

    // Seed randomness
    srand(time(NULL));

    // Verify that iteration count is a positive number
    long num_iterations = atol(argv[1]);
    if (num_iterations <= 0) {
        printf("Number of iterations must be greater than 0\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    // Balance computations across distributed hosts.
    int r = num_iterations % world_size;
    long block_size;
    if(world_rank < r) {
        block_size = ceil(num_iterations / (double) world_size);
    }
    else {
        block_size = floor(num_iterations/ (double) world_size);
    }

    // Quick balancing printout
    printf("Rank: %d, Blocksize: %ld\n", world_rank, block_size);

    if (world_rank == 0) {
        // Initialize data struct in root node
        Pi_monte_carlo* estimator = (Pi_monte_carlo*) malloc(sizeof(Pi_monte_carlo));

        // Time simulation from root perspective for num_iterations provided
        printf("Starting Monte Carlo Estimation Simulation with %ld iterations\n", num_iterations);
        double start = omp_get_wtime();
        monte_carlo(estimator, block_size, world_rank);
        double end = omp_get_wtime();
        double elapsed = end - start;

        // Print results
        printf("Plots in circle: %ld\n", estimator->plots_in_circle);
        printf("Total plots: %ld\n", estimator->total_plots);
        printf("Pi Estimate: %f\n", estimator->pi);
        printf("Elapsed time: %.3f\n", elapsed);

        free(estimator);
    }
    else {
        monte_carlo(NULL, block_size, world_rank);
    }
    MPI_Finalize();
    return 0;
}
