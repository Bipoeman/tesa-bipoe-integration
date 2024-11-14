#include "../main.h"
#include <MQTTClient.h>

const char MQTT_BROKER[] = "tcp://openfruit-tesa.southeastasia.cloudapp.azure.com:1883";
char base_topic[60] = "rpi";
const int qos = 2;
char mqtt_clientid[200];
char localSerialNumber[100];

char receivedMessage[200];
char receivedTopic[200];

int messageArrived(void *context, char *topicName, int topicLen, MQTTClient_message *message);
int connectionLost(void *context, char *cause);

void setupMQTT(MQTTClient *mqttClient);

void *mqtt_thread(void *) {
    // setup
    strcpy(localSerialNumber, serialNumber);
    sprintf(mqtt_clientid, "raspi-%s", localSerialNumber);
    int rc;
    int shared_value = 55;
    printf("Starting MQTT thread\n");
    MQTTClient mqttClient;
    MQTTClient_create(&mqttClient, MQTT_BROKER, mqtt_clientid, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    rc = MQTTClient_setCallbacks(mqttClient, (void *)&mqttClient, &connectionLost, &messageArrived, NULL);
    printf("MQTT Client Set Callback Status : %d\r\n", rc);
    MQTTClient_deliveryToken token;
    setupMQTT(&mqttClient);
    char topic[100];

    sprintf(topic, "%s/%s/%s", base_topic, localSerialNumber, "data");
    printf("MQTT TOPIC: %s\n", topic);
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    while (1) {
        // loop
        // float value;

        // // printf("Shared data: %d\n", shared_value);
        // value = shared_value;
        // cJSON *json = cJSON_CreateObject();
        // cJSON_AddStringToObject(json, "team", (char *)"Hello");
        // cJSON_AddNumberToObject(json, "value", value);
        // char *json_payload = cJSON_Print(json);
        // pubmsg.payload = json_payload;
        // pubmsg.payloadlen = strlen(json_payload);
        // pubmsg.qos = 0;
        // pubmsg.retained = 0;
        // MQTTClient_publishMessage(mqttClient, topic, &pubmsg, &token);
        // // printf("Publish to %s\n", topic);
        // cJSON_free(json);
        int msgLen = strlen(receivedMessage);
        if (msgLen > 0) {
            cJSON *json = cJSON_Parse(receivedMessage);
            cJSON *command = cJSON_GetObjectItemCaseSensitive(json, "command");
            // printf("Get Command \n%s\nis String : %d\n", receivedMessage,cJSON_IsString(command));
            if (cJSON_IsString(command) && (command->valuestring != NULL)) {
                char commandString[50];
                strcpy(commandString, command->valuestring);
                printf("Command -> %s\r\n", commandString);
                pthread_mutex_lock(&audio_cond_mutex);
                if (strcmp(commandString,"startRecord") == 0){
                    modeRecord = 1;
                }
                if (strcmp(commandString,"stopRecord") == 0){
                    modeRecord = 2;
                }
                if (strcmp(commandString,"normalRecord") == 0){
                    modeRecord = 0;
                }
                pthread_mutex_unlock(&audio_cond_mutex);
            }
            memset(receivedTopic, '\0', strlen(receivedTopic));
            memset(receivedMessage, '\0', msgLen);
            cJSON_Delete(command);
        }
        // sleep(1);
    }
    MQTTClient_destroy(mqttClient);
}

int messageArrived(void *context, char *topicName, int topicLen, MQTTClient_message *message) {
    // printf("Message arrived\n");
    // printf("   topic: %s\n", topicName);
    // printf("   message: %.*s\n", message->payloadlen, message->payload);
    strncpy(receivedMessage, (char *)message->payload, message->payloadlen);
    strcpy(receivedTopic, (char *)topicName);
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

int connectionLost(void *context, char *cause) {
    printf("Connection Lost : %s\n", cause);
    setupMQTT((MQTTClient *)context);
}

void setupMQTT(MQTTClient *mqttClient) {
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    conn_opts.keepAliveInterval = 10;
    conn_opts.cleansession = 1;
    conn_opts.username = "device";
    conn_opts.password = "2816888888";
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;

    char subscribeTopic[100];
    int rc;
    if ((rc = MQTTClient_connect(*mqttClient, &conn_opts)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to connect, return code %d\n", rc);
        if (rc == 5) {
            printf("Unauthorized\r\n");
        }
        exit(EXIT_FAILURE);
    }

    sprintf(subscribeTopic, "%s/%s/%s", base_topic, localSerialNumber, "command");
    printf("Subscribing -> %s\n", subscribeTopic);
    rc = MQTTClient_subscribe(*mqttClient, subscribeTopic, qos);
    printf("MQTT Client Subscribe Status : %d\r\n", rc);
}