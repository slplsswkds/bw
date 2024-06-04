#include <cstdio>
#include <cstdlib>
#include <cmath>
#include "../wavelib/header/wavelib.h"

double absmax(const double *array, const int N) {
    double max = 0.0;
    for (int i = 0; i < N; ++i) {
        if (fabs(array[i]) >= max) {
            max = fabs(array[i]);
        }
    }
    return max;
}

auto get_data(const int minDataLen) {
    double temp[1200];
    FILE *ifp = fopen("../wavelib/test/signal.txt", "r");
    int i = 0;
    if (!ifp) {
        printf("Cannot Open File\n");
        exit(100);
    }
    while (!feof(ifp) && i < 1200) {
        fscanf(ifp, "%lf", &temp[i]);
        i++;
    }
    fclose(ifp);

    if (i < minDataLen) {
        printf("Not enough data in file\n");
        exit(101);
    }

    return temp;
}

int main() {
    const int N = 256;
    const int J = 3;


    auto name = "db4";
    const wave_object obj = wave_init(name); // Initialize the wavelet

    const auto inp = static_cast<double *>(malloc(sizeof(double) * N));
    auto *out = static_cast<double *>(malloc(sizeof(double) * N));
    auto *diff = static_cast<double *>(malloc(sizeof(double) * N));

    auto temp = get_data(N);
    for (int i = 0; i < N; ++i) {
        inp[i] = temp[i];
    }

    const wt_object wt = wt_init(obj, "dwt", N, J); // Initialize the wavelet transform object
    setDWTExtension(wt, "sym"); // Options are "per" and "sym". Symmetric is the default option
    setWTConv(wt, "direct");

    dwt(wt, inp); // Perform DWT
    // DWT output can be accessed using wt->output vector. Use wt_summary to find out how to extract appx and detail coefficients

    for (int i = 0; i < wt->outlength; ++i) {
        printf("%g ", wt->output[i]);
    }

    idwt(wt, out); // Perform IDWT (if needed)
    // Test Reconstruction
    for (int i = 0; i < wt->siglength; ++i) {
        diff[i] = out[i] - inp[i];
    }

    printf("\nMAX %g\n", absmax(diff, wt->siglength)); // If Reconstruction succeeded then the output should be a small value.

    wt_summary(wt); // Prints the full summary.
    wave_free(obj);
    wt_free(wt);

    free(inp);
    free(out);
    free(diff);

    return 0;
}
