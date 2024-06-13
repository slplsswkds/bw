#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <fstream>
#include <print>
#include <vector>
#include <iostream>
#include <array>
#include "../wavelib/header/wavelib.h"

int compare_double(const void *a, const void *b) {
    double arg1 = *(const double *) a;
    double arg2 = *(const double *) b;

    if (arg1 < arg2) return -1;
    if (arg1 > arg2) return 1;
    return 0;
}

double median(double *x, int N) {
    double sigma;

    qsort(x, N, sizeof(double), compare_double);

    if ((N % 2) == 0) {
        sigma = (x[N / 2 - 1] + x[N / 2]) / 2.0;
    } else {
        sigma = x[N / 2];
    }

    return sigma;
}

// Function to calculate the mean square of an array
double mean_square(const double *array, int size) {
    double sum = 0.0;
    for (int i = 0; i < size; i++) {
        sum += array[i] * array[i];
    }
    return sum / size;
}

// Function to calculate SNR
double snr(const int size, const double *signal, const double *noise) {
    const double signal_power = mean_square(signal, size);
    const double noise_power = mean_square(noise, size);
    return 10.0 * log10(signal_power / noise_power);
}

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

auto rezeroConcretLevelTest(const wt_object wt) {
    int offset = 0;
    std::cout << wt->outlength;
    for (int wtOutputVectorElement = 0; wtOutputVectorElement <= wt->MaxIter; wtOutputVectorElement++) {
        const auto decompositionLvlSize = wt->length[wtOutputVectorElement];

        if (wtOutputVectorElement == wt->J) {
            std::cout << "------ LEVEL SIZE = " << decompositionLvlSize << std::endl;
            std::cout << "------ Iteration № " << wtOutputVectorElement << "-------" << std::endl;

            for (int coefPos = 0; coefPos < decompositionLvlSize; coefPos++) {
                const auto coef = wt->output[offset + coefPos];
                // wt->output[offset + coefPos] = 0;
            }

            offset += decompositionLvlSize;
        }
    }
}

void neighshrink(const wt_object wt) {
    int offset = 0;
    for (int j = 0; j <= wt->J; j++) {
        if (j != 0) {
            // skip if it is approximation coefficienst
            const auto lvlSize = wt->length[j];

            // split signal into windows and apply neighshrink
            int windows = 10; // number of windows
            int windowSize = ceil(lvlSize / windows);

            for (int window = 0; window < windows; window++) {
                for (int coefPos = 0; coefPos < windowSize; coefPos++) {
                    double *pCoefAddr = &wt->output[window * windowSize + coefPos];

                    // copy window to calculate median
                    const auto pWindowCopy = new double[windowSize];
                    std::copy_n(&wt->output[offset], windowSize, pWindowCopy);
                    delete[] pWindowCopy;

                    double sigma = sigma = median(pWindowCopy, windowSize) / 0.6745;
                    double lambda = sigma * sqrt(2 * log(windowSize)) / (1 + log(j)); // or log(wt->J-j)

                    const bool coefIsFirs = (coefPos == 0);
                    const bool coefIsLast = (coefPos == (windowSize - 1));

                    double nPow2;
                    if (coefPos == 0 || coefIsLast) {
                        const double coef = *pCoefAddr;
                        nPow2 = std::pow(coef, 2);
                    } else {
                        double prevCoef = *(pCoefAddr - 1);
                        double coef = *pCoefAddr;
                        double nextCoef = *(pCoefAddr + 1);
                        nPow2 = std::pow(prevCoef, 2) + std::pow(coef, 2) + std::pow(nextCoef, 2);
                    }

                    std::println("sigma = {}, nPow2 = {}, lambda = {}", sigma, nPow2, lambda);
                    if (nPow2 >= lambda) {
                        // wt->output[offset + coefPos] = wt->output[offset + coefPos] * (1 - std::pow(lambda, 2)/nPow2);
                    } else {
                        *pCoefAddr = 0;
                    }
                }
            }
            offset += lvlSize;
        }
    }
}

int main() {
    constexpr int N = 44101;
    constexpr int sampleRate = 44100; // Hz
    constexpr int J = 12;

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
    // rezeroConcretLevelTest(wt);
    // neighshrink(wt);

    idwt(wt, out); // Perform IDWT

    // Test Reconstruction
    for (int i = 0; i < wt->siglength; ++i) {
        diff[i] = out[i] - inp[i];
    }
    printf("\nMAX %g\n", absmax(diff, wt->siglength));
    // If Reconstruction succeeded then the output should be a small value.
    auto rmseVal = rmse(N, inp, out);
    std::println("\nRMSE(inp, out) = {}\n", rmseVal);

    auto snrVal = snr(N, inp, out);
    std::println("\nsnr(inp, out) = {}dB\n", snrVal);

    // wt_summary(wt); // Prints the full summary.

    wave_free(obj);
    wt_free(wt);
    save_data(out, N, "/home/maks/devel/bw/assets/sin_tan_exp_output.txt");
    delete[] inp;
    delete[] out;
    delete[] diff;

    return 0;
}
