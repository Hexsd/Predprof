const int buttonStartPin = 8;       // Пин кнопки включения
const int buttonStopPin = 9;        // Пин кнопки экстренной остановки
const int ledRedPin = 10;            // Пин красного светодиода
const int ledGreenPin = 11;          // Пин зеленого светодиода
const int trigPin = 5;              // Пин Trig ультразвукового датчика
const int echoPin = 6;              // Пин Echo ультразвукового датчика
const int stepPin = 3;              // Пин STEP драйвера A4988
const int dirPin = 2;               // Пин DIR драйвера A4988
// Глобальные переменные
bool isSystemActive = false;       // Флаг состояния системы
bool isObjectDetected = false;     // Флаг наличия объекта
long distance = 0;                 // Расстояние до объекта
const int objectThreshold = 10;    // Пороговое расстояние для обнаружения объекта (см)
// Состояние кнопок
bool wasStartButtonPressed = false;
bool wasStopButtonPressed = false;
// База данных для ячеек
String data; // Хранение кода
const int numCells = 6;            // Количество ячеек
String cellDatabase[numCells];     // Массив для хранения QR-кодов ("" - свободная ячейка)
const int loadingZoneCell = 5;     // Индекс ячейки зоны загрузки/выгрузки
int currentCellIndex = 0;          // Текущее положение ячейки

void setup() {
  pinMode(buttonStartPin, INPUT);
  pinMode(buttonStopPin, INPUT);
  pinMode(ledRedPin, OUTPUT);
  pinMode(ledGreenPin, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);

  digitalWrite(ledRedPin, LOW);
  digitalWrite(ledGreenPin, LOW);

  for (int i = 0; i < numCells; i++) {
    cellDatabase[i] = "";
  }

  Serial.begin(9600);
}

void loop() {
  if (digitalRead(buttonStartPin) == LOW && !wasStartButtonPressed) {
    wasStartButtonPressed = true;
    activateSystem();
  } else if (digitalRead(buttonStartPin) == HIGH) {
    wasStartButtonPressed = false;
  }

  if (digitalRead(buttonStopPin) == LOW && !wasStopButtonPressed) {
    wasStopButtonPressed = true;
    deactivateSystem();
  } else if (digitalRead(buttonStopPin) == HIGH) {
    wasStopButtonPressed = false;
  }

  if (isSystemActive) {
    isObjectDetected = detectObject();

    if (isObjectDetected) {
      handleObjectDetected();
    }
  }
}

void activateSystem() {
  isSystemActive = true;
  digitalWrite(ledRedPin, HIGH);
  Serial.println("Система активирована. Готова к работе.");
  delay(1000);
}

void deactivateSystem() {
  isSystemActive = false;
  digitalWrite(ledRedPin, LOW);
  digitalWrite(ledGreenPin, LOW);
  Serial.println("Система остановлена.");
  delay(1000);
}

bool detectObject() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;

  if (distance > 0 && distance <= objectThreshold) {
    Serial.println("Объект обнаружен.");
    delay(500);
    return true;
  }
  return false;
}

void handleObjectDetected() {
  int freeCellIndex = findFreeCell();

  if (freeCellIndex != -1) {
    Serial.print("Свободная ячейка найдена: ");
    Serial.println(freeCellIndex);
    delay(1000);

    moveToCell(freeCellIndex);
    digitalWrite(ledRedPin, LOW);
    digitalWrite(ledGreenPin, HIGH);
    delay(500);
    waitForCarToEnter();
    String qrCode = readQRCode();
    cellDatabase[freeCellIndex] = qrCode;
    Serial.print("Автомобиль с QR-кодом ");
    Serial.print(qrCode);
    Serial.println(" успешно припаркован.");
    delay(1000);

    digitalWrite(ledGreenPin, LOW);
    digitalWrite(ledRedPin, HIGH);
  } else {
    Serial.println("Нет доступных ячеек для парковки.");
    delay(1000);
  }
}

int findFreeCell() {
  for (int i = 0; i < numCells; i++) {
    if (cellDatabase[i] == "") {
      return i;
    }
  }
  return -1;
}

void moveToCell(int targetCellIndex) {
  int stepsPerCell = 25;
  int stepDifference = targetCellIndex - currentCellIndex;
  int steps = abs(stepDifference) * stepsPerCell;
  int direction = (stepDifference > 0) ? HIGH : LOW;

  digitalWrite(dirPin, direction);
  for (int i = 0; i < steps; i++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(500);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(500);
    if (i % 100 == 0) delay(10);
  }

  currentCellIndex = targetCellIndex;
  Serial.print("Перемещено к ячейке: ");
  Serial.println(targetCellIndex);
  delay(1000);
}

String readQRCode() {
  if (Serial.available() > 0) {
    data = Serial.readStringUntil('\n');
    Serial.print("You sent me: ");
    Serial.println(data);
  }
  return "zz";
}

void retrieveCar(String qrCode) {
  for (int i = 0; i < numCells; i++) {
    if (cellDatabase[i] == qrCode) {
      Serial.print("Автомобиль с QR-кодом ");
      Serial.print(qrCode);
      Serial.println(" найден. Начинается процесс выгрузки.");
      delay(1000);

      moveToCell(i);
      delay(1000);
      moveToCell(loadingZoneCell);
      delay(1000);
      digitalWrite(ledGreenPin, HIGH);
      waitForCarToEnter();
      cellDatabase[i] = "";
      Serial.println("Автомобиль успешно выгружен.");
      delay(2000);
      digitalWrite(ledGreenPin, LOW);
      return;
    }
  }
  Serial.println("Автомобиль с таким QR-кодом не найден в базе данных.");
  delay(1000);
}

void waitForCarToEnter() {
  Serial.println("Ожидание автомобиля...");
  while (detectObject() == true) {
    delay(500);
  }
  Serial.println("Автомобиль заехал в ячейку.");
  delay(1000);
}