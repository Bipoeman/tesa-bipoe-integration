#include "../fft/sound_freq.h"
#include "../main.h"
#include <float.h>

int threshold_detect(short *input, int size, int threshold) {
    for (int i = 0; i < size; i++) {
        if (input[i] > threshold) {
            // printf("Hit Thresh %d\n",input[i]);
            return 1;
        }
    }
    return 0;
}

int detectSync(short *dataIn, int size, int threshold) {
    int indexFound = -1;
    for (int i = 0; i < size; i++) {
        if (dataIn[i] > threshold) {
            indexFound = i;
            break;  // Stop after the first match
        }
    }
    if (indexFound == -1) {
        // printf("No threshold hit\n");
        return -1;  // No valid index found
    }
    int halfSize = size / 2;
    if (indexFound > halfSize) {
        // printf("Too Small\n");
        return -1;  // Invalid, too late in the data
    }
    double waveData[4096];  // Adjusted size to halfSize
    double spectrum[2048];
    for (int i = 0; i < halfSize; i++) {
        waveData[i] = 0.00001;
    }
    for (int i = 0; i < 4096; i++) {
        waveData[i] = fabs((dataIn[i] << 4) / 32768.0);  // Make sure shift and scale are correct
    }
    sound_freq(waveData, spectrum);
    double max = -DBL_MAX;
    int maxIndex = -1;
    for (int i = 1; i < 2048; i++) {
        if (spectrum[i] > max){
            max = spectrum[i];
            maxIndex = i;
        }
        // printf("%lf\n",spectrum[i]);
    }
    // printf("Maximum frequency index: %d, value: %f\n", maxIndex, max);

    return maxIndex;
}