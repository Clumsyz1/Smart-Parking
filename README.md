# Smart Parking - Ultimate Edition

โปรเจกต์ Arduino นี้เป็นระบบ Smart Parking สำหรับ ESP32 ที่ใช้เซ็นเซอร์ Ultrasonic ตรวจวัดระยะทาง และควบคุมไม้กั้นด้วย Servo พร้อมแสดงสถานะบนจอ LCD และเชื่อมต่อกับ Blynk

## ฟีเจอร์

- ตรวจสอบจำนวนที่จอดว่าง
- เปิด/ปิดไม้กั้นอัตโนมัติเมื่อมีรถเข้ามา
- ปุ่ม Manual Override ใน Blynk
- แสดงสถานะบน LCD
- บันทึกเวลาการเข้า

## ฮาร์ดแวร์

- ESP32
- เซ็นเซอร์ Ultrasonic (TRIG -> GPIO 5, ECHO -> GPIO 18)
- Servo motor (GPIO 19)
- สวิตช์ตรวจวัดช่องจอด 4 ช่อง (GPIO 4, 13, 14, 27)
- จอ LCD I2C 16x2

## การตั้งค่า

1. แก้ค่า:
   - `BLYNK_TEMPLATE_ID` เป็น Template ID ของคุณ
   - `BLYNK_TEMPLATE_NAME` เป็น Template Name ของคุณ
   - `BLYNK_AUTH_TOKEN` เป็น Auth Token ของคุณ
   - `ssid` เป็นชื่อ Wi-Fi ของคุณ
   - `pass` เป็นรหัสผ่าน Wi-Fi ของคุณ

2. อัพโหลดโค้ดไปยังบอร์ด ESP32

## ไลบรารีที่ต้องติดตั้ง

- WiFi
- BlynkSimpleEsp32
- ESP32Servo
- Wire
- LiquidCrystal_I2C
- WidgetRTC
- TimeLib
