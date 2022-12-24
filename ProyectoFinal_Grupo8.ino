/*
 Curso: IOT
 Ing. Computacion y Sistemas
 Grupo: 8
 Integrantes:
FERNANDEZ YOPLA, JOSE
HOYOS RODRIGUEZ, MARIA
PACHECO TENICELA, GUSTAVO
REBAZA RODRIGUEZ, DANNER


 */

/****************************************
 * Incluir Librerias
 ****************************************/
#include <WiFi.h>          // Libreria para WiFi
#include <PubSubClient.h>  // LIbreria para Mqtt

//*** Credenciales de la red *************
#define WIFISSID "Danner"                                 // Nombre de la red wifi
#define PASSWORD "123456789"                            // Contraseña de la red wifi
#define TOKEN "BBFF-9iT1pQeyewhlUfNr82E507sMbPN1TQ"    // Token de Ubidots
#define MQTT_CLIENT_NAME "esp32-Grupo8"                // Nombre del dispositivo Mqtt

/****************************************
 * Definición de constantes
 ****************************************/
#define VARIABLE_LABEL "temp"     // Nombre de la variable
#define VARIABLE_LABEL_PUERTA_1 "puerta_1"     // Nombre de la variable
#define DEVICE_LABEL "esp32-maquetaG8"  // Nombre del dispositivo en ubidots
#define SENSOR 35                 // Set the GPIO35 as SENSOR
#define LED_1 13                // GPIO13
#define LED_2 26                  // GPIO26
#define LED_3 27                  // GPIO27
#define LED_4 32                  // GPIO32
#define VENTILADOR 15              // GPIO15
#define PUERTA_1 5               // GPIO5



char mqttBroker[] = "industrial.api.ubidots.com";
char payload[100];
char topic[150];
char topicSubscribe[150];
char str_sensor[10];
char str_puerta_1[10];



// Variable de timer usando millis()
unsigned long lastTime = 0;
unsigned long timerDelay = 2000;

/****************************************
 * Funciones Auxiliares
 ****************************************/
WiFiClient ubidots;
PubSubClient client(ubidots);

// Función para leer un dato que llega desde Ubidots al ESP32
void callback(char* topic, byte* payload, unsigned int length) {


  Serial.println(strstr(topic, "led_1"));

  if (strstr(topic, "led_1") != NULL) {
    if ((char)payload[0] == '1') {
      digitalWrite(LED_1, HIGH);  // Encendio de led

    } else if ((char)payload[0] == '0') {
      digitalWrite(LED_1, LOW);  // Apagado de led
    }
  } else if (strstr(topic, "led_2") != NULL) {
    if ((char)payload[0] == '1') {
      digitalWrite(LED_2, HIGH);  // Encendio de led

    } else if ((char)payload[0] == '0') {
      digitalWrite(LED_2, LOW);  // Apagado de led
    }
  } else if (strstr(topic, "led_3") != NULL) {
    if ((char)payload[0] == '1') {
      digitalWrite(LED_3, HIGH);  // Encendio de led

    } else if ((char)payload[0] == '0') {
      digitalWrite(LED_3, LOW);  // Apagado de led
    }
  } else if (strstr(topic, "led_4") != NULL) {
    if ((char)payload[0] == '1') {
      digitalWrite(LED_4, HIGH);  // Encendio de led

    } else if ((char)payload[0] == '0') {
      digitalWrite(LED_4, LOW);  // Apagado de led
    }
  } 
  
  Serial.println();

  // Encender diodo led
}

void reconnect() {

  // Loop para reconexión MQTT
  while (!client.connected()) {
    Serial.println("Intentando conectarse...");

    // Conectandose
    if (client.connect(MQTT_CLIENT_NAME, TOKEN, "")) {
      Serial.println("Conectado");
      sprintf(topicSubscribe, "/v1.6/devices/%s/%s/lv", DEVICE_LABEL, "LED_1");
      client.subscribe(topicSubscribe);  // Creando topic de suscripción
      sprintf(topicSubscribe, "/v1.6/devices/%s/%s/lv", DEVICE_LABEL, "LED_2");
      client.subscribe(topicSubscribe);
      sprintf(topicSubscribe, "/v1.6/devices/%s/%s/lv", DEVICE_LABEL, "LED_3");
      client.subscribe(topicSubscribe);
      sprintf(topicSubscribe, "/v1.6/devices/%s/%s/lv", DEVICE_LABEL, "LED_4");
      client.subscribe(topicSubscribe);
    
    } else {
      Serial.print("Falla, rc=");
      Serial.print(client.state());
      Serial.println(" Espera 2 segundos");
      delay(2000);
    }
  }
}

/****************************************
 * Funciones principales
 ****************************************/
void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFISSID, PASSWORD);
  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
  pinMode(LED_3, OUTPUT);
  pinMode(LED_4, OUTPUT);


  Serial.println();
  Serial.print("Conectandose a la red Wifi...");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi Conectado");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());     // Mostrando la IP asignada

  client.setServer(mqttBroker, 1883);  // Conectando a servidor Ubidots por MQTT
  client.setCallback(callback);        // LLamada a función callback
}

void loop() {

  // Si pierde conexión , vuelve a conestarse
  if (!client.connected()) {
    reconnect();
  }

  if ((millis() - lastTime) > timerDelay) {
    lastTime = millis();
    sprintf(topic, "%s%s", "/v1.6/devices/", DEVICE_LABEL);
    sprintf(payload, "%s", "");                              // Cleans the payload
    sprintf(payload, "{\"%s\":", VARIABLE_LABEL);           // Agregar label de variable

    // Lectura de temperatura
    float sensor = 500.0 * analogRead( SENSOR ) / 1023.0; 
    if(sensor> 40.0){
      digitalWrite(VENTILADOR,HIGH); 
    }
    else{
      digitalWrite(VENTILADOR,LOW);
    }
    
    Serial.println(sensor);
    dtostrf(sensor, 4, 0, str_sensor);

  

    

    sprintf(payload, "%s {\"value\": %s}}", payload, str_sensor);    // Agregar valor
    Serial.println("Publicando el dato en Ubidots");
    client.publish(topic, payload);                                 // Publicando el valor del sensor

    sprintf(topic, "%s%s", "/v1.6/devices/", DEVICE_LABEL);
    sprintf(payload, "%s", "");                              // Cleans the payload
    sprintf(payload, "{\"%s\":", VARIABLE_LABEL_PUERTA_1);    // Agregar label de variable

    int puerta_1 = analogRead( PUERTA_1 );
    Serial.println(puerta_1);
    dtostrf(puerta_1, 1, 0, str_puerta_1);

    sprintf(payload, "%s {\"value\": %s}}", payload, str_puerta_1);    // Agregar valor
    Serial.println("Publicando el dato en Ubidots");
    client.publish(topic, payload);

    sprintf(topic, "%s%s", "/v1.6/devices/", DEVICE_LABEL);
    sprintf(payload, "%s", "");                              // Cleans the payload
    sprintf(payload, "{\"%s\":", VARIABLE_LABEL_PUERTA_2);    

    int puerta_2 = analogRead( PUERTA_2 );
    Serial.println(puerta_2);
    dtostrf(puerta_2, 1, 0, str_puerta_1);

    sprintf(payload, "%s {\"value\": %s}}", payload, str_puerta_2);    // Agregar valor
    Serial.println("Publicando el dato en Ubidots");
    client.publish(topic, payload);      
  }

  client.loop();
}
