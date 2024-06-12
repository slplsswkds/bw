#include <cstdio>
#include <cstdlib>
#include <cmath>
#include "../wavelib/header/wavelib.h"
#include <algorithm>
#include <fstream>
#include <print>
#include <vector>

double absmax(const double *array, const int N) {
    double max = 0.0;
    for (int i = 0; i < N; ++i) {
        if (fabs(array[i]) >= max) {
            max = fabs(array[i]);
        }
    }
    return max;
}

auto load_data() {
    std::vector<double> numbers;
    // "/home/maks/sin_tan_exp.txt" contain 44101 lines
    if (std::ifstream dataFile("/home/maks/sin_tan_exp.txt"); dataFile.is_open()) {
        double number;
        while (dataFile >> number) {
            numbers.push_back(number);
            // std::println("{}", number);
        }
        dataFile.close();
    } else {
        std::println("Unable to open file!");
    }
    return numbers;
}

std::tuple<double, double, double> freq_detail_min_max(int j, int fs) {
    double minF = 0;
    double midF = fs / std::pow(2, j + 1);
    double maxF = fs / std::pow(2, j);
    return std::make_tuple(minF, midF, maxF);
}

void freq(int j, int fs) {
    for (int i = 3; i <= j; i++) {
        double min, mid, max;
        std::tie(min, mid, max) = freq_detail_min_max(i, fs);
        std::println("for J={}: \t{}Hz\t-\t{}Hz\t-\t{}Hz", i, min, mid, max);
    }
}

int main() {
    const int N = 44100;
    const int sampleRate = 44100; // Hz
    const int J = 10;

    freq(J, sampleRate);

    const auto inp = new double[N];
    const auto out = new double[N];
    const auto diff = new double[N];

    auto data = load_data();
    std::copy_n(&data[0], N, inp);

    const auto name = "db4";
    const wave_object obj = wave_init(name); // Initialize the wavelet
    const wt_object wt = wt_init(obj, "dwt", N, J); // Initialize the wavelet transform object
    setDWTExtension(wt, "sym"); // Options are "per" and "sym". Symmetric is the default option
    setWTConv(wt, "direct");
    dwt(wt, inp); // Perform DWT

    idwt(wt, out); // Perform IDWT (if needed)

    // Test Reconstruction
    for (int i = 0; i < wt->siglength; ++i) {
        diff[i] = out[i] - inp[i];
    }
    printf("\nMAX %g\n", absmax(diff, wt->siglength)); // If Reconstruction succeeded then the output should be a small value.

    // wt_summary(wt); // Prints the full summary.

    wave_free(obj);
    wt_free(wt);
    delete[] inp;
    delete[] out;
    delete[] diff;

    return 0;
}
