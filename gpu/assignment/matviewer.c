#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "types.h"
#include "common.h"

int main(int argc, char** argv)
{
  if (argc < 2) {
    printf("Usage: %s <filename>\n", argv[0]);
    return 1;
  }

  FILE* fp;
  size_t fsize, ret;
  int pos;
  unsigned char* buf;

  fp = fopen(argv[1], "rb");
  if (fp == NULL) {
    printf("Error opening file %s. Aborting.\n", argv[1]);
    return 1;
  }
  
  fseek(fp, 0, SEEK_END);
  fsize = ftell(fp);
  buf = (unsigned char*) malloc(fsize * sizeof(unsigned char));
  fseek(fp, 0, SEEK_SET);
  
  ret = fread(buf, 1, fsize, fp);
  if (ret != fsize) {
    printf("Error reading file %s. Aborting.\n", argv[1]);
    return 1;
  }
  fclose(fp);

  if (buf[0] != 'M' || buf[1] != 'A' || buf[2] != 'T') {
    printf("No MAT header found: is this a matrix file?\n");
    return 1;
  }
  pos = 3;

  char* read_mat_t_fmt;
  int nrows = 0, ncols = 0;
  size_t read_mat_t_size, read_mat_t_fmt_size;
  for (size_t i = sizeof(int); i > 0; i--) {
    nrows |= (buf[pos++] << (8 * (i - 1))) & 0xFF;
  }
  for (size_t i = sizeof(int); i > 0; i--) {
    ncols |= (buf[pos++] << (8 * (i - 1))) & 0xFF;
  }
  read_mat_t_size = buf[pos++];
  read_mat_t_fmt_size = buf[pos++];
  read_mat_t_fmt = (char*) malloc((read_mat_t_fmt_size + 1) * sizeof(char));
  // memcpy(read_mat_t_fmt, buf + pos, read_mat_t_fmt_size);
  for (size_t i = 0; i < read_mat_t_fmt_size; i++) {
    read_mat_t_fmt[i] = buf[pos++];
  }
  read_mat_t_fmt[read_mat_t_fmt_size] = '\0';

  uint64_t e;
  uint64_t b;
  for (int i = 0; i < nrows; i++) {
    for (int j = 0; j < ncols; j++) {
      e = 0;
      b = 0;
      for (size_t bi = read_mat_t_size; bi > 0; bi--) {
        b = buf[pos++];
        b = b << (8 * (bi - 1));
        e |= b;
      }

      #if mat_t == double
      double v;
      memcpy(&v, &e, 8);
      printf(read_mat_t_fmt, v);
      #elif mat_t == float
      float f;
      memcpy(&f, &e, 4);
      printf(read_mat_t_fmt, f);
      #else
      printf(read_mat_t_fmt, e);
      #endif
      printf(" ");
    }
    printf("\n");
  }
  free(buf);
  
  return 0;
}