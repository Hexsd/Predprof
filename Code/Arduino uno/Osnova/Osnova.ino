int trigPin = 2; // назначаем имя для Pin8
int echoPin = 3; // назначаем имя для Pin9
const int RED = 9;
const int GREEN = 8;
const int button1 = 4;
const int button2 = 5;
int but1 = 0;
int but2 = 0;
bool deviceOn = false; // переменная состояния устройства
String ess;
void setup() {
  Serial.begin(9600); // подключаем монитор порта
  pinMode(trigPin, OUTPUT); // назначаем trigPin, как выход
  pinMode(echoPin, INPUT); // назначаем echoPin, как вход
  pinMode(button1, INPUT);
  pinMode(button2, INPUT);
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
}

void loop() {

if (Serial.available() > 0) {
    String data = Serial.readStringUntil('\n');
    Serial.print("You sent me: ");
    Serial.println(data);
    ess=data;
  }

  but1 = digitalRead(button1);
  but2 = digitalRead(button2);
  // Проверяем состояние первой кнопки (включение устройства)
  if (but1 == 1 && !deviceOn) {
    deviceOn = true; // включаем устройство
    Serial.println("Устройство включено");
    delay(500); // антидребезг
    // Индикация работы устройства
    digitalWrite(GREEN, HIGH);
    digitalWrite(RED, LOW);
  }
  // Проверяем состояние второй кнопки (выключение устройства)
  if (but2 == 1 && deviceOn) {
    deviceOn = false; // выключаем устройство
    Serial.println("Устройство выключено");
    digitalWrite(GREEN, LOW);
    digitalWrite(RED, HIGH);
    delay(500); // антидребезг
  }
  // Если устройство включено, выполняем основную логику
  if (deviceOn) {
    int duration, cm; // переменные для показаний датчика
    digitalWrite(trigPin, LOW); // изначально датчик не посылает сигнал
    delayMicroseconds(2); // задержка в 2 микросекунды
    digitalWrite(trigPin, HIGH); // посылаем сигнал
    delayMicroseconds(10); // задержка в 10 микросекунд
    digitalWrite(trigPin, LOW); // выключаем сигнал
    duration = pulseIn(echoPin, HIGH); // включаем прием сигнала
    cm = duration / 58; // вычисляем расстояние в сантиметрах
    Serial.print(cm); // выводим расстояние в сантиметрах
    Serial.println(" cm");
    delay(500);
    if (ess == "342") {
      digitalWrite(RED,HIGH);
      delay(2000);
      digitalWrite(RED,LOW);
    }
  }
  delay(100); // небольшая задержка для предотвращения дребезга кнопок
}
