void setup() {
  Serial.begin(9600);
}
void loop() {
  if (Serial.available() > 0) {
    String data = Serial.readStringUntil('\n'); // Загрузка данных с Raspberry pi
    Serial.print("You sent me: ");
    Serial.println(data);
  }
}