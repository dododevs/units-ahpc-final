
CC = mpicc
CFLAGS = -Wall -Wextra -fopenmp -O3 -IOpenBLAS/
ASM_CFLAGS = -Wall -Wextra -fopenmp -O3 -S -fverbose-asm
LDFLAGS = -lm
EXEC = matmul
SRCS = matmul.c common.c OpenBLAS/libopenblas.a
VIEWER_SRCS = matviewer.c common.c
OBJS = $(SRCS:.c=.o)
MPIRUN_FLAGS = --mca mca_base_component_show_load_errors 0 --map-by core --use-hwthread-cpus

all:
	$(CC) $(CFLAGS) -o $(EXEC) $(SRCS) $(LDFLAGS)

asm:
	$(CC) $(ASM_CFLAGS) $(SRCS) $(LDFLAGS)

viewer:
	$(CC) $(CFLAGS) -o viewer $(VIEWER_SRCS) $(LDFLAGS)

clean:
	rm -f $(EXEC) $(OBJS)

run:
	mpirun $(MPIRUN_FLAGS) -np $(np) $(EXEC) $(ARGS)
