# Uso do MQTT

O MQTT (*Message Queuing Telemetry Transport*) é um protocolo de mensagens leve, baseado no modelo de publicação e assinatura (*publish-subscribe*). Ele foi projetado para aplicações com dispositivos de baixo poder computacional e redes com largura de banda limitada, sendo amplamente utilizado em aplicações de Internet das Coisas (IoT).

Para utilizar o protocolo MQTT, um dispositivo conectado à Internet deve se comunicar com um **broker**, que é o servidor responsável por receber mensagens publicadas e distribuí-las aos clientes interessados. Existem diversas implementações de brokers MQTT, tanto comerciais quanto de código aberto. Uma das implementações livres mais conhecidas é o **Mosquitto**, que permite criar um broker próprio. Também existem brokers públicos disponíveis para testes, como o **broker.emqx.io**.

O funcionamento do MQTT baseia-se em **tópicos** (*topics*). Um dispositivo pode se registrar para receber mensagens de um determinado tópico por meio da operação **subscribe**. Por exemplo, suponha que o dispositivo A execute um *subscribe* no tópico **comandos**. Sempre que qualquer cliente conectado ao mesmo broker publicar uma mensagem nesse tópico, o dispositivo A receberá automaticamente essa mensagem.

Da mesma forma, o dispositivo A pode publicar mensagens em um tópico, por exemplo **fogo**, utilizando a operação **publish**. Qualquer outro cliente que tenha realizado um *subscribe* nesse tópico receberá imediatamente as mensagens publicadas.

Uma das principais vantagens do MQTT é seu funcionamento baseado em **eventos**. O cliente não precisa consultar periodicamente o servidor para verificar se existem novas mensagens (técnica conhecida como *polling*), como normalmente ocorre em aplicações baseadas em HTTP. Sempre que uma nova mensagem chega a um tópico assinado, o broker a entrega automaticamente aos clientes inscritos.

Outra vantagem é o baixo overhead do protocolo. Enquanto o HTTP utiliza cabeçalhos relativamente grandes em formato textual, o MQTT possui cabeçalhos muito menores, reduzindo o volume de dados transmitidos. Essas características tornam o MQTT especialmente adequado para dispositivos IoT, que frequentemente possuem recursos limitados e utilizam conexões de baixa velocidade ou com restrições de consumo de energia.

---

# Uso do MQTT no ESP32

Antes de utilizar o protocolo MQTT, o ESP32 deve estar conectado à Internet. Para isso, é necessário configurá-lo como cliente Wi-Fi e conectá-lo a um ponto de acesso (*Access Point*). Somente após a conexão com a rede será possível estabelecer uma conexão com um broker MQTT.

Nos exemplos desta disciplina será utilizado o broker público **broker.emqx.io**, que pode ser acessado gratuitamente para fins de desenvolvimento e testes.

A biblioteca **mqtt_client.h**, fornecida pelo ESP-IDF, disponibiliza as principais funções necessárias para configurar e utilizar um cliente MQTT.

## Inicialização do cliente MQTT

A primeira etapa consiste em criar a configuração do cliente e inicializá-lo por meio da função:

```c
esp_mqtt_client_handle_t esp_mqtt_client_init(
        const esp_mqtt_client_config_t *config);
```


~~~
int esp_mqtt_client_subscribe_single(esp_mqtt_client_handle_t client,
const char *topic, int qos)

Subscribe the client to defined topic with defined qos.
Notes:

Client must be connected to send subscribe message. This API is could be
executed from a user task or from a MQTT event callback i.e. internal
MQTT task (API is protected by internal mutex, so it might block if a
longer data receive operation is in progress.

esp_mqtt_client_subscribe could be used to call this function.

Parameters:
   * client – MQTT client handle
   * topic – topic filter to subscribe
   * qos – Max qos level of the subscription

Returns :
     message_id of the subscribe message on success -1 on failure -2 in
     case of full outbox.
~~~

# Teste

No Linux podemos instalar um pacote de mqtt-client para via linha de comando enviar mensagens para tópicos de um broker e também fazer subscribe de tópicos.

## Para publicar num tópico:

mosquitto_pub -d    -h  broker.emqx.io   -t "comandos" -m 'leia sensor de gas'

## Para assinar um tópico:
mosquitto_sub -d     -h  broker.emqx.io   -t "sensores"
