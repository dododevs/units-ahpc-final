#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <mpi.h>
#include <cblas.h>

#include "common.h"
#include "types.h"

int main(int argc, char** argv)
{
  int anx, any, bnx, bny, cnx, cny, mode;
  mat_t* A;
  mat_t* B;
  mat_t* C;

  if (argc < 7) {
    printf("Usage: %s <A_nx> <A_ny> <B_nx> <B_ny> <--naive|--blas|--cuda> <matrix_file>\n", argv[0]);
    exit(1);
  }

  srand((unsigned int) time(NULL));
  anx = atoi(argv[1]);
  any = atoi(argv[2]);
  bnx = atoi(argv[3]);
  bny = atoi(argv[4]);
  cnx = anx;
  cny = bny;
  if (strcmp("--naive", argv[5]) == 0) {
    mode = MATMUL_MODE_NAIVE;
  } else if (strcmp("--blas", argv[5]) == 0) {
    mode = MATMUL_MODE_BLAS;
  } else if (strcmp("--cuda", argv[5]) == 0) {
    mode = MATMUL_MODE_CUDA;
  } else {
    printf("Usage: %s <A_nx> <A_ny> <B_nx> <B_ny> <--naive|--blas|--cuda> <matrix_file>\n", argv[0]);
    exit(1);
  }

  if (any != bnx) {
    printf("Incompatible matrix sizes for multiplication: %d != %d\n", any, bnx);
    exit(1);
  }

  char* world_size;
  world_size = getenv("OMPI_COMM_WORLD_SIZE");
  if (world_size == NULL) {
    printf("Error: it seems that the program was not run with mpirun. Please run with: mpirun [options] %s\n", argv[0]);
    exit(1);
  }

  int rank, size;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  A = (mat_t*) malloc(anx * any * sizeof(mat_t));
  B = (mat_t*) malloc(bnx * bny * sizeof(mat_t));
  C = (mat_t*) calloc(cnx * cny, sizeof(mat_t));

  if (rank == 0) {
    initialize_matrix(A, anx, any, INIT_MODE_SEQUENTIAL, 0);
    initialize_matrix(B, bnx, bny, INIT_MODE_SEQUENTIAL, 0);
  }

  MPI_Bcast(A, anx * any, mat_mpi_t, 0, MPI_COMM_WORLD);
  MPI_Bcast(B, anx * any, mat_mpi_t, 0, MPI_COMM_WORLD);

  int rpr = cnx / size;
  int lo = cnx % size;
  if (mode == MATMUL_MODE_NAIVE) {
    
    for (int ci = 0; ci < rpr + (rank < lo ? 1 : 0); ci++) {
      for (int cj = 0; cj < cny; cj++) {
        #ifdef DEBUG
        printf("[rank %d] %d\n", rank, (ci + rank * rpr + (lo > 0 ? (rank >= lo ? lo : rank) : 0)));
        #endif
        for (int i = 0; i < any; i++) {
          C[ci * cny + cj] += A[(ci + rank * rpr + (lo > 0 ? (rank >= lo ? lo : rank) : 0)) * any + i] * B[cj + i * bny];
        }
      }
    }
  } else if (mode == MATMUL_MODE_BLAS) {
    // mat_t* BT = malloc(bnx * bny * sizeof(mat_t));
    // for (int i = 0; i < bnx; i++) {
    //   for (int j = 0; j < bny; j++) {
    //     BT[i * bnx + j] = B[j * bnx + i];
    //   }
    // }
    cblas_dgemm(
      CblasRowMajor,
      CblasNoTrans,
      CblasNoTrans,
      rpr + (rank < lo ? 1 : 0),
      bny,
      any,
      1.0,
      A + (rank * rpr + (lo > 0 ? (rank >= lo ? lo : rank) : 0) * any),
      bnx,
      B,
      bny,
      0.0,
      C,
      cny
    );
  } else if (mode == MATMUL_MODE_CUDA) {

  }

  int* recvcounts = (int*) malloc(sizeof(int) * size);
  int* displs = (int*) malloc(sizeof(int) * size);
  for (int i = 0; i < size; i++) {
    recvcounts[i] = (rpr + (i < lo ? 1 : 0)) * cny;
    displs[i] = i > 0 ? recvcounts[i - 1] + displs[i - 1] : 0;
  }
  MPI_Allgatherv(C, (rpr + (rank < lo ? 1 : 0)) * cny, mat_mpi_t, C, recvcounts, displs, mat_mpi_t, MPI_COMM_WORLD);

  if (rank == 0) {
    save_matrix(C, cnx, cny, argv[6]);
    // print_matrix(C, cnx, cny);
  }

  free(A);
  free(B);
  free(C);

  MPI_Finalize();
  return 0;
}