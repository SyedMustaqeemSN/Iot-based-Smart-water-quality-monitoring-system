#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

//wifi credentials
const char* ssid = "CO"; 
const char* password = "12345678";

// telegram bot token and chat id
#define BOTtoken "6835885956:AAHjx9BoTP22nAeXCvMU2Zwg9MNgygBD8Bk"  //bot token from botfather
#define CHAT_ID "6216175129"  //chat id to send msg

// certificate for telegram
X509List cert(TELEGRAM_CERTIFICATE_ROOT);

// secure client and bot object
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

void setup() {
  Serial.begin(9600);  //for checking serial data

  configTime(0, 0, "pool.ntp.org", "time.nist.gov");  //to get correct time for cert check
  client.setTrustAnchors(&cert);  //setting telegram cert

  WiFi.begin(ssid, password);  //connecting to wifi
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");  //printing dots until it gets connected
  }

  Serial.println("\nWiFi connected");  //wifi connected msg
  bot.sendMessage(CHAT_ID, "ESP32 Bot started successfully ", "");  //telegram msg when bot starts
}

void loop() {
  if (Serial.available()) {
    String bvalue = Serial.readStringUntil('\n');  //reading data from serial until new line
    Serial.println(bvalue);  //printing that data in serial monitor
    bot.sendMessage(CHAT_ID, bvalue);  //sending that data to telegram
    delay(1000);  //delay so that bot doesn’t send msg continuously
  }
}
