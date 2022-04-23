//============================================ LIBRARY ==============================
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <ESP32httpUpdate.h>
//============================================ END LIBRARY ==============================

//============================================ VARIABLE GLOBAL ======================
#ifndef WIFIID
// #define WIFIID "Minta Wifi Donk..!"
// #define PASSID  "wiwin301287"
//#define WIFIID "Indomo-5G"
//#define PASSID  "s@tr10*328"
#define WIFIID "Syarif Siddiq"
#define PASSID  "mimiwin87"
#endif
#define FIRMWARE_VERSION 1.1
#define UPDATE_URL "https://wiwin-ardyansyah.github.io/firmware.json"
const char* mqtt_server = "broker.mqtt-dashboard.com";

unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;
//============================================ END VARIABLE GLOBAL =======================

//============================================ FUNCTION GLOBAL =======================
WiFiClient espClient;
PubSubClient client(espClient);
void connWifi();
void checkUpdate();
t_httpUpdate_return updateFirmware(String url_update);
//============================================ END FUNCTION GLOBAL =======================

//============================================ SETUP ================================
void setup() {
  Serial.begin(115200);
  pinMode(BUILTIN_LED, OUTPUT);
  connWifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}
//============================================ END SETUP ================================

//============================================ LOOP =================================
void loop() {
  checkUpdate();

}
//============================================ END LOOP =================================

//============================================ SETUP WIFI ============================
void connWifi() {
  Serial.println("Sedang koneksi ke Wifi");
  WiFi.begin(WIFIID, PASSID);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("Perangkat sudah terkoneksi dengan Wifi");
  Serial.println(WiFi.SSID());
  Serial.println(WiFi.RSSI());
  Serial.println(WiFi.macAddress());
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.gatewayIP());
  Serial.println(WiFi.dnsIP());
}
//============================================ END SETUP WIFI ============================

void checkUpdate() {
  Serial.println("Checking Firmware Update on The Server \n\n");
  //  WiFiClientSecure client;
  //  client.setInsecure();
  HTTPClient http;
  String res;
  String url = UPDATE_URL;
  http.begin(url);
  http.GET();

  res = http.getString();


  StaticJsonDocument<1024> doc;
  deserializeJson(doc, res);
  JsonObject obj = doc.as<JsonObject>();

  String version = obj[String("version")];
  String url_update = obj[String("url")];

  Serial.printf("%.1f \n", version.toFloat());
  Serial.println(url_update);

  http.end();



  if (version.toDouble() > FIRMWARE_VERSION) {
    Serial.printf("Tersedia Update Version Terbaru ver.%.1f \n", version.toDouble());
    if (updateFirmware(url_update) == HTTP_UPDATE_OK) ESP.restart();
  } else {
    Serial.printf("Versi Firmware ini: ver.%.1f saat ini masih yang terbaru \n", version.toDouble());
  }

}



t_httpUpdate_return updateFirmware(String url_update) {
  t_httpUpdate_return rel;
  //  client.setInsecure();
  if ((WiFi.status() != WL_CONNECTED)) {
    Serial.print(".");
    delay(500);
  } else {
    rel = ESPhttpUpdate.update(url_update);
    switch (rel)
    {
      case HTTP_UPDATE_OK:
        Serial.print("Update Firmware Berhasil.. \n\n");
        return rel;
        break;
      case HTTP_UPDATE_NO_UPDATES:
        Serial.print("Tidak ada Update Firmware untuk saat ini.. \n\n");
        return rel;
        break;
      case HTTP_UPDATE_FAILED:
        Serial.print("Update Firmware gagal...! \n\n");
        return rel;
        break;
    }
  }
}

//============================================ MQTT CALLBACK ========================
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, HIGH);   // Turn the LED on (Note that LOW is the voltage level
    client.publish("wiwinardiansyah@gmail.com/rumah/lampu1", "1");
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, LOW);  // Turn the LED off by making the voltage HIGH
    client.publish("wiwinardiansyah@gmail.com/rumah/lampu1", "0");
  }

}
//============================================ END MQTT CALLBACK ========================

//============================================ KONEKSI MQTT =========================
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "Data-Unique = ";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect("wiwinardyansyah@gmai.com")) {
      Serial.println("connected");
      // ... and resubscribe
      client.subscribe("wiwinardiansyah@gmail.com/rumah2/lampu1");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
//============================================ END KONEKSI MQTT =========================
