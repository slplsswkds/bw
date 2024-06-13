#include <cstdio>
#include <cstdlib>
#include <cmath>
#include "../wavelib/header/wavelib.h"
#include <algorithm>
#include <fstream>
#include <print>
#include <vector>
#include <iostream>

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
    if (std::ifstream dataFile("/home/maks/devel/bw/assets/sin_tan_exp.txt"); dataFile.is_open()) {
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

auto save_data(const double *data, const int size, const char *path) {
    std::ofstream outFile(path);
    // Перевірка, чи файл вдалося відкрити
    if (!outFile.is_open()) {
        std::println("Не вдалося відкрити файл для запису!");
    }

    // Запис кожного числа з масиву у новий рядок у файлі
    for (int i = 0; i < size; ++i) {
        outFile << data[i] << std::endl;
    }

    // Закриття файлу
    outFile.close();
    std::println("Числа були успішно записані у файл ");
}

auto freq_detail_min_max(const int j, const int fs) -> std::tuple<double, double, double> {
    double minF = 0;
    double midF = fs / std::pow(2, j + 1);
    double maxF = fs / std::pow(2, j);
    return std::make_tuple(minF, midF, maxF);
}

void freq(const int j, const int fs) {
    for (int i = 0; i <= j; i++) {
        double min, mid, max;
        std::tie(min, mid, max) = freq_detail_min_max(i, fs);
        std::println("for J={}: \t{}Hz\t-\t{}Hz\t-\t{}Hz", i, min, mid, max);
    }
}

static auto rmse(const int N, const double *x, const double *y) -> double {
    double rms = 0.0;

    for (int i = 0; i < N; ++i) {
        rms += (x[i] - y[i]) * (x[i] - y[i]);
    }

    rms = sqrt(rms / static_cast<double>(N));

    return rms;
}

auto rezeroConcretLevelTest(const wt_object wt, const int J) {
    int offset = 0;
    std::cout << wt->outlength;
    for (int wtOutputVectorElement = 0; wtOutputVectorElement <= wt->MaxIter; wtOutputVectorElement++) {
        const auto decompositionLvlSize = wt->length[wtOutputVectorElement];

        if (wtOutputVectorElement == J) {
            std::cout << "------ LEVEL SIZE = " << decompositionLvlSize << std::endl;
            std::cout << "------ Iteration № " << wtOutputVectorElement << "-------" << std::endl;

            for (int coefPos = 0; coefPos < decompositionLvlSize; coefPos++) {
                const auto coef = wt->output[offset + coefPos];
                wt->output[offset + coefPos] = 0;
            }

            offset += decompositionLvlSize;
        }
    }
}

int main() {
    constexpr int N = 44101;
    constexpr int sampleRate = 44100; // Hz
    constexpr int J = 11;

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
    rezeroConcretLevelTest(wt, J);

    idwt(wt, out); // Perform IDWT (if needed)

    // Test Reconstruction
    for (int i = 0; i < wt->siglength; ++i) {
        diff[i] = out[i] - inp[i];
    }
    printf("\nMAX %g\n", absmax(diff, wt->siglength));
    // If Reconstruction succeeded then the output should be a small value.
    auto rmseVal = rmse(N, inp, out);
    std::println("\nRMSE(inp, out) = {}\n", rmseVal);

    // wt_summary(wt); // Prints the full summary.
    // for (int i = 0; i < wt->outlength; ++i) {
    //     printf("%g ",wt->output[i]);
    // }

    wave_free(obj);
    wt_free(wt);
    save_data(out, N, "/home/maks/devel/bw/assets/sin_tan_exp_output.txt");
    delete[] inp;
    delete[] out;
    delete[] diff;

    return 0;
}
