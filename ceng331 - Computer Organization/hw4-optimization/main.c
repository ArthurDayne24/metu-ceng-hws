#include <stdio.h>
#define BLOCKSIZE 2
//#define 4 4

void slayt(int n, int *src, int *src2, int *dst) {
    int i, j, k, i1, j1, k1;
//    dst = (int*) calloc(n*n*n, sizeof(int));
    for (i = 0; i < n; i+=BLOCKSIZE) {
        for (j = 0; j < n; j+=BLOCKSIZE) {
            for (k = 0; k < n; k+=BLOCKSIZE) {
                for (i1 = i; i1 < i+BLOCKSIZE; i1++) {
                    for (j1 = j; j1 < j+BLOCKSIZE; j1++) {
                        for (k1 = k; k1 < k+BLOCKSIZE; k1++) {
                            dst[i1*n+j1] += src[i1*n+k1] * src2[k1*n+j1];
                        }
                    }
                }
            }
        }
    }
}

void printMat(int mat[], int dim) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%d\t", mat[i*4+j]);
        }
        printf("\n");
    }
}

int main()
{
    int src1[][4] = {
        {3,2,1,-10},
        {0,2,1,21},
        {3,2,4,-1},
        {3,8,1,2}
    };

    int src2[][4] = {
        {1,1,6,-1},
        {0,2,8,21},
        {3,96,4,-1},
        {3,0,14,2}
    };

    int dst[4*4];

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            dst[i*4+j] = 0;
        }
    }

    slayt(4, (int*)src1, (int*)src2, dst);
    printMat(dst, 4);

    return 0;
}
