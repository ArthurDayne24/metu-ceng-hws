char slayt35_descr[] = "slayt35";
void slayt35(int n, int *src, int *src2, int *dst) {
    int i, j, k, ns = n*n / 16, nd = n - 15;

    int *dsti = dst;
    for (i = 0; i < ns; i++) {
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

    for (k = 0; k < n; k++) {
        int *srci = src2 + k*n;
        for (i = 0; i < n; i++) {
            int in = i*n;
            int r  = src[in+k];
            int *dsti = dst + in;
            for (j = 0; j < nd; j+=16) {
                int *dstij  = dsti + j;
                int *src2ij = srci + j;
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
            }
        }
    }
}

char slayt39_descr[] = "slayt39";
void slayt39(int n, int *src, int *src2, int *dst) {
    int i, j, k, ns = n*n / 16, nd = n - 15;
    int *dsti;

    dsti = dst;
    for (i = 0; i < ns; i++) {
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

    dsti = dst;
    int *srci = src2;
    for (k = 0; k < n; k++) {
        int in = 0;
        int *dsti = dst;
        for (i = 0; i < n; i++) {
            int r  = src[in+k];
            for (j = 0; j < nd; j+=16) {
                int *dstij  = dsti + j;
                int *src2ij = srci + j;
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

            }
            in += n;
            dsti += n;
        }
        srci += n;
    }
}

char slayt42old_descr[] = "slayt42old";
void slayt42old(int n, int *src, int *src2, int *dst) {
    int i, j, k, ns = n*n / 16, nd = n - 15;
    int *dsti;

    dsti = dst;
    for (i = 0; i < ns; i++) {
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

    dsti = dst;
    int *srci = src2;
    for (k = 0; k < n; k++) {
        int in = 0;
        int *dsti = dst;
        for (i = 0; i < n; i++) {
            int r  = src[in+k];
            int *dstij  = dsti;
            int *src2ij = srci;
            for (j = 0; j < nd; j+=16) {
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

                dstij  += 16;
                src2ij += 16;
            }
            in += n;
            dsti += n;
        }
        srci += n;
    }
}
