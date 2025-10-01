%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
# Overview
The following source files provided are all implementations of the Monte Carlo Pi Estimation using the methods listed:
- `pi_monte_carlo_sequential.c`		:Sequential
- `pi_monte_carlo_openmp.c` 		:OpenMP
- `pi_monte_carlo_mpi.c` 			:OpenMPI

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
# Compiling
To compile the programs, please make sure to include the required libraries demonstrated in examples below. Requirements per file:
Sequential: openmp
OpenMP: openmp
OpenMPI: openmp, math

## Example compiles
### Sequential (includes openmp for timing)
`$ gcc -fopenmp -o sequential pi_monte_carlo_sequential.c`

### OpenMP
`$ gcc -fopenmp -o openmp pi_monte_carlo_openmp.c`

### MPI (includes openmp for timing, math for load balancing)
`$ mpicc -fopenmp -o mpi pi_monte_carlo_mpi.c -lm`

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
# Running the programs
All programs are standardized to be run with 1 command line argument specifying the number of iterations for the simulation. Provided below are example program executions for a test case of 1 million

## Examples
### Sequential
`$ ./sequential 1000000`

### OpenMP (change export value to desired thread count)
`$ export OMP_NUM_THREADS=4 `
`$ ./openmp 1000000`

### MPI (change -np # to desired processes, provide own machinefile)
`$ mpirun -np 4 -machinefile machinefile.dsu ./mpi 1000000`

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
# Expected Output
All files have a mostly standardized output for simple comparison. The expected output should have at least the following:
A starting message indicating the start of the simulation with number of iterations
The final total of plots in circle from all threads/processes
The final total of all plots (should match the initiation value)
Estimated Pi 
(4 * plots in circle / total plots) for manual verification
Elapsed Time in seconds.

Any additional outputs indicate slight differences in implementation such as process block sizes during the MPI run.

