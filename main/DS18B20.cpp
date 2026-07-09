#include "DS18B20.h"
#include "esp_random.h"

DS18B20::DS18B20() {
    // A inicialização real do pino do sensor físico entrará aqui amanhã
}

float DS18B20::le() {
    // Gera um número inteiro aleatório entre 0 e 400
    uint32_t random_val = esp_random() % 401; 
    
    // Converte para um float entre 0.0 e 40.0 para simular a temperatura
    return (float)random_val / 10.0;
}