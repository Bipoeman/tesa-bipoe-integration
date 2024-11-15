#include "../fft/rt_nonfinite.h"
#include "../main.h"
#include "CheckEvent.h"
#include "ExtractFeatures.h"
#include "NeuralNgin_terminate.h"
#include "NeuralNgin_types.h"
#include "NeuralPredictAudio2.h"

/* Function Declarations */
static void argInit_1003x14_real_T(double result[14042]);
static void argInit_1x43200_real_T(double result[43200]);
static double argInit_real_T(void);

static void argInit_1003x14_real_T(double result[14042]) {
    int i;
    for (i = 0; i < 14042; i++) {
        result[i] = argInit_real_T();
    }
}

static void argInit_1x43200_real_T(double result[43200]) {
    int idx1;
    for (idx1 = 0; idx1 < 43200; idx1++) {
        result[idx1] = argInit_real_T();
    }
}

static double argInit_real_T(void) {
    return 0.0;
}

void *ml_thread(void *) {
    double localBuf[samplingSize];
    short rawBuf[samplingSize];
    printf("ML Running\n");
    static double features[14042];
    categorical predictResult;
    float confidence;

    while (1) {
        pthread_cond_wait(&ml_cond, &ml_cond_mutex);
        for (int i = 0; i < samplingSize; i++) {
            localBuf[i] = (audio_buffer[i] << 4) / 32768.0;
        }
        double m;
        bool isCandidate;
        CheckEvent(localBuf, &m, &isCandidate);
        // printf("ce\n");
        ExtractFeatures(localBuf, 0.0, features);
        // printf("ef\n");
        NeuralPredictAudio2(features, &predictResult, &confidence);
        // printf("pa\n");
        // printf("confidence: %f preduct_result: %c iscandidate: %d m: %lf\n",confidence,predictResult.codes,isCandidate,m);
        // printf();
        // int isAudio = threshold_detect(rawBuf,samplingSize,40);
    }
}

void main_CheckEvent(void) {
    static double b_dv[43200];
    double m;
    bool isCandidate;
    argInit_1x43200_real_T(b_dv);
    CheckEvent(b_dv, &m, &isCandidate);
}

void main_ExtractFeatures(void) {
    static double b_dv[43200];
    static double features[14042];
    argInit_1x43200_real_T(b_dv);
    ExtractFeatures(b_dv, argInit_real_T(), features);
}

void main_NeuralPredictAudio2(void) {
    static double b_dv[14042];
    categorical predictResult;
    float confidence;
    argInit_1003x14_real_T(b_dv);
    NeuralPredictAudio2(b_dv, &predictResult, &confidence);
}