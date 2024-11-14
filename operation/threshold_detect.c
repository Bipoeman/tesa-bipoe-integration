#include "../main.h"

int threshold_detect(short *input,int size,int threshold){
    for (int i = 0;i<size;i++){
        if (input[i] > threshold){
            // printf("Hit Thresh %d\n",input[i]);
            return 1;
        }
    }
    return 0;
}