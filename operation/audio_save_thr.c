#include "../main.h"
#include "./wav_file.h"
#define DATA_SIZE 960000
const char *dbPath = "./db/audio_capture.db";
void *save_audio_thread(void *) {
    printf("Save Audio Thread\n");
    int fileIndex;
    uint64_t timeFound;
    uint64_t timeStart;
    uint64_t timeEnd;
    int foundTimeout = 200;
    int isFound = 0;
    int lastIsFound = 0;

    short localBuf[DATA_SIZE];
    int bufIndex = 0;
    int isCapture = 0;
    wav_header wavh;
    strncpy(wavh.riff, "RIFF", 4);
    strncpy(wavh.wave, "WAVE", 4);
    strncpy(wavh.fmt, "fmt ", 4);
    strncpy(wavh.data, "data", 4);

    wavh.chunk_size = 16;
    wavh.format_tag = 1;
    wavh.num_chans = 1;
    wavh.srate = samplingRate;
    wavh.bits_per_samp = 16;
    wavh.bytes_per_sec = wavh.srate * wavh.bits_per_samp / 8 * wavh.num_chans;
    wavh.bytes_per_samp = wavh.bits_per_samp / 8 * wavh.num_chans;
    FILE *audioCaptureFile;
    int fileOpenned = 0;
    char filename[100];

    for (;;) {
        pthread_cond_wait(&audio_cond, &audio_cond_mutex);
        // printf("Found Audio From Thread Save Audio\n");
        int isAudio = threshold_detect(audio_buffer, samplingSize, 20);
        if (isAudio) {
            isCapture = 1;
            timeFound = timeInMilliseconds();
            isFound = 1;
        }

        if (isCapture && bufIndex < DATA_SIZE) {
            for (int i = 0; i < samplingSize; i++) {
                localBuf[i] = audio_buffer[i] << 4;
                // localBuf[i] = audio_buffer[i];
            }
            if (fileOpenned) {
                fwrite(localBuf, 2, samplingSize, audioCaptureFile);
            }
        }

        if (!isAudio && timeInMilliseconds() - timeFound > foundTimeout) {
            isFound = 0;
        }

        if (lastIsFound != isFound) {

            if (isFound) {
                // printf("Hit Above Threshold\n");
                sprintf(filename, "./captured_audio/capture%d.wav", fileIndex++);
                printf("Start recording '%s'\n", filename);
                audioCaptureFile = fopen(filename, "w");
                const int header_length = sizeof(wav_header);
                wavh.dlength = bufIndex * wavh.bytes_per_samp;
                wavh.flength = wavh.dlength + header_length;
                fwrite(&wavh, 1, header_length, audioCaptureFile);
                fileOpenned = 1;
            } else {
                printf("Audio Silenced, stop record\n");
                if (fileOpenned) {
                    fclose(audioCaptureFile);
                    struct stat st;
                    stat(filename, &st);
                    char dataSize[20];
                    sprintf(dataSize,"%ldB",st.st_size);
                    st.st_size;
                    insert_value(dbPath, "audio_record", dataSize);
                }
                // printf("Buff Size %ld\n", bufIndex);
                fileOpenned = 0;
                bufIndex = 0;
                isCapture = 0;
                // }
            }
            lastIsFound = isFound;
        }
    }
}