
// From
// https://repository.prace-ri.eu/git/CodeVault/hpc-kernels/dense_linear_algebra/-/blob/master/gemm/gemm_openmp/src/gemm_openmp.cpp
// Apache v2 license

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <omp.h>

static void fill_random(float *A, const int n, const int m) {

  time_t t;
  // Assign seed from current time integer
  srand((unsigned int)time(&t));

  int i, j;
  for (i = 0; i < n; ++i) {
    for (j = 0; j < m; ++j) {
      A[i * m + j] = (float)rand();
    }
  }
}

static void gemm(float *A, float *B, float *C, const int A_rows,
                 const int A_cols, const int B_rows) {
  int i, k, j;
#pragma omp parallel for shared(A, B, C, A_rows, A_cols, B_rows) private(i, k, \
                                                                         j)
  for (i = 0; i < A_rows; i++) {
    for (k = 0; k < A_cols; k++) {
      for (j = 0; j < B_rows; j++) {
        C[i * B_rows + j] += A[i * A_cols + k] * B[k * B_rows + j];
      }
    }
  }
}

int main(int argc, char *argv[]) {

  int A_rows, A_cols, B_rows, B_cols;

  if (argc != 4) {
    printf(
        "Usage: 3 arguments: matrix A rows, matrix A cols and matrix B cols\n");
    return 1;
  } else {
    A_rows = atoi(argv[1]);
    A_cols = atoi(argv[2]);
    B_rows = atoi(argv[2]);
    B_cols = atoi(argv[3]);
  }

  float *A = (float *)malloc(A_rows * A_cols * sizeof(float));
  float *B = (float *)malloc(B_rows * B_cols * sizeof(float));
  // value-init to zero
  float *C = (float *)calloc(A_rows * B_cols, sizeof(float));

  fill_random(A, A_rows, A_cols);
  fill_random(B, B_rows, B_cols);

  struct timespec start, end;
  clock_gettime(CLOCK_MONOTONIC_RAW, &start);
  gemm(A, B, C, A_rows, A_cols, B_cols);
  clock_gettime(CLOCK_MONOTONIC_RAW, &end);

  float dtime = ((float)((end.tv_sec - start.tv_sec) * 1000000 +
                         (end.tv_nsec - start.tv_nsec) / 1000)) /
                1E6;

  const double total_memory_GB = (A_rows * A_cols) * (B_rows * B_cols) *
                                 (A_rows * B_cols) * sizeof(float) / 1.E9;

  printf("ordinary gemm time: %f s for %d %d rows columns. Memory = %f GB\n",
         dtime, A_rows, B_cols, total_memory_GB);

  free(A);
  free(B);
  free(C);
  return 0;
}