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


extern pthread_cond_t  audio_cond;
extern pthread_mutex_t audio_cond_mutex;
extern short audio_buffer[9600];

extern const int samplingSize;
extern const int samplingRate;


void *server_thread(void*);
void *mqtt_thread(void*);
void *audio_input_thread(void*);
void *save_audio_thread(void*);
void *ml_thread(void*);
long long timeInMilliseconds(void);
int threshold_detect(short *input,int size,int threshold);

void dbase_init(const char *db_name);
int dbase_append(const char *db_name, int value);
int dbase_query(const char *db_name);
int insert_value(const char *db_name, const char *table_name, const char *value);

extern char serialNumber[100];