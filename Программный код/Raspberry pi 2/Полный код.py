import cv2
from picamera2 import Picamera2
from pyzbar.pyzbar import decode
import serial
import time

# Настройка UART (последовательного порта) для связи с микроконтроллером
ser = serial.Serial('/dev/ttyUSB0', 9600, timeout=1)
ser.flush()

# Настройка камеры Raspberry Pi
picam2 = Picamera2()
camera_config = picam2.create_preview_configuration()
picam2.configure(camera_config)
picam2.start()

# Основные параметры изображения
main = {"format": "RGB888", "size": (640, 480)}

while True:
    # Захват кадра с камеры
    frame = picam2.capture_array()

    # Декодирование QR-кодов в кадре
    decoded_objects = decode(frame)

    for obj in decoded_objects:
        qr_data = obj.data.decode("utf-8")  # Извлечение данных из QR-кода

        # Отрисовка рамки вокруг QR-кода
        points = obj.polygon
        if len(points) == 4:
            for i in range(len(points)):
                pt1 = (int(points[i].x), int(points[i].y))
                pt2 = (int(points[(i + 1) % 4].x), int(points[(i + 1) % 4].y))
                cv2.line(frame, pt1, pt2, (0, 255, 0), 2)  # Зеленая рамка

        # Отправка QR-кода через UART
        time.sleep(1)  # Задержка для стабильной передачи
        ser.write(qr_data.encode('utf-8'))

    # Получение ответа от микроконтроллера через UART
    receive_string = ser.readline().decode('utf-8').rstrip()
    print(receive_string)

    # Отображение обработанного изображения
    cv2.imshow("QR Code Scanner", frame)

    # Выход из программы при нажатии 'q'
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# Остановка камеры и закрытие всех окон OpenCV
picam2.stop()
cv2.destroyAllWindows()
