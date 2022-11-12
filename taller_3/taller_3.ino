#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <DHTesp.h>
#include <stdlib.h>

//**************
//**** MQTT BROKER CONFIG *****
//**************
const char *mqtt_server = "node02.myqtthub.com";
const int mqtt_port = 1883;
const char *mqtt_user = "heracliofuente25";
const char *mqtt_pass = "4lghMlp9-wHpehvqa";
const char *root_topic_subscribe = "temperatura";
const char *root_topic_publish = "temperatura";

const char *root_topic_subscribe_modificaTemperatura = "temperatura";
const char *root_topic_subscribe_puerta = "puerta";
const char *root_topic_publish_temperatura = "temperatura";
const char *root_topic_publish_data = "esp32_02";
const char *root_topic_publish_humedad = "humedad";

//    **************
//**** EMAIL CONFIG *****
//    **************
#define emailSenderAcount "iotdecimo@gmail.com"
#define emailSenderPaswrd "123abc..."
#define emailRecipient    "iotdecimo@gmail.com"
#define smtpServerPort    465

String emailSubject = "ESP32 inicio de trabajo ";
String emailBodyMessage = "<div style=\"color:#2f4468;\">
<h1>mensaje de prueba en configuracion inicial</h1>" ;


//   **************
//**** WIFICONFIG ******
//   **************
const char *ssid = "Heraclio's Galaxy M32";
const char *password = "Konectar";

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

//      ***********
//*    CONEXION WIFI      *
//      ***********
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

//      ***********
//*    CONEXION MQTT      *
//      ***********

void reconnect()
{
  while (!client.connected())
  {
    Serial.print("Intentando conexión Broker...");
    // Creamos un cliente ID
    String clientId = "heracliofuente25"; // este es el nombre que  se debe dejar en el broker
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
      Serial.print("falló conexión broker: con error -> ");
      Serial.print(client.state());
      Serial.println(" Intentamos de nuevo en 5 segundos");
      delay(5000);
    }
  }
}

//       ***********
//*       CALLBACK        *
//      ***********

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

//       ***********
//*       SEND MAIL        *
//      ***********

/*
  void readModifyTemp () {
  Serial.println(topic);

  }
  void correo(){
  digitalWrite(23, HIGH);

  datosSMTP.setLogin("smtp.gmail.com", 465, "iotdecimo@gmail.com", "123abc...");
  datosSMTP.setSender("ESP32S", "iotdecimo@gmail.com");
  datosSMTP.setPriority("High");

  datosSMTP.setSubject("Probando envio de correo con ESP32");// Establecer el asunto
  // Establece el mensaje de correo electrónico en formato de texto (sin formato)
  datosSMTP.setMessage("Hola soy el esp32s! y me estoy comunicando contigo", false);
  datosSMTP.addRecipient("iotdecimo@gmail.com"); // destinatario del correo

  if (!MailClient.sendMail(datosSMTP)) //enviar correo electrónico.
  Serial.println("Error enviando el correo, " + MailClient.smtpErrorReason());
  datosSMTP.empty();//liberar memoria
  delay(10000);
  digitalWrite(23, LOW);
*/
