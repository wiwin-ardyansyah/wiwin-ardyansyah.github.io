#include <Arduino.h>
#include <wifi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <ESP32httpUpdate.h>

#ifndef WIFIID
#define WIFIID "Minta Wifi Donk..!"
#define PASSID  "wiwin301287"
//#define WIFIID "Indomo-5G"
//#define PASSID  "s@tr10*328"
//#define WIFIID "Syarif Siddiq"
//#define PASSID  "mimiwin87"
#endif

#define FIRMWARE_VERSION 1.0
#define UPDATE_URL "https://wiwin-ardyansyah.github.io/firmware.json"



void setup(){
Serial.begin(115200);
connWifi();
checkUpdate()
}

void loop(){
checkUpdate();
delay(10000);
}

void connWifi(){
Serial.println("Sedang koneksi ke Wifi");
Wifi.begin(WIFIID,PASSID);
	while(Wifi.status()!=WL_CONNECTED){
		Serial.print(".");
		delay(500);	
	}
Serial.println("Perangkat sudah terkoneksi dengan Wifi");
Serial.println(Wifi.SSID());
Serial.println(Wifi.RSSI());
Serial.println(Wifi.macAddress());
Serial.println(Wifi.localIP());
Serial.println(Wifi.gatewayIP());
Serial.println(Wifi.dnsIP());
}

void checkUpdate(){
Serial.println("Checking Firmware Update on The Server");
HTTPClient http;
String res;
String url = UPDATE_URL;
http.begin(url);
http.GET();

res = http.getString();
Serial.println(res);

StaticJsonDocument<1024> doc;
deserializeJson(doc,res);
JsonObject obj = doc.as<JsonObject>();

String version = obj[String("version")];
String url_update = obj[String("url")];

Serial.println(version);
Serial.println(url_update);

if(version.toDouble()>FIRMWARE_VERSION){
	Serial.println("Tersedia Update Version Terbaru");	
	}else {
	Serial.println("Versi %s masih yang terbaru", version);
	}


}
