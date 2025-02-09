import cv2
from picamera2 import Picamera2
from pyzbar.pyzbar import decode
picam2 = Picamera2()
camera_config = picam2.create_preview_configuration()
picam2.configure(camera_config)
picam2.start()
main={"format": "RGB888", "size": (640, 480)}
while True:
    frame = picam2.capture_array()
    decoded_objects = decode(frame)
    for obj in decoded_objects:
        qr_data = obj.data.decode("utf-8")
        print(f"QR Code Detected: {qr_data}")
        points = obj.polygon
        if len(points) == 4:
            for i in range(len(points)):
                pt1 = (int(points[i].x), int(points[i].y))
                pt2 = (int(points[(i + 1) % 4].x), int(points[(i + 1) % 4].y))
                cv2.line(frame, pt1, pt2, (0, 255, 0), 2)
    cv2.imshow("QR Code Scanner", frame)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break
picam2.stop()
cv2.destroyAllWindows()