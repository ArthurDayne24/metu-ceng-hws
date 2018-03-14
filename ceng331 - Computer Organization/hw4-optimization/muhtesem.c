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
char conv4_descr[] = "conv4";
void conv4(int dim, int *src, int *ker,int *dst) {
    int ker0  = ker[0];
    int ker1  = ker[1];
    int ker2  = ker[2];
    int ker3  = ker[3];
    int ker4  = ker[4];
    int ker5  = ker[5];
    int ker6  = ker[6];
    int ker7  = ker[7];

    ker += dim;
    int ker8  = ker[0];
    int ker9  = ker[1];
    int ker10 = ker[2];
    int ker11 = ker[3];
    int ker12 = ker[4];
    int ker13 = ker[5];
    int ker14 = ker[6];
    int ker15 = ker[7];
    
    ker += dim;
    int ker16 = ker[0];
    int ker17 = ker[1];
    int ker18 = ker[2];
    int ker19 = ker[3];
    int ker20 = ker[4];
    int ker21 = ker[5];
    int ker22 = ker[6];
    int ker23 = ker[7];
    
    ker += dim;
    int ker24 = ker[0];
    int ker25 = ker[1];
    int ker26 = ker[2];
    int ker27 = ker[3];
    int ker28 = ker[4];
    int ker29 = ker[5];
    int ker30 = ker[6];
    int ker31 = ker[7];
    
    ker += dim;
    int ker32 = ker[0];
    int ker33 = ker[1];
    int ker34 = ker[2];
    int ker35 = ker[3];
    int ker36 = ker[4];
    int ker37 = ker[5];
    int ker38 = ker[6];
    int ker39 = ker[7];
    
    ker += dim;
    int ker40 = ker[0];
    int ker41 = ker[1];
    int ker42 = ker[2];
    int ker43 = ker[3];
    int ker44 = ker[4];
    int ker45 = ker[5];
    int ker46 = ker[6];
    int ker47 = ker[7];
    
    ker += dim;
    int ker48 = ker[0];
    int ker49 = ker[1];
    int ker50 = ker[2];
    int ker51 = ker[3];
    int ker52 = ker[4];
    int ker53 = ker[5];
    int ker54 = ker[6];
    int ker55 = ker[7];
    
    ker += dim;
    int ker56 = ker[0];
    int ker57 = ker[1];
    int ker58 = ker[2];
    int ker59 = ker[3];
    int ker60 = ker[4];
    int ker61 = ker[5];
    int ker62 = ker[6];
    int ker63 = ker[7];

    int rdim = dim-7;
    for(int j = 0; j < rdim; ++j, dst += dim, src += dim) {
        int acc0, acc1;
        for(int i = 0; i < rdim; dst[i] = (acc0 + acc1), ++i) {
            int *srcji = src+i;
            
            acc0   = srcji[0]*ker0 ;
            acc1   = srcji[1]*ker1 ;
            acc0  += srcji[2]*ker2 ;
            acc1  += srcji[3]*ker3 ;
            acc0  += srcji[4]*ker4 ;
            acc1  += srcji[5]*ker5 ;
            acc0  += srcji[6]*ker6 ;
            acc1  += srcji[7]*ker7 ;
            srcji += dim;
            acc0  += srcji[0]*ker8 ;
            acc1  += srcji[1]*ker9 ;
            acc0  += srcji[2]*ker10;
            acc1  += srcji[3]*ker11;
            acc0  += srcji[4]*ker12;
            acc1  += srcji[5]*ker13;
            acc0  += srcji[6]*ker14;
            acc1  += srcji[7]*ker15;
            srcji += dim;
            acc0  += srcji[0]*ker16;
            acc1  += srcji[1]*ker17;
            acc0  += srcji[2]*ker18;
            acc1  += srcji[3]*ker19;
            acc0  += srcji[4]*ker20;
            acc1  += srcji[5]*ker21;
            acc0  += srcji[6]*ker22;
            acc1  += srcji[7]*ker23;
            srcji += dim;
            acc0  += srcji[0]*ker24;
            acc1  += srcji[1]*ker25;
            acc0  += srcji[2]*ker26;
            acc1  += srcji[3]*ker27;
            acc0  += srcji[4]*ker28;
            acc1  += srcji[5]*ker29;
            acc0  += srcji[6]*ker30;
            acc1  += srcji[7]*ker31;
            srcji += dim;
            acc0  += srcji[0]*ker32;
            acc1  += srcji[1]*ker33;
            acc0  += srcji[2]*ker34;
            acc1  += srcji[3]*ker35;
            acc0  += srcji[4]*ker36;
            acc1  += srcji[5]*ker37;
            acc0  += srcji[6]*ker38;
            acc1  += srcji[7]*ker39;
            srcji += dim;
            acc0  += srcji[0]*ker40;
            acc1  += srcji[1]*ker41;
            acc0  += srcji[2]*ker42;
            acc1  += srcji[3]*ker43;
            acc0  += srcji[4]*ker44;
            acc1  += srcji[5]*ker45;
            acc0  += srcji[6]*ker46;
            acc1  += srcji[7]*ker47;
            srcji += dim;
            acc0  += srcji[0]*ker48;
            acc1  += srcji[1]*ker49;
            acc0  += srcji[2]*ker50;
            acc1  += srcji[3]*ker51;
            acc0  += srcji[4]*ker52;
            acc1  += srcji[5]*ker53;
            acc0  += srcji[6]*ker54;
            acc1  += srcji[7]*ker55;
            srcji += dim;
            acc0  += srcji[0]*ker56;
            acc1  += srcji[1]*ker57;
            acc0  += srcji[2]*ker58;
            acc1  += srcji[3]*ker59;
            acc0  += srcji[4]*ker60;
            acc1  += srcji[5]*ker61;
            acc0  += srcji[6]*ker62;
            acc1  += srcji[7]*ker63;
        }
    }
}
*/
char conv5_descr[] = "conv5";
void conv5(int dim, int *src, int *ker,int *dst) {
    int ker0  = ker[0];
    int ker1  = ker[1];
    int ker2  = ker[2];
    int ker3  = ker[3];
    int ker4  = ker[4];
    int ker5  = ker[5];
    int ker6  = ker[6];
    int ker7  = ker[7];

    ker += dim;
    int ker8  = ker[0];
    int ker9  = ker[1];
    int ker10 = ker[2];
    int ker11 = ker[3];
    int ker12 = ker[4];
    int ker13 = ker[5];
    int ker14 = ker[6];
    int ker15 = ker[7];
    
    ker += dim;
    int ker16 = ker[0];
    int ker17 = ker[1];
    int ker18 = ker[2];
    int ker19 = ker[3];
    int ker20 = ker[4];
    int ker21 = ker[5];
    int ker22 = ker[6];
    int ker23 = ker[7];
    
    ker += dim;
    int ker24 = ker[0];
    int ker25 = ker[1];
    int ker26 = ker[2];
    int ker27 = ker[3];
    int ker28 = ker[4];
    int ker29 = ker[5];
    int ker30 = ker[6];
    int ker31 = ker[7];
    
    ker += dim;
    int ker32 = ker[0];
    int ker33 = ker[1];
    int ker34 = ker[2];
    int ker35 = ker[3];
    int ker36 = ker[4];
    int ker37 = ker[5];
    int ker38 = ker[6];
    int ker39 = ker[7];
    
    ker += dim;
    int ker40 = ker[0];
    int ker41 = ker[1];
    int ker42 = ker[2];
    int ker43 = ker[3];
    int ker44 = ker[4];
    int ker45 = ker[5];
    int ker46 = ker[6];
    int ker47 = ker[7];
    
    ker += dim;
    int ker48 = ker[0];
    int ker49 = ker[1];
    int ker50 = ker[2];
    int ker51 = ker[3];
    int ker52 = ker[4];
    int ker53 = ker[5];
    int ker54 = ker[6];
    int ker55 = ker[7];
    
    ker += dim;
    int ker56 = ker[0];
    int ker57 = ker[1];
    int ker58 = ker[2];
    int ker59 = ker[3];
    int ker60 = ker[4];
    int ker61 = ker[5];
    int ker62 = ker[6];
    int ker63 = ker[7];

    int rdim = dim-7;
    for(int j = rdim; j--; ) {
        int acc0, acc1;
        int *srcj = src, *dsti = dst;
        for(int i = rdim; i--; ) {
            int *srcji = srcj;
            
            acc0   = srcji[0]*ker0 ;
            acc1   = srcji[1]*ker1 ;
            acc0  += srcji[2]*ker2 ;
            acc1  += srcji[3]*ker3 ;
            acc0  += srcji[4]*ker4 ;
            acc1  += srcji[5]*ker5 ;
            acc0  += srcji[6]*ker6 ;
            acc1  += srcji[7]*ker7 ;
            srcji += dim;
            acc0  += srcji[0]*ker8 ;
            acc1  += srcji[1]*ker9 ;
            acc0  += srcji[2]*ker10;
            acc1  += srcji[3]*ker11;
            acc0  += srcji[4]*ker12;
            acc1  += srcji[5]*ker13;
            acc0  += srcji[6]*ker14;
            acc1  += srcji[7]*ker15;
            srcji += dim;
            acc0  += srcji[0]*ker16;
            acc1  += srcji[1]*ker17;
            acc0  += srcji[2]*ker18;
            acc1  += srcji[3]*ker19;
            acc0  += srcji[4]*ker20;
            acc1  += srcji[5]*ker21;
            acc0  += srcji[6]*ker22;
            acc1  += srcji[7]*ker23;
            srcji += dim;
            acc0  += srcji[0]*ker24;
            acc1  += srcji[1]*ker25;
            acc0  += srcji[2]*ker26;
            acc1  += srcji[3]*ker27;
            acc0  += srcji[4]*ker28;
            acc1  += srcji[5]*ker29;
            acc0  += srcji[6]*ker30;
            acc1  += srcji[7]*ker31;
            srcji += dim;
            acc0  += srcji[0]*ker32;
            acc1  += srcji[1]*ker33;
            acc0  += srcji[2]*ker34;
            acc1  += srcji[3]*ker35;
            acc0  += srcji[4]*ker36;
            acc1  += srcji[5]*ker37;
            acc0  += srcji[6]*ker38;
            acc1  += srcji[7]*ker39;
            srcji += dim;
            acc0  += srcji[0]*ker40;
            acc1  += srcji[1]*ker41;
            acc0  += srcji[2]*ker42;
            acc1  += srcji[3]*ker43;
            acc0  += srcji[4]*ker44;
            acc1  += srcji[5]*ker45;
            acc0  += srcji[6]*ker46;
            acc1  += srcji[7]*ker47;
            srcji += dim;
            acc0  += srcji[0]*ker48;
            acc1  += srcji[1]*ker49;
            acc0  += srcji[2]*ker50;
            acc1  += srcji[3]*ker51;
            acc0  += srcji[4]*ker52;
            acc1  += srcji[5]*ker53;
            acc0  += srcji[6]*ker54;
            acc1  += srcji[7]*ker55;
            srcji += dim;
            acc0  += srcji[0]*ker56;
            acc1  += srcji[1]*ker57;
            acc0  += srcji[2]*ker58;
            acc1  += srcji[3]*ker59;
            acc0  += srcji[4]*ker60;
            acc1  += srcji[5]*ker61;
            acc0  += srcji[6]*ker62;
            acc1  += srcji[7]*ker63;

            srcj++;
            *dsti++ = (acc0 + acc1);
        }
        dst += dim;
        src += dim;
    }
}
/* 
 * convolution - Your current working version of convolution
 * IMPORTANT: This is the version you will be graded on
 */
char convolution_descr[] = "Dot product: Current working version";
void convolution(int dim,int *src, int *ker,int *dst) 
{
    conv5(dim, src, ker, dst);
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

char matmul_optimized3_descr[] = "matmul_optimized3";
void matmul_optimized3(int n, int *src1, int *src2, int *dst) {
    if (n == 32) {
        int in = 0;
        for (int i = 32; i-- ; ) {
            int r  = src1[in];
            int *dstij  = dst+in;
            
            dstij[0 ] = r * src2[0 ];
            dstij[1 ] = r * src2[1 ];
            dstij[2 ] = r * src2[2 ];
            dstij[3 ] = r * src2[3 ];
            dstij[4 ] = r * src2[4 ];
            dstij[5 ] = r * src2[5 ];
            dstij[6 ] = r * src2[6 ];
            dstij[7 ] = r * src2[7 ];
            dstij[8 ] = r * src2[8 ];
            dstij[9 ] = r * src2[9 ];
            dstij[10] = r * src2[10];
            dstij[11] = r * src2[11];
            dstij[12] = r * src2[12];
            dstij[13] = r * src2[13];
            dstij[14] = r * src2[14];
            dstij[15] = r * src2[15];
            dstij[16] = r * src2[16];
            dstij[17] = r * src2[17];
            dstij[18] = r * src2[18];
            dstij[19] = r * src2[19];
            dstij[20] = r * src2[20];
            dstij[21] = r * src2[21];
            dstij[22] = r * src2[22];
            dstij[23] = r * src2[23];
            dstij[24] = r * src2[24];
            dstij[25] = r * src2[25];
            dstij[26] = r * src2[26];
            dstij[27] = r * src2[27];
            dstij[28] = r * src2[28];
            dstij[29] = r * src2[29];
            dstij[30] = r * src2[30];
            dstij[31] = r * src2[31];

            in += 32;
        }

        int kn = 32;
        for (int k = 1; k < 32; k++) {
            int in = 0;
            int *src2i = src2 + kn;
            for (int i = 32; i--;) {
                int r  = src1[in+k];
                int *dstij  = dst+in;
                
                dstij[0 ] += r * src2i[0 ];
                dstij[1 ] += r * src2i[1 ];
                dstij[2 ] += r * src2i[2 ];
                dstij[3 ] += r * src2i[3 ];
                dstij[4 ] += r * src2i[4 ];
                dstij[5 ] += r * src2i[5 ];
                dstij[6 ] += r * src2i[6 ];
                dstij[7 ] += r * src2i[7 ];
                dstij[8 ] += r * src2i[8 ];
                dstij[9 ] += r * src2i[9 ];
                dstij[10] += r * src2i[10];
                dstij[11] += r * src2i[11];
                dstij[12] += r * src2i[12];
                dstij[13] += r * src2i[13];
                dstij[14] += r * src2i[14];
                dstij[15] += r * src2i[15];
                dstij[16] += r * src2i[16];
                dstij[17] += r * src2i[17];
                dstij[18] += r * src2i[18];
                dstij[19] += r * src2i[19];
                dstij[20] += r * src2i[20];
                dstij[21] += r * src2i[21];
                dstij[22] += r * src2i[22];
                dstij[23] += r * src2i[23];
                dstij[24] += r * src2i[24];
                dstij[25] += r * src2i[25];
                dstij[26] += r * src2i[26];
                dstij[27] += r * src2i[27];
                dstij[28] += r * src2i[28];
                dstij[29] += r * src2i[29];
                dstij[30] += r * src2i[30];
                dstij[31] += r * src2i[31];

                in += 32;
            }
            kn += 32;
        }
    }
    else {
        int *src2c = malloc(n*n*sizeof(int));
        
        int *src2i   = src2;
        int *src2ci  = src2c;
        int *src2ik  = src2;
        int *src2cik = src2c;
        for (int i = 0; i < n; i++) {
            int *src2ikj  = src2ik;
            int *src2cikj = src2cik;
            for (int j = 0; j < n; j++) {
                int t0 = src2i[j];

                *src2cikj = t0;

                src2ikj  += n;
                src2cikj += n;
            }
            src2i   += n;
            src2ci  += n;
            src2ik++;
            src2cik++;
        }    

        int nd = n >> 5;
        dst += n-1;
        src2c += n*(n-1);
        for (int i = n; i--; ) {
            int *src2cj = src2c;
            int *dstij  = dst;
            for (int j = n; j--; ) {
                int acc0 = 0;
                int *src1ik  = src1;
                int *src2cjk = src2cj;
                for (int k = nd; k--; ) {
                    acc0 += src1ik[0 ]*src2cjk[0 ];
                    acc0 += src1ik[1 ]*src2cjk[1 ];
                    acc0 += src1ik[2 ]*src2cjk[2 ];
                    acc0 += src1ik[3 ]*src2cjk[3 ];
                    acc0 += src1ik[4 ]*src2cjk[4 ];
                    acc0 += src1ik[5 ]*src2cjk[5 ];
                    acc0 += src1ik[6 ]*src2cjk[6 ];
                    acc0 += src1ik[7 ]*src2cjk[7 ];
                    acc0 += src1ik[8 ]*src2cjk[8 ];
                    acc0 += src1ik[9 ]*src2cjk[9 ];
                    acc0 += src1ik[10]*src2cjk[10];
                    acc0 += src1ik[11]*src2cjk[11];
                    acc0 += src1ik[12]*src2cjk[12];
                    acc0 += src1ik[13]*src2cjk[13];
                    acc0 += src1ik[14]*src2cjk[14];
                    acc0 += src1ik[15]*src2cjk[15];
                    acc0 += src1ik[16]*src2cjk[16];
                    acc0 += src1ik[17]*src2cjk[17];
                    acc0 += src1ik[18]*src2cjk[18];
                    acc0 += src1ik[19]*src2cjk[19];
                    acc0 += src1ik[20]*src2cjk[20];
                    acc0 += src1ik[21]*src2cjk[21];
                    acc0 += src1ik[22]*src2cjk[22];
                    acc0 += src1ik[23]*src2cjk[23];
                    acc0 += src1ik[24]*src2cjk[24];
                    acc0 += src1ik[25]*src2cjk[25];
                    acc0 += src1ik[26]*src2cjk[26];
                    acc0 += src1ik[27]*src2cjk[27];
                    acc0 += src1ik[28]*src2cjk[28];
                    acc0 += src1ik[29]*src2cjk[29];
                    acc0 += src1ik[30]*src2cjk[30];
                    acc0 += src1ik[31]*src2cjk[31];

                    src1ik  += 32;
                    src2cjk += 32;
                }
                *dstij-- = acc0;

                src2cj -= n;
            }
            src1 += n;
            dst  += n;
        }
    }
}
/*
char matmul_optimized2_descr[] = "matmul_optimized2";
void matmul_optimized2(int n, int *src, int *src2, int *dst) {
    int i, j, k, r, nd = n >> 5, in = 0, kn = n;
    int *dstij, *src2ij, *src2i = src2;
    
    for (i = n; i-- ; ) {
        r  = src[in];
        dstij  = dst+in;
        src2ij = src2i;
        for (j = nd; j--;) {
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
        for (i = n; i--;) {
            r  = src[in+k];
            dstij  = dst+in;
            src2ij = src2i;
            for (j = nd; j--;) {
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
*/
/*
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
*/

/* 
 * matrix_multiplication - Your current working version of matrix_multiplication
 * IMPORTANT: This is the version you will be graded on
 */
char matrix_multiplication_descr[] = "Matrix multiplications: Current working version";
void matrix_multiplication(int dim,int *src, int *src2,int *dst) 
{
    matmul_optimized3(dim,src,src2,dst);
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
//    add_matrix_multiplication_function(&matmul_optimized2, matmul_optimized2_descr);
}

