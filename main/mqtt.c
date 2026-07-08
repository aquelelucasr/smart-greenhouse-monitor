#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "mqtt_client.h"
#include "secrets.h"

#define SAIDA (1<<23)
#define HIGH 1
#define LOW 0

static void wifi_init(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_wifi_connect());
    printf("WiFi inicializado e conectando...\n");
}

/*************** MQTT EVENT HANDLER ************************/

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;

    switch (event->event_id) {

        case MQTT_EVENT_CONNECTED:
            printf("MQTT conectado!\n");

            esp_mqtt_client_subscribe(client, "ufsc/alerta", 0);
            esp_mqtt_client_publish(client, "ufsc/tempo", "Olá do ESP32!", 0, 1, 0);
            break;

        case MQTT_EVENT_DISCONNECTED:
            printf("MQTT desconectado.\n");
            break;

        case MQTT_EVENT_DATA:
            printf("Mensagem recebida:\n");
            printf("Tópico: %.*s\n", event->topic_len, event->topic);
            printf("Dados : %.*s\n", event->data_len, event->data);
            char dados[200];
            strncpy(dados, event->data,  event->data_len);
            dados[ event->data_len]=0;
            printf("RECEBEU: %s\n",dados);
            if (strcmp(dados,"LIGA")==0)
            {
                    gpio_set_level(23, HIGH);
            }
            else {
                      gpio_set_level(23, LOW);
            }
            break;

        default:
            printf("Evento MQTT id:%d\n", event->event_id);
            break;
    }
}

static void mqtt_start(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker = {
            .address = {
                .uri = MQTT_BROKER_URI
            }
        }
    };

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
}

/********************* APP MAIN ****************************/

void app_main(void)
{

    gpio_config_t io_conf;

    io_conf.intr_type = GPIO_INTR_DISABLE;//sem interrupcoes
    io_conf.mode = GPIO_MODE_OUTPUT;//pinos de saida
    io_conf.pin_bit_mask = SAIDA;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;// sem pulldown
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;//sem pullUP
    gpio_config(&io_conf);

    wifi_init();

    vTaskDelay(pdMS_TO_TICKS(3000));

    mqtt_start();
}
