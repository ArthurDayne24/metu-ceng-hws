/********************************************************
 * Kernels to be optimized for the CS:APP Performance Lab
 ********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "defs.h"
#define BLOCKSIZE 8
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
char slayt_descr[] = "Slayt xd";
/*
void slayt(int n, int *src, int *src2, int *dst) {
    int i, j, k, i1, j1, k1, r;
//    dst = (int*) calloc(n*n*n, sizeof(int));
    for (i = 0; i < n; i+=BLOCKSIZE) {
        for (j = 0; j < n; j+=BLOCKSIZE) {
            for (k = 0; k < n; k+=BLOCKSIZE) {
                for (k1 = k; k1 < k+BLOCKSIZE; k1++) {
                    for (i1 = i; i1 < i+BLOCKSIZE; i1++) {
                        r = src[i1*n+k1];
                        for (j1 = j; j1 < j+BLOCKSIZE; j1++) {
                            dst[i1*n+j1] += r * src2[k1*n+j1];
                        }
                    }
                }
            }
        }
    }
}
*/

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

