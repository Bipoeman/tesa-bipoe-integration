#include "main.h"

char serialNumber[100];
const int samplingSize = 4800;
const int samplingRate = 48000;

int modeRecord = 0; // 0 auto, 1 record, 2 stop
pthread_cond_t  audio_cond;
pthread_mutex_t audio_cond_mutex;
pthread_cond_t  http_cond;
pthread_mutex_t http_cond_mutex;
short audio_buffer[9600];

                        // upload_file_to_http("https://pvhlxgc5-8080.asse.devtunnels.ms/sound/",filename,);
const char http_server_url[100] = "https://pvhlxgc5-8080.asse.devtunnels.ms/sound/";
char bearerToken[100] = "QcrAF4l5+GLoZu9ePTI1GS411u3uSdOOiyNITM7zQzI=";
char httpFilePath[100];
char httpTimeStamp[100];

int main(int argc,char **argv){
    pthread_t tcpServerThread,mqttClientThread,audioInputThread,audioSaveThread,mlThread,httpUpload;
    FILE *infoFile;
    char info[200];
    printf("Hello World\n");
    printf("\033[2J\033[H\n");
    infoFile = fopen("./boardInfo.json","r");
    fgets(info,200,infoFile);
    fclose(infoFile);
    cJSON *json = cJSON_Parse(info);
    cJSON *name = cJSON_GetObjectItemCaseSensitive(json, "serial"); 
    
    if (cJSON_IsString(name)) { 
        strcpy(serialNumber,name->valuestring);
    } 
    cJSON_free(name);

    pthread_mutex_init(&audio_cond_mutex,NULL);
    pthread_cond_init(&audio_cond,&audio_cond_mutex);

    printf("Serial Number of this board is -> %s\n",serialNumber);

    pthread_create(&tcpServerThread,NULL,server_thread,NULL);
    pthread_create(&mqttClientThread,NULL,mqtt_thread,NULL);
    pthread_create(&audioInputThread,NULL,audio_input_thread,NULL);
    pthread_create(&audioSaveThread,NULL,save_audio_thread,NULL);
    pthread_create(&mlThread,NULL,ml_thread,NULL);
    pthread_create(&httpUpload,NULL,file_upload_thread,NULL);

    pthread_join(tcpServerThread,NULL);
    pthread_join(mqttClientThread,NULL);
    pthread_join(audioInputThread,NULL);
    pthread_join(audioSaveThread,NULL);
    pthread_join(mlThread,NULL);
    pthread_join(httpUpload,NULL);
    return 0;
}

long long timeInMilliseconds(void) {
    struct timeval tv;

    gettimeofday(&tv,NULL);
    return (((long long)tv.tv_sec)*1000)+(tv.tv_usec/1000);
}