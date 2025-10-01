//
// Created by tristan on 11/18/24.
//
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

/**
 * @brief Small struct to hold key data relating to monte carlo estimation
 */
typedef struct {
    long plots_in_circle; /**< Number of generated plots within radius 1. */
    long total_plots; /**< Total number of iterations performed. */
    double pi; /**< Estimated pi value. 4*plots_in_circle/total_plots. */
} Pi_monte_carlo;

/**
 * @brief OpenMP Parallelized Monte Carlo Pi Estimation sim function
 * @param data Pi_monte_carlo struct pointer
 * @param iterations Number of iterations provided by user
 * @return While a void function, returns values in Pi_monte_carlo struct
 */
void monte_carlo(Pi_monte_carlo* data, long iterations) {
    unsigned int seed = time(NULL);
    long local_in_circle = 0;
    long local_total = 0;

    #pragma omp parallel for reduction(+:local_in_circle,local_total)
    for (long i = 0; i < iterations; i++) {
        // Generate a coordinate pair between 0 and 1
        double x = (double)((rand_r(&seed) % 10000)+1) / 10000;
        double y = (double)((rand_r(&seed) % 10000)+1) / 10000;

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

    data->plots_in_circle = local_in_circle;
    data->total_plots = local_total;
    data->pi = (double)(4 * local_in_circle) / (double) local_total;
}

/**
 * @brief Setup simulation, time simulation run, and print results
 * @param argc Number of commandline arguments
 * @param argv Pointer to commandline arguments
 * @return Success value
 */
int main(int argc, char *argv[]) {
    srand(time(NULL));

    // Verify user provided iteration count
    if (argc != 2) {
        printf("Usage: %s <num_iterations>\n", argv[0]);
        exit(1);
    }

    // Verify that iteration count is a positive number
    long num_iterations = atol(argv[1]);
    if (num_iterations <= 0) {
        printf("Number of iterations must be greater than 0\n");
        exit(1);
    }

    // Initialize data struct
    Pi_monte_carlo* estimator = (Pi_monte_carlo*) malloc(sizeof(Pi_monte_carlo));

    // Run simulation sequentially for num_iterations provided
    printf("Starting Monte Carlo Estimation Simulation with %ld iterations\n", num_iterations);
    double start = omp_get_wtime();
    monte_carlo(estimator, num_iterations);
    double end = omp_get_wtime();
    double elapsed = end - start;

    // Print results
    printf("Plots in circle: %ld\n", estimator->plots_in_circle);
    printf("Total plots: %ld\n", estimator->total_plots);
    printf("Pi Estimate: %f\n", estimator->pi);
    printf("Elapsed time: %.3f\n", elapsed);

    free(estimator);
    return 0;
}