import serial
import time

# Настройка последовательного порта (UART)
ser = serial.Serial('/dev/ttyUSB0', 9600, timeout=1)
ser.flush()  # Очистка входного и выходного буфера

while True:
    # Отправка строки по UART
    ser.write(send_string.encode('utf-8'))
    time.sleep(2)  # Задержка для ожидания ответа

    # Чтение строки, полученной через UART
    receive_string = ser.readline().decode('utf-8').rstrip()
    print(receive_string)  # Вывод полученных данных