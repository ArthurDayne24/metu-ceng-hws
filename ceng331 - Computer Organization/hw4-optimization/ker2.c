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


/***************
 * CONVOLUTION KERNEL
 ***************/

/******************************************************
 * Your different versions of the convolution functions  go here
 ******************************************************/

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




/***************
 * MATRIX MULTIP KERNEL
 ***************/

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

char slayt_descr[] = "slayt";
void slayt(int n, int *src, int *src2, int *dst) {
    int i, j, k, nd = n / 16, nsd = n*nd;
    int *dsti;

    dsti = dst;
    for (i = 0; i < nsd; i++) {
        dsti[0 ] = 0;
        dsti[1 ] = 0;
        dsti[2 ] = 0;
        dsti[3 ] = 0;
        dsti[4 ] = 0;
        dsti[5 ] = 0;
        dsti[6 ] = 0;
        dsti[7 ] = 0;
        dsti[8 ] = 0;
        dsti[9 ] = 0;
        dsti[10] = 0;
        dsti[11] = 0;
        dsti[12] = 0;
        dsti[13] = 0;
        dsti[14] = 0;
        dsti[15] = 0;
        
        dsti += 16;
    }

    dsti = dst; int *srci = src2;
    for (k = 0; k < n; k++) {
        int in = 0;
        int *dsti = dst;
        for (i = 0; i < n; i++) {
            int r  = src[in+k];
            int *dstij  = dsti;
            int *src2ij = srci;
            for (j = 0; j < nd; j++) {
                int e0  = src2ij[0 ];
                int e1  = src2ij[1 ];
                int e2  = src2ij[2 ];
                int e3  = src2ij[3 ];
                int e4  = src2ij[4 ];
                int e5  = src2ij[5 ];
                int e6  = src2ij[6 ];
                int e7  = src2ij[7 ];
                int e8  = src2ij[8 ];
                int e9  = src2ij[9 ];
                int e10 = src2ij[10];
                int e11 = src2ij[11];
                int e12 = src2ij[12];
                int e13 = src2ij[13];
                int e14 = src2ij[14];
                int e15 = src2ij[15];
                dstij[0 ] += r * e0;
                dstij[1 ] += r * e1;
                dstij[2 ] += r * e2;
                dstij[3 ] += r * e3;
                dstij[4 ] += r * e4;
                dstij[5 ] += r * e5;
                dstij[6 ] += r * e6;
                dstij[7 ] += r * e7;
                dstij[8 ] += r * e8;
                dstij[9 ] += r * e9;
                dstij[10] += r * e10;
                dstij[11] += r * e11;
                dstij[12] += r * e12;
                dstij[13] += r * e13;
                dstij[14] += r * e14;
                dstij[15] += r * e15;

                dstij  += 16;
                src2ij += 16;
            }
            in += n;
            dsti += n;
        }
        srci += n;
    }
}

/* 
 * matrix_multiplication - Your current working version of matrix_multiplication
 * IMPORTANT: This is the version you will be graded on
 */
char matrix_multiplication_descr[] = "Matrix multiplications: Current working version";
void matrix_multiplication(int dim,int *src, int *src2,int *dst) 
{

        naive_matrix_multiplication(dim,src,src2,dst);

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
    add_matrix_multiplication_function(&slayt, slayt_descr);   

}

