void setupMqtt() {
  mqttClient.setServer("test.mosquitto.org", 1883);
  mqttClient.setCallback(receiveCallback);
}

void receiveCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived: ");
  Serial.print(topic);
  Serial.print("---");

  char payloadCharAr[length];

  // Converting byte array into char array using for loop
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);  // bytes cannot print so we need to convert them to char
    payloadCharAr[i] = (char)payload[i];
  }
  Serial.println();


  if (strcmp(topic, "ELECTRIC-METER-RELAY") == 0) {
    switchRelay(payloadCharAr[0] == '1');
  } else if (strcmp(topic, "ELECTRIC-METER-RELAY-THRESHOLD") == 0) {
    integerValue = atoi(payloadCharAr);
    Serial.print("Converted integer: ");
    Serial.println(integerValue);
  } else if (strcmp(topic, "ELECTRIC-METER-RELAY-THRESHOLD-RESET") == 0) {
    unitReset = atoi(payloadCharAr);
    Serial.print("Unit Reset: ");
    Serial.println(unitReset);
  }
}


void connectToBroker() {
  while (!mqttClient.connected()) {
    Serial.println("Attempting to connect...");
    if (mqttClient.connect("ESP32-8765457897")) {
      Serial.println("Connected to Broker...");
      mqttClient.subscribe("ELECTRIC-METER-RELAY");  // to send data from node red to esp32 // Subscribe
      mqttClient.subscribe("ELECTRIC-METER-RELAY-THRESHOLD");
      mqttClient.subscribe("ELECTRIC-METER-RELAY-THRESHOLD-RESET");
    } else {
      Serial.println("Error...");
      Serial.println(mqttClient.state());
      delay(1000);
    }
  }
}