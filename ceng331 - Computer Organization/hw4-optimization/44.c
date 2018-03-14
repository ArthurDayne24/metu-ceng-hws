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
void matmul_optimized(int n, int *arr1, int *arr2, int *dest) {
    int i, j, k, r, nd = n >> 5;
    int *desti = dest, *arr1i = arr1, *arr2i = arr2, *destij, *arr2ij;
    
    for (i = 0; i < n; i++) {
        r  = arr1i[0];
        destij  = desti;
        arr2ij = arr2i;
        for (j = 0; j < nd; j++) {
            destij[0 ] = r * arr2ij[0 ];
            destij[1 ] = r * arr2ij[1 ];
            destij[2 ] = r * arr2ij[2 ];
            destij[3 ] = r * arr2ij[3 ];
            destij[4 ] = r * arr2ij[4 ];
            destij[5 ] = r * arr2ij[5 ];
            destij[6 ] = r * arr2ij[6 ];
            destij[7 ] = r * arr2ij[7 ];
            destij[8 ] = r * arr2ij[8 ];
            destij[9 ] = r * arr2ij[9 ];
            destij[10] = r * arr2ij[10];
            destij[11] = r * arr2ij[11];
            destij[12] = r * arr2ij[12];
            destij[13] = r * arr2ij[13];
            destij[14] = r * arr2ij[14];
            destij[15] = r * arr2ij[15];
            destij[16] = r * arr2ij[16];
            destij[17] = r * arr2ij[17];
            destij[18] = r * arr2ij[18];
            destij[19] = r * arr2ij[19];
            destij[20] = r * arr2ij[20];
            destij[21] = r * arr2ij[21];
            destij[22] = r * arr2ij[22];
            destij[23] = r * arr2ij[23];
            destij[24] = r * arr2ij[24];
            destij[25] = r * arr2ij[25];
            destij[26] = r * arr2ij[26];
            destij[27] = r * arr2ij[27];
            destij[28] = r * arr2ij[28];
            destij[29] = r * arr2ij[29];
            destij[30] = r * arr2ij[30];
            destij[31] = r * arr2ij[31];

            destij += 32;
            arr2ij += 32;
        }
        arr1i += n;
        desti += n;
    }
    arr2i += n;

    for (k = 1; k < n; k++) {
        arr1i = arr1;
        desti = dest;
        for (i = 0; i < n; i++) {
            r  = arr1i[k];
            destij  = desti;
            arr2ij = arr2i;
            for (j = 0; j < nd; j++) {
                destij[0 ] += r * arr2ij[0 ];
                destij[1 ] += r * arr2ij[1 ];
                destij[2 ] += r * arr2ij[2 ];
                destij[3 ] += r * arr2ij[3 ];
                destij[4 ] += r * arr2ij[4 ];
                destij[5 ] += r * arr2ij[5 ];
                destij[6 ] += r * arr2ij[6 ];
                destij[7 ] += r * arr2ij[7 ];
                destij[8 ] += r * arr2ij[8 ];
                destij[9 ] += r * arr2ij[9 ];
                destij[10] += r * arr2ij[10];
                destij[11] += r * arr2ij[11];
                destij[12] += r * arr2ij[12];
                destij[13] += r * arr2ij[13];
                destij[14] += r * arr2ij[14];
                destij[15] += r * arr2ij[15];
                destij[16] += r * arr2ij[16];
                destij[17] += r * arr2ij[17];
                destij[18] += r * arr2ij[18];
                destij[19] += r * arr2ij[19];
                destij[20] += r * arr2ij[20];
                destij[21] += r * arr2ij[21];
                destij[22] += r * arr2ij[22];
                destij[23] += r * arr2ij[23];
                destij[24] += r * arr2ij[24];
                destij[25] += r * arr2ij[25];
                destij[26] += r * arr2ij[26];
                destij[27] += r * arr2ij[27];
                destij[28] += r * arr2ij[28];
                destij[29] += r * arr2ij[29];
                destij[30] += r * arr2ij[30];
                destij[31] += r * arr2ij[31];

                destij  += 32;
                arr2ij += 32;
            }
            arr1i += n;
            desti += n;
        }
        arr2i += n;
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

