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
    int iter = 3200;
    size_t sz = 200;

    // Allocate matrices.
    // EM Fields, Relative permeability.
    double* dx = malloc(sz * sizeof(double));
    double* ex = malloc(sz * sizeof(double));
    double* hy = malloc(sz * sizeof(double));
    double* er = malloc(sz * sizeof(double));

    // Erase files initially.
    erase_file("ex.dat");
    erase_file("hy.dat");

    // Initialize EM Fields.
    for (size_t k = 1; k < sz; ++k) {
        dx[k] = 0.0;
        ex[k] = 0.0;
        hy[k] = 0.0;
    }

    // Initialize electric permeability.
    for (size_t k = 0; k < sz/2; ++k) er[k] = 1.0;
    for (size_t k = sz/2; k < sz; ++k) er[k] = 4.0;

    // Iterate
    double spread = 12.0;
    double t0 = 40.0;
    double r1=0.0, r2=0.0, l1=0.0, l2=0.0;
    for (int i = 0; i < iter; ++i) {
        // Update Ex.
        for (size_t k = 1; k < sz; ++k) {
            dx[k] += 0.5 * (hy[k-1] - hy[k]);
        }

        // Contour condition
        ex[0] = r2;
        r2 = r1;
        r1 = ex[1];

        ex[sz-1] = l2;
        l2 = l1;
        l1 = ex[sz-2];

        // Gaussian pulse in center.
        double fi = (double)i;
        double arg = (t0-fi)/spread * (t0-fi)/spread;
        double pulse = exp(-0.5 * arg);
        // dx[5] += pulse;

        // Do instead a wave.
        dx[5] += sin(2.0 * M_PI * fi * 0.01);

        // Update Ex.
        for (size_t k = 1; k < sz; ++k) {
            ex[k] = dx[k] / er[k];
        }

        // Update Hy.
        for (size_t k = 0; k < sz-1; ++k) {
            hy[k] += 0.5 * (ex[k] - ex[k+1]);
        }

        append_file("ex.dat", ex, sz);
        append_file("hy.dat", hy, sz);
    }

    free(ex);
    free(hy);
}
