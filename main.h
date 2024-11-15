#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <cjson/cJSON.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <curl/curl.h>

extern int modeRecord;

extern char mqttTransferPayload[512];
extern pthread_cond_t  mqtt_cond;
extern pthread_mutex_t mqtt_cond_mutex;

extern pthread_cond_t  audio_cond;
extern pthread_mutex_t audio_cond_mutex;
extern pthread_cond_t  http_cond;
extern pthread_mutex_t http_cond_mutex;
extern pthread_cond_t  ml_cond;
extern pthread_mutex_t ml_cond_mutex;
extern short audio_buffer[48000];

extern const int samplingSize;
extern const int samplingRate;

extern const char http_server_url[100];
extern char bearerToken[100];
extern char httpFilePath[100];
extern char httpTimeStamp[100];

void *server_thread(void*);
void *mqtt_thread(void*);
void *audio_input_thread(void*);
void *save_audio_thread(void*);
void *ml_thread(void*);
void *file_upload_thread(void*);
long long timeInMilliseconds(void);
int threshold_detect(short *input,int size,int threshold);
int detectSync(short *dataIn,int size,int threshold);
void getTimeStamp(char *buffer);
int upload_file_to_http(const char *url, const char *file_path, const char *bearer_token, const char *timestamp, const char *device_id);

void dbase_init(const char *db_name);
int dbase_append(const char *db_name, int value);
int dbase_query(const char *db_name);
int insert_value(const char *db_name, const char *table_name, const char *value);
int insert_value_command(const char *db_name, const char *table_name, const char *value);

extern char serialNumber[100];