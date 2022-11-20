#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

const char* ssid = "Heraclio's Galaxy M32";
const char* password = "Konectar";
const char* mqttServer = "node02.myqtthub.com";
const int mqttPort = 1883;
const char* mqttUser = "heracliofuente25";
const char* mqttPassword = "4lghMlp9-wHpehvqa";

int tempMax = 30;
int dhtPort = 8;
int ventiladorPort = 2;
int ledAmarilloPort = 0;
int ledverdePort = 0;
int ledRojoPort = 0;
int ledCalefaccionPort = 0;
int puertaPort = 0;


const char* TOPIC_HUMEDAD = "casa/humedad";
const char* TOPIC_PUERTA = "casa/puerta";
const char* TOPIC_TEMPERATURA = "casa/temperatura";
const char* TOPIC_TEMPERATURA_MAX = "casa/temperatura-max";

void callback(char* topic, byte* payload, unsigned int length);

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("...................................");

  Serial.print("Connecting to WiFi.");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to the WiFi network");
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  // Inicia Sensor Temperatura
  dht.begin();

  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect("ESP32Client", mqttUser, mqttPassword)) {
      Serial.println("connected");

      connectTopics();

    } else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
}



void connectTopics() {
  // Nos suscribimos
  if (client.subscribe(TOPIC_TEMPERATURA)) {
    Serial.println("Suscripcion a topic " + String(TOPIC_TEMPERATURA));
  } else {
    Serial.println("fallo Suscripciión a topic " + String(TOPIC_TEMPERATURA));
  }

  if (client.subscribe(TOPIC_TEMPERATURA_MAX)) {
    Serial.println("Suscripcion a topic " + String(TOPIC_TEMPERATURA_MAX));
  } else {
    Serial.println("fallo Suscripciión a topic " + String(TOPIC_TEMPERATURA_MAX));
  }

  if (client.subscribe(TOPIC_HUMEDAD)) {
    Serial.println("Suscripcion a topic " + String(TOPIC_HUMEDAD));
  } else {
    Serial.println("fallo Suscripciión a topic " + String(TOPIC_HUMEDAD));
  }

  if (client.subscribe(TOPIC_PUERTA)) {
    Serial.println("Suscripcion a topic " + String(TOPIC_PUERTA));
  } else {
    Serial.println("fallo Suscripciión a topic " + String(TOPIC_PUERTA));
  }
}

void indicadoresLed(String temp) {
  int tempInt = temp.toInt();
  int rango = 2;

  if (tempInt >= tempMax - rango && tempInt < tempMax) {
    digitalWrite(ledverdePort, HIGH);
    digitalWrite(ledRojoPort, LOW);
    digitalWrite(ledAmarilloPort, LOW);
    //    Apagamos Calefacción
    digitalWrite(ledCalefaccionPort, LOW);
  }
  if (tempInt < tempMax - rango) {
    digitalWrite(ledAmarilloPort, HIGH);
    digitalWrite(ledverdePort, LOW);
    digitalWrite(ledRojoPort, LOW);

    //    Encendemos Calefacción
    digitalWrite(ledCalefaccionPort, HIGH);
  }

  if (tempInt = > tempMax) {
    digitalWrite(ledRojoPort, HIGH);
    digitalWrite(ledverdePort, LOW);
    digitalWrite(ledAmarilloPort, LOW);
  }
}

void getTemperature() {
  float humedad = dht.readHumidity();
  float temp = dht.readTemperature();
  Serial.print("Temperatura: ");
  Serial.print(temp);
  Serial.print("ºC Humedad: ");
  Serial.print(humedad);
  Serial.println("%");
  delay(1000);

  // Comprobamos si ha habido algún error en la lectura
  if (isnan(humedad) || isnan(temp)) {
    Serial.println("Error obteniendo los datos del sensor DHT11");
    return;
  } else {

    // Encender/Apagar LEDs
    indicadoresLed(String(temp));

    // Enviar la temp al MQTT
    sendTemperature(String(temp), String(humedad));
  }

  if (temp >= tempMax) {
    // Encender ventilador
    digitalWrite(ventiladorPort, HIGH);
  } else {
    // Apagar Ventilador
    digitalWrite(ventiladorPort, LOW);
  }

  getMaxTemp();
}


void sendTemperature(String temp, String hum) {
  client.publish(TOPIC_HUMEDAD, hum);
  client.publish(TOPIC_TEMPERATURA, temp);
}


void callback(char* topic, byte* payload, unsigned int length) {
  String incoming = "";
  Serial.print("Mensaje recibido desde -> ");
  Serial.print(topic);

  Serial.println("");
  for (int i = 0; i < length; i++) {
    incoming += (char)payload[i];
  }
  incoming.trim();
  Serial.println("Mensaje -> " + incoming);

  // Temp Max
  if (topic == TOPIC_TEMPERATURA_MAX) {
    tempMax = incoming.toInt();
  }

  // Puerta!!!
  if (topic == TOPIC_PUERTA) {
    if (incoming == 'open') {
      Serial.println('Se Abrió la puerta.');
      digitalWrite(puertaPort, HIGH);
      delay(1000);
      digitalWrite(puertaPort, LOW);
    }
    if (incoming == 'close') {
      digitalWrite(puertaPort, HIGH);
      delay(1000);
      digitalWrite(puertaPort, LOW);
      client.publish(TOPIC_PUERTA, 'closed');
      Serial.println('Se Cerró la puerta.');
    }
  }
}

void loop() {
  getTemperature();
  client.loop();
  delay(500);
}
