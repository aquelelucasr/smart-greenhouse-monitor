#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "mqtt_client.h"

#include "secrets.h"
#include "WIFI.h"
#include "MQTT.h"

#define OUTPUT_PIN GPIO_NUM_23
#define HIGH 1
#define LOW 0

// message handler do mqtt dos guri
void message_handler(const char* topic, const char* payload) {
    printf("RECEBIDO NO MAIN -> Tópico: %s | Payload: %s\n", topic, payload);
    
    // mantendo a lógica do led apenas para testes visuais
    if (strcmp(payload, "LIGA") == 0) {
        gpio_set_level(OUTPUT_PIN, HIGH);
    } else {
        gpio_set_level(OUTPUT_PIN, LOW);
    }
}

extern "C" void app_main(void)
{
    gpio_config_t io_conf = {}; // chaves vazias para limpar possíveis warnings
    io_conf.intr_type = GPIO_INTR_DISABLE; // sem interrupcoes
    io_conf.mode = GPIO_MODE_OUTPUT; // pinos de saida
    
    // pplica o bit shifft no pino definido
    io_conf.pin_bit_mask = (1ULL << OUTPUT_PIN); 
    
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE; // sem pulldown
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE; // sem pullup
    gpio_config(&io_conf);

    WIFI meuWiFi = WIFI(WIFI_SSID, WIFI_PASS);
    (void)meuWiFi;

    // delayzinho de guri
    vTaskDelay(pdMS_TO_TICKS(3000));

    MQTT meuMqtt = MQTT(MQTT_BROKER_URI);
    
    meuMqtt.subHandler(message_handler);
    meuMqtt.sub("/configura/alta");
    meuMqtt.sub("/configura/baixa");

    while (1) {
        // toma mais delay dos guri
        vTaskDelay(pdMS_TO_TICKS(5000)); 
    }
}