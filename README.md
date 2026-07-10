# Monitor de Estufa IoT - ESP32 🌡️🌱

Um sistema robusto de Internet das Coisas (IoT) desenvolvido em **C++** para o microcontrolador **ESP32**, utilizando o framework **ESP-IDF** e **FreeRTOS**. O dispositivo realiza o monitoramento contínuo de temperatura utilizando um sensor DS18B20, publicando alertas e recebendo configurações dinâmicas de limites via protocolo **MQTT**.

## 🚀 Funcionalidades

* **Monitoramento Contínuo:** Leitura da temperatura a cada 5 segundos.
* **Alertas Inteligentes:** Publicação automática no broker MQTT caso a temperatura ultrapasse os limites superior ou inferior configurados.
* **Configuração Dinâmica:** Atualização remota dos limites de temperatura em tempo de execução via *subscribe* em tópicos MQTT (sem necessidade de reprogramação).
* **Sob Demanda:** Responde a requisições externas enviando a temperatura atual ou os limites configurados no momento.
* **Arquitetura Orientada a Objetos (POO):** Código modularizado e limpo, com separação de responsabilidades e encapsulamento dos módulos de rede e hardware.

<br>

## 🛠️ Arquitetura do Software

O projeto implementa uma arquitetura modularizada utilizando as seguintes classes C++:

* `WIFI`: Encapsula toda a complexidade de inicialização, configuração e conexão da interface de rede (Station Mode).
* `MQTT`: Gerencia o client MQTT, controlando os eventos de conexão, assinaturas (*sub*) e publicações (*pub*), e repassando os *payloads* recebidos para a camada principal da aplicação.
* `DS18B20`: Interface de abstração do hardware. (Atualmente suporta *Mocking* de dados para testes de protocolo e transição invisível para o sensor físico OneWire).
* `main.cpp`: Orquestrador principal. Cuida apenas do loop do FreeRTOS e das regras de negócio do sistema.

<br>

## 📡 API MQTT (Tópicos)

O dispositivo se comunica através do broker público **`broker.emqx.io`**. 

### Publicações (O Dispositivo Envia)
| Tópico | Payload | Descrição |
| :--- | :--- | :--- |
| `/alerta/temperaturaAlta` | `[float]` | Disparado quando a temperatura excede o limite superior. |
| `/alerta/temperaturaBaixa` | `[float]` | Disparado quando a temperatura cai abaixo do limite inferior. |
| `/responde/temperaturaCorrente` | `[float]` | Retorna a temperatura atual em resposta a uma requisição. |
| `/responde/limiteAlta` | `[float]` | Retorna o limite superior atual em resposta a uma requisição. |
| `/responde/limiteBaixa` | `[float]` | Retorna o limite inferior atual em resposta a uma requisição. |

### Assinaturas (O Dispositivo Ouve)
| Tópico | Payload | Descrição |
| :--- | :--- | :--- |
| `/configura/alta` | `[float]` | Atualiza o limite máximo de temperatura. |
| `/configura/baixa` | `[float]` | Atualiza o limite mínimo de temperatura. |
| `/informa/temperaturaCorrente` | *(vazio)* | Solicita ao dispositivo o envio da temperatura atual. |
| `/informa/limiteAlta` | *(vazio)* | Solicita ao dispositivo o envio do limite superior configurado. |
| `/informa/limiteBaixa` | *(vazio)* | Solicita ao dispositivo o envio do limite inferior configurado. |

> **Nota:** Todos os tópicos são *case sensitive*.

<br>

## ⚙️ Hardware Necessário

* Placa de Desenvolvimento ESP32.
* Sensor de Temperatura DS18B20 (Ponta de prova à prova d'água).
* Resistor de 4.7kΩ (Para pull-up do barramento OneWire) *ou configuração de pull-up via software no GPIO do ESP32.*
* Protoboard e Jumpers.

<br>

## 💻 Como Compilar e Rodar

### 1. Pré-requisitos
Certifique-se de ter o [ESP-IDF](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/) devidamente instalado e exportado no seu ambiente.

### 2. Configurando Credenciais
Crie um arquivo chamado `secrets.h` dentro do diretório `main/` com as definições da sua rede Wi-Fi e URI do broker:

```cpp
#ifndef SECRETS_H
#define SECRETS_H

#define WIFI_SSID "NOME_DA_SUA_REDE"
#define WIFI_PASS "SENHA_DA_SUA_REDE"
#define MQTT_BROKER_URI "mqtt://broker.emqx.io"

#endif
```
### 3. Build e Flash
Conecte o ESP32 via USB. Compile o projeto, grave na flash e abra o monitor serial:

```bash
idf.py build flash monitor
```
*(Para sair do monitor serial, pressione `Ctrl + ]`)*.

<br>

## 🧪 Como Testar e Configurar Dinamicamente (Via Terminal)

O grande diferencial deste sistema é a capacidade de alterar as regras de negócio em tempo real pela internet, sem precisar reiniciar o ESP32. Para isso, instale e utilize as ferramentas de linha de comando `mosquitto_clients`.

### 1. Alterando os Limites de Temperatura
O dispositivo ouve constantemente os tópicos de configuração. Para alterar o limite máximo para **30.5 graus**, publique uma mensagem:

```bash
mosquitto_pub -h broker.emqx.io -t "/configura/alta" -m "30.5"
```

Para alterar o limite mínimo para **12 graus**:

```bash
mosquitto_pub -h broker.emqx.io -t "/configura/baixa" -m "12.0"
```

*O ESP32 atualizará a variável interna instantaneamente e passará a usar a nova faixa de temperatura para disparar os alertas.*

### 2. Solicitando Informações Sob Demanda
Para saber a temperatura exata no momento sem esperar um alerta, publique uma mensagem vazia no tópico de requisição:

```bash
mosquitto_pub -h broker.emqx.io -t "/informa/temperaturaCorrente" -m ""
```

### 3. Ouvindo os Alertas (Monitoramento)
Para visualizar o sistema funcionando e receber as mensagens publicadas pelo ESP32, deixe um terminal rodando o comando de assinatura genérica:

```bash
mosquitto_sub -h broker.emqx.io -t "/alerta/#" -v
```
*(A flag `-v` mostrará tanto o tópico quanto o valor da temperatura no seu terminal).*