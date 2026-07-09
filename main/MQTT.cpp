#include "MQTT.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>
#include <stdio.h>

MQTT::MQTT(const char* broker_uri) {
    this->is_connected = false;
    this->user_callback = NULL;
    esp_mqtt_client_config_t mqtt_cfg = {};
    mqtt_cfg.broker.address.uri = broker_uri;
    this->client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(this->client, MQTT_EVENT_ANY, mqtt_event_handler, this);
    esp_mqtt_client_start(this->client);
}

void MQTT::mqtt_event_handler(void* handler_args, esp_event_base_t base, int32_t event_id, void* event_data) {
    MQTT* instance = (MQTT*)handler_args;
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;

    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            printf("MQTT Conectado ao broker!\n");
            instance->is_connected = true;
            break;
            
        case MQTT_EVENT_DISCONNECTED:
            printf("MQTT Desconectado.\n");
            instance->is_connected = false;
            break;
            
        case MQTT_EVENT_DATA:
            if (instance->user_callback != NULL) {
                char topic[100];
                char payload[100];
                snprintf(topic, sizeof(topic), "%.*s", event->topic_len, event->topic);
                snprintf(payload, sizeof(payload), "%.*s", event->data_len, event->data);

                instance->user_callback(topic, payload);
            }
            break;
            
        default:
            break;
    }
}

void MQTT::sub(const char* topic) {
    while (!this->is_connected) {
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    esp_mqtt_client_subscribe(this->client, topic, 0);
    printf("Inscrito no tópico: %s\n", topic);
}

void MQTT::pub(const char* topic, const char* payload) {
    if (this->is_connected) {
        esp_mqtt_client_publish(this->client, topic, payload, 0, 1, 0);
        printf("Publicado -> Tópico: %s | Payload: %s\n", topic, payload);
    }
}

void MQTT::subHandler(mqtt_callback_t callback) {
    this->user_callback = callback;
}