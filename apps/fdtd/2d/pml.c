#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void erase_file(char* filename) {
    FILE* file;
    file = fopen(filename, "w");
    fclose(file);
}

void append_file(char* filename, double* pointer, size_t sz) {
    FILE* file;
    file = fopen(filename, "a");
    if (file == NULL) {
        printf("Error while opening file");
        exit(1);
    }

    for (size_t i = 0; i < sz; ++i) {
        fprintf(file, "%f ", pointer[i]);
    }

    fprintf(file, "\n");
    fclose(file);
}

int main() {
    // Choose simulation parameters.
    // iter: number of time iterations.
    // sq: the size of the square box. Memory required will be O(sq^2).
    // pml_size: the size of the PML border surrounding the box.
    int iter = 1200;
    size_t sq = 256;
    size_t pml_size = 32;

    // Allocate 2D grid for transverse magnetic (TM) mode.
    size_t sz = sq * sq;
    double* dz = malloc(sz * sizeof(double));
    double* ez = malloc(sz * sizeof(double));
    double* hx = malloc(sz * sizeof(double));
    double* hy = malloc(sz * sizeof(double));

    // Integral-H Fields.
    double* ihx = malloc(sz * sizeof(double));
    double* ihy = malloc(sz * sizeof(double));

    // Auxiliary fields for PML.
    // Notice: They have size sq. (as they're detached between x and y coordinates).
    // The integral field has size sz.
    double* xn = malloc(sq * sizeof(double));
    double* xnh = malloc(sq * sizeof(double));

    // Erase files initially.
    erase_file("hx.dat");
    erase_file("hy.dat");
    erase_file("ez.dat");

    // Initialize PML field parameters.
    for (size_t k = 0; k < sq; ++k) {
        xn[k] = 0.0;
    }

    for (size_t k = 0; k < pml_size; ++k) {
        size_t xnum = pml_size - k;
        double fpml_size = (double)pml_size;

        double fxnum = (double)xnum;
        xn[k] = 0.33 * pow(fxnum / fpml_size, 3.0);
        xn[sq-k-1] = 0.33 * pow(fxnum / fpml_size, 3.0);

        double fxnum_half = fxnum - 0.5;
        xnh[k] = 0.25 * pow(fxnum_half / fpml_size, 3.0);
        xnh[sq-k-1] = 0.25 * pow(fxnum_half / fpml_size, 3.0);
    }

    // Initialize EM fields.
    for (size_t k = 1; k < sz; ++k) {
        hx[k] = 0.0;
        hy[k] = 0.0;
        ez[k] = 0.0;
        dz[k] = 0.0;
        ihx[k] = 0.0;
        ihy[k] = 0.0;
    }

    // Iterate.
    double spread = 12.0;
    double t0 = 40.0;
    for (int t = 0; t < iter; ++t) {
        // Helpful message.
        printf("\rCalculating frame %d/%d...", 1+t, iter);
        fflush(stdout);

        // Update Dz
        for (size_t k = sq; k < sz; ++k) {
            size_t i = k / sq;
            size_t j = k % sq;

            double gi2 = 1.0 / (1.0 + xn[i]);
            double gj2 = 1.0 / (1.0 + xn[j]);

            double gi3 = (1.0 - xn[i]) / (1.0 + xn[i]);
            double gj3 = (1.0 - xn[j]) / (1.0 + xn[j]);

            dz[k] = gi3 * gj3 * dz[k] 
                    + 0.5 * gi2 * gj2 * (hy[k] - hy[k-sq] - hx[k] + hx[k-1]);
        }

        // Gaussian pulse in center.
        size_t center = (sq/2) * sq + (sq/2);
        double ft = (double)t;
        double arg = (t0-ft)/spread * (t0-ft)/spread;
        double pulse = exp(-0.5 * arg);
        dz[center] += pulse;

        // Set Ez field.
        for (size_t k = 0; k < sz; ++k) {
            ez[k] = dz[k];
        }

        // Set Ez as zero at the edges.
        for (size_t i = 0; i < sq; ++i) {
            ez[i] = 0.0;
            ez[sz-i-1] = 0.0;
            ez[i*sq] = 0.0;
            ez[i*sq+1] = 0.0;
        }

        // Update Hx, Hy.
        for (size_t k = 0; k < sz-sq; ++k) {
            size_t i = k / sq;
            size_t j = k % sq;

            double fi1 = xn[i];
            double fj1 = xn[j];

            double fi2 = 1.0 / (1.0 + xnh[i]);
            double fj2 = 1.0 / (1.0 + xnh[j]);

            double fi3 = (1.0 - xnh[i]) / (1.0 + xnh[i]);
            double fj3 = (1.0 - xnh[j]) / (1.0 + xnh[j]);

            double hx_curl_e = ez[k] - ez[k+1];
            double hy_curl_e = ez[k+sq] - ez[k];

            ihx[k] += fi1 * hx_curl_e;
            ihy[k] += fj1 * hy_curl_e;

            hx[k] = fj3 * hx[k] + 0.5 * fj2 * (hx_curl_e + ihx[k]);
            hy[k] = fi3 * hy[k] + 0.5 * fi2 * (hy_curl_e + ihy[k]);
        }

        // Append data.
        append_file("ez.dat", ez, sz);
        append_file("hx.dat", hx, sz);
        append_file("hy.dat", hy, sz);
    }

    free(hx);
    free(hy);
    free(ez);
    printf("\n");
}
