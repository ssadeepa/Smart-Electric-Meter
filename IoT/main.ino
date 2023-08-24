#include <WiFi.h>
#include <PubSubClient.h>

#include <ArduinoJson.h>
#include <UniversalTelegramBot.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

#define BOTtokenValue "6605990724:AAFCmdHqQUJP6_uEYMiKHeSNG7uD8KwRl18"
#define OUR_CHAT_ID "1142672061"
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtokenValue, client);

#define LED 22
const byte CURRENT_GPIO = 36;
const byte VOLTAGE_GPIO = 39;
char currentAr[6];
char voltageAr[6];
#define RXp2 16
#define TXp2 17
const int bufferSize = 9;  // Adjust the buffer size as needed
//const int bufferSize = 15;
char receivedData[bufferSize];
char firstHalf[5];  // +1 for the null terminator
char secondHalf[5];
int integerValue;
int unitReset = 0;
int firstHalfInteger;
int secondHalfInteger;
int power = 0;
int volatgeCheck = 0;
//char receivedDataArray[9];


//uint8_t incomingByte = 0;


WiFiClient espClient;
PubSubClient mqttClient(espClient);

void setup() {
  // put your setup code here, to run once:
  delay(1000);
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  delay(10);
  setupWiFi();
  setupMqtt();
  Serial2.begin(9600, SERIAL_8N1, RXp2, TXp2);
   Serial.print("receivedDataNUONHU");
 
}

void loop() {
  // put your main code here, to run repeatedly:
  // Serial.println("H");
  if (!mqttClient.connected()) {
    connectToBroker();
    // Serial.println("B");
  }

  mqttClient.loop();

  // updateCurrent();
  // Serial.print("currentAr: ");
  // Serial.println(currentAr);
  // mqttClient.publish("ELECTRIC-METER-CURRENT", currentAr);  // Publish

  // updateVoltage();
  // Serial.print("voltageAr: ");
  // Serial.println(voltageAr);
  // mqttClient.publish("ELECTRIC-METER-VOLTAGE", voltageAr); // Publish
  // delay(1000);

  
  updateUART();
  //String receivedString = Serial2.readString(); 
  // Serial.print("receivedDataNow: ");
  // Serial.println(receivedData);
  if ( firstHalfInteger > 200) {
    bot.sendMessage(OUR_CHAT_ID, " Voltage Limit Exceeds", "");
    volatgeCheck = 1;
  } 

  if (integerValue > power & unitReset == 0) {
    bot.sendMessage(OUR_CHAT_ID, " Unit Limit Exceeds", "");
    unitReset = 1;
  } 
  separateArray();

  Serial.print("firstHalf: ");
  Serial.println(firstHalf);
  mqttClient.publish("ELECTRIC-METER-CURRENT", firstHalf);  // Publish

  Serial.print("secondHalf: ");
  Serial.println(secondHalf);
  mqttClient.publish("ELECTRIC-METER-VOLTAGE", secondHalf);  // Publish
  Serial.print("Power: ");
  firstHalfInteger = atoi(firstHalf); 
  secondHalfInteger = atoi(secondHalf);
  power = power + ((firstHalfInteger * secondHalfInteger)/1000);
  Serial.println(power);
  delay(1000);
}
