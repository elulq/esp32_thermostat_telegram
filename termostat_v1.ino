#include <WiFi.h>
#include <WiFiClientSecure.h> 
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include "ATC_MiThermometer.h"

// Wifi Baglantisi
const char* ssid = "***************";
const char* password = "*********";

const int scanTime = 5; // BLE scan time in seconds
const int relay = 12;
String roleDurum = "KAPALI";
bool sistemAcik = false;
//Checks for new messages every 1 second.
int botRequestDelay = 1000;
unsigned long lastTimeBotRan;

// List of known sensors' BLE addresses, multiple device supported
std::vector<std::string> knownBLEAddresses = {"A4:C1:38:CD:3C:B0"};

ATC_MiThermometer miThermometer(knownBLEAddresses);

// Iteration counter
int iteration = 0;

float maxTemp = 22.5;
float minTemp = 21.5;
float temp = 22;
float humi = 0;
float volt = 0;
float bat =  0;
float rssi = 0;
String text = "/bilgi";
String sistem = "kapali";

// Telegram BOT için gereken token'ı girin
#define BOTtoken "***********************************"  // BotFather TOKEN

// IDBOT üzerinden /getid mesajı ile aldığımız ID bilgimizi gireceğiz
// Botun bizimle haberleşebilmesi için /start komutunu çalıştırmamız gerekli
#define CHAT_ID "************"

//telegram bota bağlantı için gerekli tanımlamaları yapmalıyız
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);


void setup() {
  Serial.begin(115200);
  pinMode(relay, OUTPUT);
  digitalWrite(relay, HIGH);

  delay(1000);
  // Kablosuz Ağa bağlanıyoruz
  Serial.print("Kablosuz Ağa Bağlanıyor: ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  //bağlanana kadar bekletiyoruz
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("Ağa Bağlanıldı");
  Serial.print("IP adresi: ");
  Serial.println(WiFi.localIP());
  client.setInsecure();

  // Print free heap before initialization
  Serial.println("Starting:    Free heap is " + String(ESP.getFreeHeap()));
    
  // Initialization
  miThermometer.begin();
    
  // Print free heap after initialization
  Serial.println("Initialized: Free heap is " + String(ESP.getFreeHeap()));

  //sistemin çalışmaya başladığını bot'a mesaj göndererek bildiriyoruz
  bot.sendMessage(CHAT_ID, "Bot Başlatıldı", "");
  bot.sendMessage(CHAT_ID, "Sistem Kapali, aktif etmek icin /aktif komutunu kullanin.", "");

}

String getReadings(){


    // Set sensor data invalid
  miThermometer.resetData();
    
    // Get sensor data - run BLE scan for <scanTime>
  unsigned found = miThermometer.getData(scanTime);

  for (int i=0; i < miThermometer.data.size(); i++) {  
      if (miThermometer.data[i].valid) {
//          Serial.println();
//          Serial.printf("Sensor %d: %s\n", i, knownBLEAddresses[i].c_str());
//          Serial.printf("%.2f°C\n", miThermometer.data[i].temperature/100.0);
//          Serial.printf("%.2f%%\n", miThermometer.data[i].humidity/100.0);
//          Serial.printf("%d%%\n",   miThermometer.data[i].batt_level);
//          Serial.println();
          temp = miThermometer.data[i].temperature/100.0;
          humi = miThermometer.data[i].humidity/100.0;
          bat =  miThermometer.data[i].batt_level;
        }
  }
  String message = "Sicaklik: " + String(temp) + " ºC \n";
  message += "Nem: " + String (humi) + " % \n";
  message += "Pil Durum: " + String (bat) + " % \n";
  message += "Kazan Durum: " + roleDurum + "\n";

  return message;
}



//Handle what happens when you receive new messages
void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i=0; i<numNewMessages; i++) {
    // Chat id of the requester
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID){
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }
    
    // Print the received message
    text = bot.messages[i].text;
    Serial.println(text);

    String from_name = bot.messages[i].from_name;

    if (text == "/bilgi") {
      String welcome = "Merhaba, " + from_name + ".\n";
      welcome += "Guncel olcumleri getirmek icin /olc komutunu kullanabilirsiniz.\n\n";
      welcome += "Sistemi kapatmak icin /kapali komutunu kullanabilirsiniz.\n\n";
      welcome += "Sistemi aktiflestirmek icin /aktif komutunu kullanabilirsiniz.\n\n";
      welcome += "Maksimum degeri degistirmek icin /max komutunu kullanabilirsiniz.\n\n";
      welcome += "Su anki Maksimum deger =" + String(maxTemp) + "\n\n";
      welcome += "Minimum degeri degistirmek icin /min komutunu kullanabilirsiniz.\n\n";
      welcome += "Su anki Minimum deger =" + String(minTemp) + "\n\n";
      welcome += "Sistem durumu =" + sistem + "\n\n";

      bot.sendMessage(chat_id, welcome, "");
    }

    if (text == "/olc") {
      String readings = getReadings();
      bot.sendMessage(chat_id, readings, "");
    } 
    if (text == "/kapali") {
      sistemAcik = false;
      String readings = getReadings();
      bot.sendMessage(CHAT_ID, "Sistem Kapali, aktif etmek icin /aktif komutunu kullanin.", "");
      bot.sendMessage(chat_id, readings, "");
    }
    if (text == "/aktif") {
      sistemAcik = true;
      bot.sendMessage(CHAT_ID, "Sistem Aktif, kapatmak icin /kapali komutunu kullanin.", "");
      String readings = getReadings();
      bot.sendMessage(chat_id, readings, "");
    }
    if (text.startsWith("/max ")) {
      String maxStr = text.substring(5);
      maxTemp = maxStr.toFloat();
      String messageTemp = "Maksimum deger guncellendi: " + String(maxTemp);
      bot.sendMessage(chat_id, messageTemp, "");
    }  
    if (text.startsWith("/min ")) {
      String maxStr = text.substring(5);
      minTemp = maxStr.toFloat();
      String messageTemp = "Minimum deger guncellendi: " + String(minTemp);
      bot.sendMessage(chat_id, messageTemp, "");
    }  
  }
}



void loop() {


    // Set sensor data invalid
  miThermometer.resetData();
    
    // Get sensor data - run BLE scan for <scanTime>
  unsigned found = miThermometer.getData(scanTime);
  for (int i=0; i < miThermometer.data.size(); i++) {  
      if (miThermometer.data[i].valid) {
          Serial.println();
          Serial.printf("Sensor %d: %s\n", i, knownBLEAddresses[i].c_str());
          Serial.printf("%.2f°C\n", miThermometer.data[i].temperature/100.0);
          Serial.printf("%.2f%%\n", miThermometer.data[i].humidity/100.0);
          Serial.printf("%d%%\n",   miThermometer.data[i].batt_level);
          Serial.printf("%ddBm\n",  miThermometer.data[i].rssi);
          Serial.println();
          temp = miThermometer.data[i].temperature/100.0;
          humi = miThermometer.data[i].humidity/100.0;
          volt = miThermometer.data[i].batt_voltage/1000.0;
          bat =  miThermometer.data[i].batt_level;
          rssi = miThermometer.data[i].rssi;
        }
  }

//  bot.sendMessage(CHAT_ID, "Sicaklik: " + String(temp), "");
  Serial.println("BLE Devices found (total): " + String(found));
  
  delay(500);

if ( sistemAcik == true ) {
  sistem = "acik";

    if (temp < minTemp) {
        
        delay(500);
        if(roleDurum == "KAPALI"){
          bot.sendMessage(CHAT_ID, "Kazan Acildi.", "");
        }
        digitalWrite(relay, LOW);
        roleDurum= "ACIK";
        
        //bot.sendMessage(CHAT_ID, "Kazan Acildi.", "");
        Serial.println("role açık");
      } else if (temp > maxTemp) {
        
        if(roleDurum == "ACIK"){
          delay(500);
          bot.sendMessage(CHAT_ID, "Kazan Kapatiliyor.", "");
        }
        digitalWrite(relay, HIGH);
        roleDurum= "KAPALI";
        //bot.sendMessage(CHAT_ID, "Kazan Kapatiliyor.", "");
        Serial.println("role kapali");
      } 

} else {
  sistem = "kapali";
  digitalWrite(relay, HIGH);
  roleDurum= "KAPALI";

}


    // Delete results from BLEScan buffer to release memory
    miThermometer.clearScanResults();
    
    // Print iteration counter and free heap
    Serial.println("Iteration " + String(iteration) + " - Free heap is " + String(ESP.getFreeHeap()));
    Serial.println("---");

  if (millis() > lastTimeBotRan + botRequestDelay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while(numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }

    delay(5000);
}

