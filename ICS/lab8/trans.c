/*
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */
// name: Zhu Han
// ID: 522031910213
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);
void transpose_32(int M, int N, int A[N][M], int B[M][N]);
void transpose_64(int M, int N, int A[N][M], int B[M][N]);
void transpose_61(int M, int N, int A[N][M], int B[M][N]);

/*
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded.
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    if (M == 32 && N == 32)
    {
        transpose_32(M, N, A, B);
    }
    else if (M == 64 && N == 64)
    {
        transpose_64(M, N, A, B);
    }
    else if (M == 61 && N == 67)
    {
        transpose_61(M, N, A, B);
    }
    else // for other cases,we can use a 8*8 block to transpose the matrix
    {
        int i, j, k, l;
        for (i = 0; i < N; i += 8)
        {
            for (j = 0; j < M; j += 8)
            {
                for (k = i; k < i + 8 && k < N; k++)
                {
                    for (l = j; l < j + 8 && l < M; l++)
                    {
                        B[l][k] = A[k][l];
                    }
                }
            }
        }
    }
}

/*
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started.
 */
/*
use 8*8 block to transpose the matrix
if the block is on the diagonal, we need to use a temporary variable to store the value so
that we can avoid the conflict miss
*/
void transpose_32(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, k;
    int a0, a1, a2, a3, a4, a5, a6, a7;
    for (i = 0; i < N; i += 8)
    {
        for (j = 0; j < M; j += 8)
        {
            for (k = i; k < i + 8; k++)
            {
                a0 = A[k][j];
                a1 = A[k][j + 1];
                a2 = A[k][j + 2];
                a3 = A[k][j + 3];
                a4 = A[k][j + 4];
                a5 = A[k][j + 5];
                a6 = A[k][j + 6];
                a7 = A[k][j + 7];
                B[j][k] = a0;
                B[j + 1][k] = a1;
                B[j + 2][k] = a2;
                B[j + 3][k] = a3;
                B[j + 4][k] = a4;
                B[j + 5][k] = a5;
                B[j + 6][k] = a6;
                B[j + 7][k] = a7;
            }
        }
    }
}
/*
a row of matrix now contains 16 sets,so every 4 columns the cache will be full
so we need to transpose the matrix by 4*4 blocks
but a 4*4 block will cause no-need conflict miss since a cache line can actually store 4 integers which is 8 columns
so we need to use a 8*8 block to avoid the conflict miss,but we need to use a temporary variable to store the value
we can divide the 8*8 block into 4 4*4 blocks,so we can fully use the cache
*/
void transpose_64(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, k;
    int a0, a1, a2, a3, a4, a5, a6, a7;
    for (i = 0; i < N; i += 8)
    {
        for (j = 0; j < M; j += 8)
        {
            for (k = i; k < i + 4; k++)
            {
                // get the left-up and right-up 4*4 blocks of A
                a0 = A[k][j];
                a1 = A[k][j + 1];
                a2 = A[k][j + 2];
                a3 = A[k][j + 3];
                a4 = A[k][j + 4];
                a5 = A[k][j + 5];
                a6 = A[k][j + 6];
                a7 = A[k][j + 7];
                // store the left-up and right-up 4*4 blocks to B
                B[j][k] = a0;
                B[j + 1][k] = a1;
                B[j + 2][k] = a2;
                B[j + 3][k] = a3;
                B[j][k + 4] = a4;
                B[j + 1][k + 4] = a5;
                B[j + 2][k + 4] = a6;
                B[j + 3][k + 4] = a7;
            }
            for (k = j; k < j + 4; k++)
            {
                // get the left-down block of A
                a0 = A[i + 4][k];
                a1 = A[i + 5][k];
                a2 = A[i + 6][k];
                a3 = A[i + 7][k];
                // get the right-up block of B,which need to be stored to the left-down block of B
                a4 = B[k][i + 4];
                a5 = B[k][i + 5];
                a6 = B[k][i + 6];
                a7 = B[k][i + 7];
                // store the left-down block of A to the right-up block of B
                B[k][i + 4] = a0;
                B[k][i + 5] = a1;
                B[k][i + 6] = a2;
                B[k][i + 7] = a3;
                // store the right-up block of B to the left-down block of B
                B[k + 4][i] = a4;
                B[k + 4][i + 1] = a5;
                B[k + 4][i + 2] = a6;
                B[k + 4][i + 3] = a7;
            }
            for (k = i + 4; k < i + 8; k++)
            {
                // get the right-down block of A and store it to the right-up block of B
                a0 = A[k][j + 4];
                a1 = A[k][j + 5];
                a2 = A[k][j + 6];
                a3 = A[k][j + 7];
                B[j + 4][k] = a0;
                B[j + 5][k] = a1;
                B[j + 6][k] = a2;
                B[j + 7][k] = a3;
            }
        }
    }
}
/*
for a 61*67 matrix,we can divide it into 16*16 blocks
since 61 and 67 is not a multiple of cache line size, the conflict miss is less frequent
*/
void transpose_61(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, k, l;
    for (i = 0; i < N; i += 16)
    {
        for (j = 0; j < M; j += 16)
        {
            for (k = i; k < i + 16 && k < N; k++)
            {
                for (l = j; l < j + 16 && l < M; l++)
                {
                    B[l][k] = A[k][l];
                }
            }
        }
    }
}
/*
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++)
    {
        for (j = 0; j < M; j++)
        {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }
}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc);

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc);
}

/*
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++)
    {
        for (j = 0; j < M; ++j)
        {
            if (A[i][j] != B[j][i])
            {
                return 0;
            }
        }
    }
    return 1;
}
