#include <string.h>
#include <mpi.h>

#define mat_t double
#define mat_t_size sizeof(mat_t)
#define mat_mpi_t MPI_DOUBLE
#define mat_t_fmt "%.2f"
#define mat_t_fmt_size strlen(mat_t_fmt)

#define MATMUL_MODE_NAIVE 0
#define MATMUL_MODE_BLAS 1
#define MATMUL_MODE_CUDA 2