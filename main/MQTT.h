#ifndef MQTT_H
#define MQTT_H

#include "mqtt_client.h"

typedef void (*mqtt_callback_t)(const char* topic, const char* payload);

class MQTT {
private:
    esp_mqtt_client_handle_t client;
    mqtt_callback_t user_callback;
    bool is_connected;

    // esse trem aqui tem que ser estático pra api do esp-idf aceitar (pq e em c)
    static void mqtt_event_handler(void* handler_args, esp_event_base_t base, int32_t event_id, void* event_data);

public:
    MQTT(const char* broker_uri);
    
    void sub(const char* topic);
    void pub(const char* topic, const char* payload);
    void subHandler(mqtt_callback_t callback);
};

#endif