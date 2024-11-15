#include "../main.h"
#include "./wav_file.h"
#define DATA_SIZE 960000
const char *dbPath = "./db/audio_capture.db";

void getTimeStamp(char *buffer) {
    time_t raw_time;
    struct tm *time_info;
    time(&raw_time);
    time_info = gmtime(&raw_time);
    strftime(buffer, 50, "%Y-%m-%dT%H:%M:%SZ", time_info);
}

void *save_audio_thread(void *) {
    printf("Save Audio Thread\n");
    int fileIndex;
    int manualRecordIndex;
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
    char timeStampBuffer[50];

    for (;;) {
        pthread_cond_wait(&audio_cond, &audio_cond_mutex);
        for (int i = 0; i < samplingSize; i++) {
            localBuf[i] = audio_buffer[i] << 2;
            // localBuf[i] = audio_buffer[i];
        }
        int freqDetect = detectSync(localBuf, samplingSize, 70);
        if (freqDetect > 0) {
            if (abs(171 - freqDetect) <= 1) {
                printf("Start Detected %d\n", freqDetect);
                cJSON *json = cJSON_CreateObject();
                getTimeStamp(timeStampBuffer);
                cJSON_AddStringToObject(json, "report_state", "sync_signal_detected");
                cJSON_AddStringToObject(json, "filename", filename);
                cJSON_AddStringToObject(json, "timestamp", timeStampBuffer);
                pthread_mutex_lock(&mqtt_cond_mutex);
                strcpy(mqttTransferPayload, cJSON_PrintUnformatted(json));
                pthread_cond_signal(&mqtt_cond);
                pthread_mutex_unlock(&mqtt_cond_mutex);
                cJSON_Delete(json);
            }
        }
        // printf("Found Audio From Thread Save Audio\n");
        if (modeRecord == 0) {
            int isAudio = threshold_detect(audio_buffer, samplingSize, 20);
            if (isAudio) {
                isCapture = 1;
                timeFound = timeInMilliseconds();
                isFound = 1;
            }

            if (isCapture && bufIndex < DATA_SIZE) {

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
                    sprintf(filename, "./captured_audio/auto_capture_%d.wav", fileIndex++);
                    printf("Start recording '%s'\n", filename);

                    cJSON *json = cJSON_CreateObject();
                    getTimeStamp(timeStampBuffer);
                    cJSON_AddStringToObject(json, "report_state", "audio_detected");
                    cJSON_AddStringToObject(json, "filename", filename);
                    cJSON_AddStringToObject(json, "timestamp", timeStampBuffer);
                    pthread_mutex_lock(&mqtt_cond_mutex);
                    strcpy(mqttTransferPayload, cJSON_PrintUnformatted(json));
                    pthread_cond_signal(&mqtt_cond);
                    pthread_mutex_unlock(&mqtt_cond_mutex);
                    cJSON_Delete(json);

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

                        cJSON *json = cJSON_CreateObject();
                        cJSON_AddStringToObject(json, "report_state", "detected_audio_saved");
                        cJSON_AddStringToObject(json, "filename", filename);
                        getTimeStamp(timeStampBuffer);
                        cJSON_AddStringToObject(json, "timestamp", timeStampBuffer);
                        pthread_mutex_lock(&mqtt_cond_mutex);
                        strcpy(mqttTransferPayload, cJSON_PrintUnformatted(json));
                        pthread_cond_signal(&mqtt_cond);
                        pthread_mutex_unlock(&mqtt_cond_mutex);
                        cJSON_Delete(json);

                        struct stat st;
                        stat(filename, &st);
                        char dataSize[20];
                        sprintf(dataSize, "%ldB", st.st_size);
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
        } else if (modeRecord == 1) {
            if (!fileOpenned) {
                sprintf(filename, "./captured_audio/command_record_%d.wav", fileIndex++);
                printf("Start recording '%s'\n", filename);
                audioCaptureFile = fopen(filename, "w");
                const int header_length = sizeof(wav_header);
                wavh.dlength = bufIndex * wavh.bytes_per_samp;
                wavh.flength = wavh.dlength + header_length;
                fwrite(&wavh, 1, header_length, audioCaptureFile);
                fwrite(localBuf, 2, samplingSize, audioCaptureFile);
                fileOpenned = 1;

                cJSON *json = cJSON_CreateObject();
                cJSON_AddStringToObject(json, "report_state", "start_record_command");
                cJSON_AddStringToObject(json, "filename", filename);
                getTimeStamp(timeStampBuffer);
                cJSON_AddStringToObject(json, "timestamp", timeStampBuffer);
                pthread_mutex_lock(&mqtt_cond_mutex);
                strcpy(mqttTransferPayload, cJSON_PrintUnformatted(json));
                pthread_cond_signal(&mqtt_cond);
                pthread_mutex_unlock(&mqtt_cond_mutex);
                cJSON_Delete(json);
            }
            if (fileOpenned) {
                fwrite(localBuf, 2, samplingSize, audioCaptureFile);
            }
        } else if (modeRecord == 2) {
            if (fileOpenned) {
                fclose(audioCaptureFile);
                struct stat st;
                stat(filename, &st);
                char dataSize[20];
                sprintf(dataSize, "%ldB", st.st_size);
                st.st_size;
                insert_value(dbPath, "audio_record", dataSize);
                char buffer[50];
                time_t raw_time;
                struct tm *time_info;
                time(&raw_time);
                time_info = gmtime(&raw_time);
                strftime(buffer, 50, "%Y-%m-%dT%H:%M:%SZ", time_info);

                cJSON *json = cJSON_CreateObject();
                cJSON_AddStringToObject(json, "report_state", "stop_record_command");
                cJSON_AddStringToObject(json, "filename", filename);
                getTimeStamp(timeStampBuffer);
                cJSON_AddStringToObject(json, "timestamp", timeStampBuffer);
                pthread_mutex_lock(&mqtt_cond_mutex);
                strcpy(mqttTransferPayload, cJSON_PrintUnformatted(json));
                pthread_cond_signal(&mqtt_cond);
                pthread_mutex_unlock(&mqtt_cond_mutex);
                cJSON_Delete(json);

                pthread_mutex_lock(&http_cond_mutex);
                strcpy(httpFilePath, filename);
                strcpy(httpTimeStamp, buffer);
                pthread_cond_signal(&http_cond);
                pthread_mutex_unlock(&http_cond_mutex);
            }
            fileOpenned = 0;
        }
    }
}