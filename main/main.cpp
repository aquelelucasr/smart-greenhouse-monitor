#include <stdio.h>
#include <stdlib.h>
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
#include "DS18B20.h"

#define OUTPUT_PIN GPIO_NUM_23
#define HIGH 1
#define LOW 0

// variaveis globais de estado
float limite_alta = 35.0; 
float limite_baixa = 15.0;
float temperatura_corrente = 0.0;
MQTT* app_mqtt = NULL; // ponteiro pra gente usar no handler

// message handler do mqtt dos guri
void message_handler(const char* topic, const char* payload) {
    printf("RECEBIDO NO MAIN -> Tópico: %s | Payload: %s\n", topic, payload);
    char buffer[20];
    
    // mantendo a lógica do led apenas para testes visuais
    if (strcmp(payload, "LIGA") == 0) {
        gpio_set_level(OUTPUT_PIN, HIGH);
    } else if (strcmp(payload, "DESLIGA") == 0) {
        gpio_set_level(OUTPUT_PIN, LOW);
    }
    
    // configura os limites
    if (strcmp(topic, "/configura/alta") == 0) {
        limite_alta = atof(payload);
        printf(">> Limite ALTO atualizado: %.2f\n", limite_alta);
    } 
    else if (strcmp(topic, "/configura/baixa") == 0) {
        limite_baixa = atof(payload);
        printf(">> Limite BAIXO atualizado: %.2f\n", limite_baixa);
    }
    // responde as solicitacoes de informacao
    else if (strcmp(topic, "/informa/temperaturaCorrente") == 0) {
        sprintf(buffer, "%.2f", temperatura_corrente);
        app_mqtt->pub("/responde/temperaturaCorrente", buffer);
    }
    else if (strcmp(topic, "/informa/limiteAlta") == 0) {
        sprintf(buffer, "%.2f", limite_alta);
        app_mqtt->pub("/responde/limiteAlta", buffer);
    }
    else if (strcmp(topic, "/informa/limiteBaixa") == 0) {
        sprintf(buffer, "%.2f", limite_baixa);
        app_mqtt->pub("/responde/limiteBaixa", buffer);
    }
}

extern "C" void app_main(void)
{
    gpio_config_t io_conf = {}; // zerando chaves para limpar possíveis warnings
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
    app_mqtt = &meuMqtt; // salva a referencia global pro handleer

    meuMqtt.subHandler(message_handler);

    // assinando os topicos exigidos
    meuMqtt.sub("/configura/alta");
    meuMqtt.sub("/configura/baixa");
    meuMqtt.sub("/informa/temperaturaCorrente");
    meuMqtt.sub("/informa/limiteAlta");
    meuMqtt.sub("/informa/limiteBaixa");

    // instanciando o sensor falso pq to sem os jumper pra testar o de verdade :(
    DS18B20 meuSensor = DS18B20(GPIO_NUM_4);

    char buffer[20]; // bufferzinho pra converter float pra string

    while (1) {
        temperatura_corrente = meuSensor.readTemp();
        printf("Temperatura: %.2f graus (Baixa: %.1f | Alta: %.1f)\n", temperatura_corrente, limite_baixa, limite_alta);

        // Alertas automaticos
        if (temperatura_corrente > limite_alta) {
            sprintf(buffer, "%.2f", temperatura_corrente);
            meuMqtt.pub("/alerta/temperaturaAlta", buffer);
        } else if (temperatura_corrente < limite_baixa) {
            sprintf(buffer, "%.2f", temperatura_corrente);
            meuMqtt.pub("/alerta/temperaturaBaixa", buffer);
        }

        // toma mais delay dos guri (nesse caso, pra ler a temperatura de de 5 em 5 segundos)
        vTaskDelay(pdMS_TO_TICKS(5000)); 
    }
}