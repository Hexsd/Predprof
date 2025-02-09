import cv2
from picamera2 import Picamera2
from pyzbar.pyzbar import decode

# Инициализация камеры Raspberry Pi
picam2 = Picamera2()
camera_config = picam2.create_preview_configuration()  # Создание конфигурации предварительного просмотра
picam2.configure(camera_config)  # Применение конфигурации
picam2.start()  # Запуск камеры

# Основные параметры формата изображения
main = {"format": "RGB888", "size": (640, 480)}

while True:
    # Захват кадра с камеры
    frame = picam2.capture_array()

    # Декодирование QR-кодов в кадре
    decoded_objects = decode(frame)
    for obj in decoded_objects:
        qr_data = obj.data.decode("utf-8")  # Декодирование данных QR-кода
        print(f"QR Code Detected: {qr_data}")  # Вывод распознанного QR-кода

        # Получение координат границ QR-кода
        points = obj.polygon
        if len(points) == 4:  # Проверка, что это четырехугольник
            for i in range(len(points)):
                pt1 = (int(points[i].x), int(points[i].y))
                pt2 = (int(points[(i + 1) % 4].x), int(points[(i + 1) % 4].y))
                cv2.line(frame, pt1, pt2, (0, 255, 0), 2)  # Отрисовка границ QR-кода

    # Отображение изображения с камеры
    cv2.imshow("QR Code Scanner", frame)

    # Выход из программы при нажатии клавиши 'q'
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# Остановка камеры и закрытие окна OpenCV
picam2.stop()
cv2.destroyAllWindows()