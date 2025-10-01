%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%   MATRIX MULTIPLICATION  %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%% How to compile %%%
As this program is intended to use openMPI to do parallelism,
compiling should be performed with the "mpicc" wrapper to gcc.

$ mpicc -O -o matrix matrix.c

%%% Steps to run %%%
To run this program, use the "mpirun" command to initialize the binary with
a number of processes (# = number of workers + 1 for manager node).
When working with distributed system machine files, change <machinefile> to
testing environment machinefile path.
Additionally, the binary requires 2 commandline arguments. Start the binary with:

$ mpirun -np <#> -machinefile <machinefile> ./matrix <row> <col>

EX: 4 worker processes
Run the binary "matrix" with 4 worker processes and 1 root manager for 1000x1000 matrix multiplication.
$ mpirun -np 5 -machinefile machinefile.list ./matrix 1000 1000

%%% Expected Output %%%
The process should print out the following information in this order:
- A hello from each of the active processes, providing rank and processor information
- The final computed vector/matrix depending on settings
- The statistics of what each processor rank completed for the final result
- Time taken to perform total operation

%%% Additional Info %%%
There are several MACROS in this program capable of changing runtime behavior.
This program is capable of MATRIX VECTOR and MATRIX MATRIX multiplication.
#define VECTOR (0/1)
    - when 0: Run in matrix matrix mode
    - when 1: Run in matrix vector mode

#define MAX_CHUNKS  (How many rows to split per job request)
    - DEFAULT = 10 rows at a time
    - Change value to increase/decrease row distribution per job request from worker.
