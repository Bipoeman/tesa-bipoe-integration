#include "../main.h"
#include <alsa/asoundlib.h>
char *audioInputPort = "plughw:1,0";

void *audio_input_thread(void *) {
    printf("Audio Thread\n");
    int i;
    int err;
    short buf[9600];
    snd_pcm_t *capture_handle;
    snd_pcm_hw_params_t *hw_params;
    snd_pcm_sw_params_t *sw_params;

    // initialize sound
    if ((err = snd_pcm_open(&capture_handle, audioInputPort, SND_PCM_STREAM_CAPTURE, 0)) < 0) {
        fprintf(stderr, "cannot open audio device %s (%s)\n", audioInputPort, snd_strerror(err));
        exit(1);
    } else {
        printf("snd_pcm_open OK\n");
    }
    snd_pcm_hw_params_alloca(&hw_params);
    snd_pcm_sw_params_alloca(&sw_params);

    unsigned int rate = samplingRate;
    unsigned int channels = 1;
    snd_pcm_hw_params_any(capture_handle, hw_params);
    snd_pcm_hw_params_set_access(capture_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(capture_handle, hw_params, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_rate_near(capture_handle, hw_params, &rate, 0);
    snd_pcm_hw_params_set_channels(capture_handle, hw_params, channels);
    snd_pcm_hw_params(capture_handle, hw_params);
    printf("HW init ok\n");

    snd_pcm_sw_params_current(capture_handle, sw_params);
    snd_pcm_sw_params_set_start_threshold(capture_handle, sw_params, 0);
    snd_pcm_sw_params(capture_handle, sw_params);
    printf("SW init ok\n");

    // Start the audio stream
    if ((err = snd_pcm_prepare(capture_handle)) < 0) {
        fprintf(stderr, "unable to prepare audio stream: %s\n", snd_strerror(err));
        exit(1);
    }

    // loop reading sound data
    // for (i = 0; i < 1000; ++i) {
    
    while (1) {
        if ((err = snd_pcm_readi(capture_handle, buf, samplingSize)) != samplingSize) {
            fprintf(stderr, "read from audio interface failed (%s)\n", snd_strerror(err));
            exit(1);
        }
        pthread_mutex_lock(&audio_cond_mutex);
        for (int i = 0;i<samplingSize;i++){
            audio_buffer[i] = buf[i];
        }
        pthread_cond_signal(&audio_cond);
        pthread_mutex_unlock(&audio_cond_mutex);
        
        
        // if ()
        // float avg_val = 0.0;
        // for (int j = 0; j < 4800; j++) {
        //     avg_val += abs(buf[j]);
        // }
        // avg_val /= 4800;
        // printf("snd_pcm_readi %f\n", avg_val);
    }
    // }

    snd_pcm_close(capture_handle);
    snd_pcm_hw_params_free(hw_params);
    snd_pcm_sw_params_free(sw_params);
}