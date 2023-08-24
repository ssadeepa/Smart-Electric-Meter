

void updateCurrent() {
  //Serial.println(analogRead(CURRENT_GPIO));
  double current = analogRead(CURRENT_GPIO);
  dtostrf(current, 6, 2, currentAr);
}

// void updateVoltage() {
//   double voltage = analogRead(VOLTAGE_GPIO);
//   dtostrf(voltage, 6, 2, voltageAr);
// }

void updateUART() {

  // Serial.println("Message Received: ");
  // Serial.println(Serial2.readString());
  String receivedString = Serial2.readString();
  // Serial.println(receivedString);// Read the incoming data as a String
  receivedString.trim();                         // Remove leading and trailing whitespaces

  // Convert the String to a C-style char array
  receivedString.toCharArray(receivedData, bufferSize);
}

// void updateUART() {

//   if (Serial2.available()) {
//     char receivedData = Serial2.read();

//     // Store the received character in the array
//     receivedDataArray[dataIndex] = receivedData;
//     dataIndex++;

//     // If we've received 4 characters, print the array and reset the index
//     if (dataIndex >= 8) {
//       for (int i = 0; i < 8; i++) {
//         Serial.print(receivedDataArray[i]);
//       }
//       Serial.println();  // Print a newline after the array
//       dataIndex = 0;     // Reset the index
//     }
//   }
// }

void separateArray() {
  int arrayLength = sizeof(receivedData) - 1;
  Serial.println(arrayLength);
  int halfLength = arrayLength / 2;


  for (int i = 0; i < halfLength; i++) {
    firstHalf[i] = receivedData[i];
    secondHalf[i] = receivedData[halfLength + i];
  }

  firstHalf[halfLength] = '\0';  // Null-terminate the new arrays
  secondHalf[halfLength] = '\0';
  //delay(500);
}
