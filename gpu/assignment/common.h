#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "types.h"

#define INIT_MODE_ZERO 1
#define INIT_MODE_RANDOM 2
#define INIT_MODE_SEQUENTIAL 3

void print_matrix(mat_t* m, int nrows, int ncols);
void initialize_matrix(mat_t* m, int nrows, int ncols, int init_mode, mat_t e);
void matmul(mat_t* A, int anx, int any, mat_t* B, int bnx, int bny, mat_t* C);
void save_matrix(mat_t* m, int nrows, int ncols, char* filename);
void print_bin(uint64_t v, int b);