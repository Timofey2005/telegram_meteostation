#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h> 
#include <ArduinoJson.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Wire.h>

Adafruit_BME280 bme;

// указать в кавычках название WiFi сети и её пароль
const char* ssid = "Redmi tima";
const char* password = "00000000";

volatile bool MESSAGE_FLAG = false;

#define BOTtoken "1492669289:AAFyv6WSEKo9u-2bCLpjV1IK7yKnRENbtCg"
#define CHAT_ID "1585435736"
#define TIMER_DELAY 60000 // Время повтора

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

int botRequestDelay = 1000;
unsigned long lastTimeBotRan;

unsigned long myTime = 0;

void handleNewMessages(int numNewMessages){
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));
  
   for (int i=0; i<numNewMessages; i++) {
    // Chat id of the requester
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID){
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }
    String text = bot.messages[i].text;
    Serial.println(text);
    
    String from_name = bot.messages[i].from_name;

    if (text == "/info") {
      String welcome = "Welcome, " + from_name + ".\n";
      welcome += "Use the following commands to control your outputs.\n\n";
      welcome += "/start to start measuring every 60 seconds \n";
      welcome += "/stop to stop measuring every 60 seconds \n";
 
      bot.sendMessage(chat_id, welcome, "");
    }

    if(text == "/start"){
      bot.sendMessage(chat_id, "Start all measuring", "");
      MESSAGE_FLAG = true;
    }
    
    if(text == "/stop"){
      bot.sendMessage(chat_id, "Stop all measuring", "");
      MESSAGE_FLAG = false;
    }
    
  }
}

void setup() {
  Serial.begin(115200);
  #ifdef ESP8266
    client.setInsecure();
  #endif

  // WiFi.mode(WIFI_STA) - как клиент 
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  bme.begin(0x76);
  delay(3000);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  Serial.println(WiFi.localIP());

  bool myStatus;
  
  Serial.println("-- Default Test --");

  delay(100);
}

void loop() {
  if (millis() > lastTimeBotRan + botRequestDelay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    
    while(numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }

  if((millis() - myTime > TIMER_DELAY) && (MESSAGE_FLAG == true)){
    String message = "Temperature: " + String(bme.readTemperature()) + " degrees \n";
    message += "Pressure: " + String(bme.readPressure() / 100.0F) + " hPa \n";
    message += "Humidity: " + String(bme.readHumidity()) + " % \n";
    bot.sendMessage(CHAT_ID, message, "");
    myTime = millis();
  }
}
