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
    int iter = 1200;
    size_t sq = 100;

    // Allocate 2D grid for transverse magnetic (TM) mode.
    size_t sz = sq * sq;
    double* ez = malloc(sz * sizeof(double));
    double* hx = malloc(sz * sizeof(double));
    double* hy = malloc(sz * sizeof(double));

    // Erase files initially.
    erase_file("hx.dat");
    erase_file("hy.dat");
    erase_file("ez.dat");

    // Initialize.
    for (size_t k = 1; k < sz; ++k) {
        hx[k] = 0.0;
        hy[k] = 0.0;
        ez[k] = 0.0;
    }

    // Iterate.
    double spread = 12.0;
    double t0 = 40.0;
    for (int t = 0; t < iter; ++t) {
        // Update Ex
        for (size_t k = sq; k < sz; ++k) {
            ez[k] += 0.5 * (hy[k] - hy[k-sq] - hx[k] + hx[k-1]);
        }

        // Gaussian pulse in center.
        size_t center = (sq/2) * sq + (sq/2);
        double ft = (double)t;
        double arg = (t0-ft)/spread * (t0-ft)/spread;
        double pulse = exp(-0.5 * arg);
        ez[center] += pulse;

        // Update Hx, Hy.
        for (size_t k = 0; k < sz-sq; ++k) {
            hx[k] += 0.5 * (ez[k] - ez[k+1]);
            hy[k] += 0.5 * (ez[k+sq] - ez[k]);
        }

        // Append data.
        append_file("ez.dat", ez, sz);
        append_file("hx.dat", hx, sz);
        append_file("hy.dat", hy, sz);
    }

    free(hx);
    free(hy);
    free(ez);
}
