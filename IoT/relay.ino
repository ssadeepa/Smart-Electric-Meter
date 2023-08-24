void switchRelay(boolean statusRelay) {

  if (statusRelay) {
    digitalWrite(LED, HIGH);
  } else {
    digitalWrite(LED, LOW);
  }
}