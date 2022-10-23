#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

//**************
//**** MQTT CONFIG *****
//**************
const char *mqtt_server = "node02.myqtthub.com";
const int mqtt_port = 1883;
const char *mqtt_user = "32";
const char *mqtt_pass = "32";
const char *root_topic_subscribe = "Temperatura/esp32";
const char *root_topic_publish = "Temperatura/public_esp32_02";

//**************
//**** WIFICONFIG ******
//**************
const char *ssid = "UNISANGIL YOPAL";
const char *password = "";

//**************
//**** GLOBALES   ******
//**************
WiFiClient espClient;
PubSubClient client(espClient);
char msg[25];
long count = 0;

//********
//* F U N C I O N E S **
//********
void callback(char *topic, byte *payload, unsigned int length); // METODO ENCARGADO DE RECIBIR DATOS
void reconnect();                                               // conectar al broker
void setup_wifi();                                              // conectar al wifi

void setup()
{
  Serial.begin(115200); // configurar el chip en consola para que funcione a esta velocidad
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port); //
  client.setCallback(callback);
}

void loop()
{

  if (!client.connected())
  {
    reconnect();
  }

  if (client.connected())
  {
    String str = "La cuenta es -> " + String(count);
    str.toCharArray(msg, 25);
    client.publish(root_topic_publish, msg);
    count++;
    Serial.println(msg);
    delay(10000); // tiempo para envio de mensaje (cada 10 segundos (10.000 milisegundos))
  }
  client.loop();
}

//***********
//*    CONEXION WIFI      *
//***********
void setup_wifi()
{
  delay(5000);
  // Nos conectamos a nuestra red Wifi
  Serial.println();
  Serial.print("Conectando a ssid: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Conectado a red WiFi!");
  Serial.println("Dirección IP: ");
  Serial.println(WiFi.localIP());
}

//***********
//*    CONEXION MQTT      *
//***********

void reconnect()
{

  while (!client.connected())
  {
    Serial.print("Intentando conexión Broker...");
    // Creamos un cliente ID
    String clientId = "ESP_32_02"; // este es el nombre que  se debe dejar en el broker

    // Intentamos conectar
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_pass))
    {
      Serial.println("Conectado al broker!");
      // Nos suscribimos
      if (client.subscribe(root_topic_subscribe))
      {
        Serial.println("Suscripcion a topic " + String(root_topic_subscribe));
      }
      else
      {
        Serial.println("fallo Suscripciión a topic " + String(root_topic_subscribe));
      }
    }
    else
    {
      Serial.print("falló conexión broker:( con error -> ");
      Serial.print(client.state());
      Serial.println(" Intentamos de nuevo en 5 segundos");
      delay(5000);
    }
  }
}

//***********
//*       CALLBACK        *
//***********

void callback(char *topic, byte *payload, unsigned int length)
{
  String incoming = "";
  Serial.print("Mensaje recibido desde -> ");
  Serial.print(topic);
  Serial.println("");
  for (int i = 0; i < length; i++)
  {
    incoming += (char)payload[i];
  }
  incoming.trim();
  Serial.println("Mensaje -> " + incoming);
}
