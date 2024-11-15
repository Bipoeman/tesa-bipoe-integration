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
    rc = MQTTClient_setCallbacks(mqttClient, (void *)&mqttClient, (MQTTClient_connectionLost *)&connectionLost, (MQTTClient_messageArrived *)&messageArrived, NULL);
    printf("MQTT Client Set Callback Status : %d\r\n", rc);
    MQTTClient_deliveryToken token;
    setupMQTT(&mqttClient);
    char topic[100];

    sprintf(topic, "%s/%s/%s", base_topic, localSerialNumber, "status");
    printf("MQTT TOPIC: %s\n", topic);
    MQTTClient_message pubmsg = MQTTClient_message_initializer;

    while (1) {
        pthread_cond_wait(&mqtt_cond, &mqtt_cond_mutex);
        
        MQTTClient_message pubmsg = MQTTClient_message_initializer;
        pubmsg.payload = mqttTransferPayload;
        pubmsg.payloadlen = strlen(mqttTransferPayload);
        pubmsg.qos = 2;
        pubmsg.retained = 0;
        rc = MQTTClient_publishMessage(mqttClient, topic, &pubmsg, &token);
        printf("MQTT Client Publish Status : %d\r\n", rc);
    }
    MQTTClient_destroy(mqttClient);
}

int messageArrived(void *context, char *topicName, int topicLen, MQTTClient_message *message) {
    // printf("Message arrived\n");
    // printf("   topic: %s\n", topicName);
    // printf("   message: %.*s\n", message->payloadlen, message->payload);
    strncpy(receivedMessage, (char *)message->payload, message->payloadlen);
    strcpy(receivedTopic, (char *)topicName);
    cJSON *json = cJSON_Parse(receivedMessage);
    cJSON *command = cJSON_GetObjectItemCaseSensitive(json, "command");
    // printf("Get Command \n%s\nis String : %d\n", receivedMessage,cJSON_IsString(command));
    if (cJSON_IsString(command) && (command->valuestring != NULL)) {
        char commandString[50];
        strcpy(commandString, command->valuestring);
        printf("Command -> %s\r\n", commandString);
        pthread_mutex_lock(&audio_cond_mutex);
        insert_value_command("./db/command.db","command",commandString);
        if (strcmp(commandString, "startRecord") == 0) {
            modeRecord = 1;
        }
        if (strcmp(commandString, "stopRecord") == 0) {
            modeRecord = 2;
        }
        if (strcmp(commandString, "normalRecord") == 0) {
            modeRecord = 0;
        }
        pthread_mutex_unlock(&audio_cond_mutex);
    }
    memset(receivedTopic, '\0', strlen(receivedTopic));
    memset(receivedMessage, '\0', message->payloadlen);
    cJSON_Delete(command);
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
    MQTTClient_deliveryToken token;
    char subscribeTopic[100];
    int rc;
    if ((rc = MQTTClient_connect(*mqttClient, &conn_opts)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to connect, return code %d\n", rc);
        if (rc == 5) {
            printf("Unauthorized\r\n");
        }
        exit(EXIT_FAILURE);
    }

    float value;

    // printf("Shared data: %d\n", shared_value);
    MQTTClient_message pubmsg = MQTTClient_message_initializer;

    sprintf(subscribeTopic, "%s/%s/%s", base_topic, localSerialNumber, "status");
    cJSON *json = cJSON_CreateObject();
    cJSON_AddStringToObject(json, "statue", "online");
    char *json_payload = cJSON_PrintUnformatted(json);
    pubmsg.payload = json_payload;
    pubmsg.payloadlen = strlen(json_payload);
    pubmsg.qos = 0;
    pubmsg.retained = 0;
    rc = MQTTClient_publishMessage(*mqttClient, subscribeTopic, &pubmsg, &token);
    printf("MQTT Client Publish Status : %d\r\n", rc);

    sprintf(subscribeTopic, "%s/%s/%s", base_topic, localSerialNumber, "command");
    printf("Subscribing -> %s\n", subscribeTopic);
    rc = MQTTClient_subscribe(*mqttClient, subscribeTopic, qos);
    printf("MQTT Client Subscribe Status : %d\r\n", rc);
}