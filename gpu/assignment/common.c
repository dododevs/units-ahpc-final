#include "common.h"

void print_matrix(mat_t* m, int nrows, int ncols)
{
  for (int i = 0; i < nrows; i++) {
    for (int j = 0; j < ncols; j++) {
      if (m[i * ncols + j] >= 0) {
        printf(" ");
      }
      printf(mat_t_fmt, m[i * ncols + j]);
      printf(" ");
    }
    printf("\n");
  }
}

void print_bin(uint64_t v, int b) {
  int i;
  int bits = sizeof(v) * b;
  for (i = bits - 1; i >= 0; i--) {
      printf("%d", (int) ((v >> i) & 1));
  }
  printf("\n");
}

void initialize_matrix(mat_t* m, int nrows, int ncols, int init_mode, mat_t e)
{
  mat_t elem = e;
  for (int i = 0; i < nrows; i++) {
    for (int j = 0; j < ncols; j++) {
      if (init_mode == INIT_MODE_ZERO) {
        elem = 0;
      } else if (init_mode == INIT_MODE_RANDOM) {
        elem = rand() % (int) e;
      } else {
        if (i + j != 0) {
          elem++;
        }
      }
      m[i * ncols + j] = elem;
    }
  }
}

void save_matrix(mat_t* m, int nrows, int ncols, char* filename)
{
  FILE* fp;
  fp = fopen(filename, "wb");
  if (fp == NULL) {
    printf("Error opening file %s. Aborting.\n", filename);
    exit(1);
  }
  
  fprintf(fp, "%s", "MAT");

  int b;
  for (size_t i = sizeof(int); i > 0; i--) {
    b = (nrows >> (8 * (i - 1))) & 0xFF;
    fprintf(fp, "%c", (unsigned char) b);
  }
  for (size_t i = sizeof(int); i > 0; i--) {
    b = (ncols >> (8 * (i - 1))) & 0xFF;
    fprintf(fp, "%c", (unsigned char) b);
  }
  fprintf(fp, "%c", (unsigned char) mat_t_size);
  fprintf(fp, "%c", (unsigned char) mat_t_fmt_size);
  fprintf(fp, "%s", mat_t_fmt);

  uint64_t v;
  for (int i = 0; i < nrows; i++) {
    for (int j = 0; j < ncols; j++) {
      memcpy(&v, m + (i * ncols + j), mat_t_size);
      for (size_t bi = mat_t_size; bi > 0; bi--) {
        fprintf(fp, "%c", (unsigned char) (v >> (8 * (bi - 1)) & 0xFF));
      }
    }
  }
  fclose(fp);
}

void matmul(mat_t* A, int anx, int any, mat_t* B, int bnx, int bny, mat_t* C)
{
  #ifdef DEBUG
  char* fmt_str;
  #endif
  int cnx, cny;
  cnx = anx;
  cny = bny;

  if (any != bnx) {
    return;
  }

  #ifdef DEBUG
  fmt_str = (char*) malloc(39 * sizeof(char));
  sprintf(fmt_str, "C[%%d] += A[%%d] * B[%%d] = %s * %s * %s\n", mat_t_fmt, mat_t_fmt, mat_t_fmt);
  #endif

  for (int ci = 0; ci < cnx; ci++) {
    for (int cj = 0; cj < cny; cj++) {
      for (int i = 0; i < any; i++) {
        #ifdef DEBUG
        printf(
          fmt_str,
          ci * cnx + cj,
          ci * any + i,
          cj + (i * bny),
          A[ci * any + i],
          B[cj + (i * bny)],
          A[ci * any + i] * B[cj + (i * bny)]
        );
        #endif
        C[ci * cny + cj] += A[ci * any + i] * B[cj + i * bny];
      }
    }
  }
}