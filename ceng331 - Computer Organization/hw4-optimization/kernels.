/********************************************************
 * Kernels to be optimized for the CS:APP Performance Lab
 ********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "defs.h"
/* 
 * Please fill in the following student struct 
 */
team_t team = {
    "2099380",                       /* Student ID */
    "Onur Can TIRTIR",               /* full name */
    "onur.tirtir@ceng.metu.edu.tr",  /* email address */
    "",                              /* leave blank */
    ""                               /* leave blank */
};


/*******************
 * CONVOLUTION KERNEL
 *******************/

/***************************************************************
 * Your different versions of the convolution functions  go here
 ***************************************************************/

/* 
 * naive_conv - The naive baseline version of convolution 
 */
char naive_conv_descr[] = "naive_conv: Naive baseline implementation";
void naive_conv(int dim,int *src, int *ker,int *dst) {
    int i,j,k,l;

    for(i = 0; i < dim-8+1; i++)
    for(j = 0; j < dim-8+1; j++) {
        dst[j*dim+i] = 0;
        for(k = 0; k < 8; k++)
        for(l = 0; l < 8; l++) {
            dst[j*dim+i] = dst[j*dim+i] +src[(j+l)*dim+(i+k)]*ker[l*dim+k];
        }
    }
}

/* 
 * convolution - Your current working version of convolution
 * IMPORTANT: This is the version you will be graded on
 */
char convolution_descr[] = "Dot product: Current working version";
void convolution(int dim,int *src, int *ker,int *dst) 
{
    naive_conv(dim,src,ker,dst);
}

/*********************************************************************
 * register_conv_functions - Register all of your different versions
 *     of the convolution functions  with the driver by calling the
 *     add_conv_function() for each test function. When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.  
 *********************************************************************/

void register_conv_functions() {
    add_conv_function(&naive_conv, naive_conv_descr);   
    add_conv_function(&convolution, convolution_descr);   
    /* ... Register additional test functions here */
}

/*********************
 * MATRIX MULTIP KERNEL
 *********************/

/******************************************************
 * Your different versions of the matrix multiplications  go here
 ******************************************************/

/* 
 * naive_matrix_multiplication - The naive baseline version of matrix multiplication 
 */

char naive_matrix_multiplication_descr[] =
    "Naive_matrix_multiplication: Naive baseline implementation";
void naive_matrix_multiplication(int dim,int *src, int *src2,int *dst) {
    int i,j,k;

    for(i = 0; i < dim; i++)
        for(j = 0; j < dim; j++) {
            dst[j*dim+i]=0;
            for(k = 0; k < dim; k++) 
            dst[j*dim+i] = dst[j*dim+i] + src[j*dim+k]*src2[i+k*dim];
        }    
}

char matmul_optimized_descr[] = "matmul_optimized";
void matmul_optimized(int n, int *src, int *src2, int *dst) {
    int i, j, k, r, nd = n >> 5, in = 0, kn = n;
    int *dstij, *src2ij, *src2i = src2;
    
    for (i = 0; i < n; i++) {
        r  = src[in];
        dstij  = dst+in;
        src2ij = src2i;
        for (j = 0; j < nd; j++) {
            dstij[0 ] = r * src2ij[0 ];
            dstij[1 ] = r * src2ij[1 ];
            dstij[2 ] = r * src2ij[2 ];
            dstij[3 ] = r * src2ij[3 ];
            dstij[4 ] = r * src2ij[4 ];
            dstij[5 ] = r * src2ij[5 ];
            dstij[6 ] = r * src2ij[6 ];
            dstij[7 ] = r * src2ij[7 ];
            dstij[8 ] = r * src2ij[8 ];
            dstij[9 ] = r * src2ij[9 ];
            dstij[10] = r * src2ij[10];
            dstij[11] = r * src2ij[11];
            dstij[12] = r * src2ij[12];
            dstij[13] = r * src2ij[13];
            dstij[14] = r * src2ij[14];
            dstij[15] = r * src2ij[15];
            dstij[16] = r * src2ij[16];
            dstij[17] = r * src2ij[17];
            dstij[18] = r * src2ij[18];
            dstij[19] = r * src2ij[19];
            dstij[20] = r * src2ij[20];
            dstij[21] = r * src2ij[21];
            dstij[22] = r * src2ij[22];
            dstij[23] = r * src2ij[23];
            dstij[24] = r * src2ij[24];
            dstij[25] = r * src2ij[25];
            dstij[26] = r * src2ij[26];
            dstij[27] = r * src2ij[27];
            dstij[28] = r * src2ij[28];
            dstij[29] = r * src2ij[29];
            dstij[30] = r * src2ij[30];
            dstij[31] = r * src2ij[31];

            dstij  += 32;
            src2ij += 32;
        }
        in += n;
    }

    for (k = 1; k < n; k++) {
        in = 0;
        src2i = src2 + kn;
        for (i = 0; i < n; i++) {
            r  = src[in+k];
            dstij  = dst+in;
            src2ij = src2i;
            for (j = 0; j < nd; j++) {
                dstij[0 ] += r * src2ij[0 ];
                dstij[1 ] += r * src2ij[1 ];
                dstij[2 ] += r * src2ij[2 ];
                dstij[3 ] += r * src2ij[3 ];
                dstij[4 ] += r * src2ij[4 ];
                dstij[5 ] += r * src2ij[5 ];
                dstij[6 ] += r * src2ij[6 ];
                dstij[7 ] += r * src2ij[7 ];
                dstij[8 ] += r * src2ij[8 ];
                dstij[9 ] += r * src2ij[9 ];
                dstij[10] += r * src2ij[10];
                dstij[11] += r * src2ij[11];
                dstij[12] += r * src2ij[12];
                dstij[13] += r * src2ij[13];
                dstij[14] += r * src2ij[14];
                dstij[15] += r * src2ij[15];
                dstij[16] += r * src2ij[16];
                dstij[17] += r * src2ij[17];
                dstij[18] += r * src2ij[18];
                dstij[19] += r * src2ij[19];
                dstij[20] += r * src2ij[20];
                dstij[21] += r * src2ij[21];
                dstij[22] += r * src2ij[22];
                dstij[23] += r * src2ij[23];
                dstij[24] += r * src2ij[24];
                dstij[25] += r * src2ij[25];
                dstij[26] += r * src2ij[26];
                dstij[27] += r * src2ij[27];
                dstij[28] += r * src2ij[28];
                dstij[29] += r * src2ij[29];
                dstij[30] += r * src2ij[30];
                dstij[31] += r * src2ij[31];

                dstij  += 32;
                src2ij += 32;
            }
            in += n;
        }
        kn += n;
    }
}

/* 
 * matrix_multiplication - Your current working version of matrix_multiplication
 * IMPORTANT: This is the version you will be graded on
 */
char matrix_multiplication_descr[] = "Matrix multiplications: Current working version";
void matrix_multiplication(int dim,int *src, int *src2,int *dst) 
{
    matmul_optimized(dim,src,src2,dst);
}

/*********************************************************************
 * register_matrix_multiplication_functions - Register all of your different versions
 *     of the matrix multiplication  with the driver by calling the
 *     add_matrix_multiplication_function() for each test function. When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.  
 *********************************************************************/

void register_matrix_multiplication_functions() {
    add_matrix_multiplication_function(&naive_matrix_multiplication,
        naive_matrix_multiplication_descr);   
    add_matrix_multiplication_function(&matrix_multiplication,
        matrix_multiplication_descr);   
    /* ... Register additional test functions here */
    add_matrix_multiplication_function(&matmul_optimized, matmul_optimized_descr);   

}

