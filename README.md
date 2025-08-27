# securelocker
# Secure RFID Access Control & Logging System

This project is a complete solution for a secure RFID-based access control system. It uses an ESP32 microcontroller to read RFID cards, encrypts the UID data using AES-128, and sends it to a central server running XAMPP (Apache and MySQL). A PHP backend processes the data, logs all access attempts to a database, and provides a simple web-based dashboard to view real-time logs.

The system is designed to be a proof-of-concept for secure data transmission between IoT devices and a server.

# ✨ Features
- End-to-End Encryption: Encrypts RFID UID data on the ESP32 using AES-128 before transmission.
- Centralized Logging: Stores all access attempts (allowed or denied) in a MySQL database.
- Real-time Monitoring: A simple web interface displays the latest RFID logs as they happen.
- Automated Access Control: The system automatically grants or denies access based on a list of pre-registered UID cards in the database.
- Wireless Communication: All data is sent wirelessly from the ESP32 to the server via a Wi-Fi network.

# 🛠️ Prerequisites
## Hardware
- ESP32 board (e.g., NodeMCU-32S, ESP32-WROOM-32)
- MFRC522 RFID reader module
- Micro Servo (e.g., SG90) for the locker mechanism
- Jumper wires and a breadboard
- An RFID card/tag

## Software
- Arduino IDE with ESP32 board support
- XAMPP (or any LAMP/WAMP stack) for the server
- MySQL database (included in XAMPP)

## Required Arduino libraries:
- MFRC522
- ESP32Servo
- ArduinoJson
- ESPmDNS
- Base64

# ⚙️ Setup and Installation
## Step 1: Database Setup
- Launch XAMPP Control Panel and start the Apache and MySQL services.
- Open your web browser and go to http://localhost/phpmyadmin/.
- Navigate to the SQL tab and run the SQL code from the server/database.sql file to create the rfid_system database and its tables.

## Step 2: Server Setup
- Place the server/ directory and all its contents inside your XAMPP's web root folder. This is typically C:\xampp\htdocs\. The path should look like C:\xampp\htdocs\rfid-access-control\server\.
- Open server/log_uid.php and server/index.php and verify the database credentials match your setup (default is username="root" and password="").

## Step 3: ESP32 Setup
- Open the esp32/rfid-aes.ino file in the Arduino IDE.
- Install the required libraries listed in the Prerequisites section using the Arduino Library Manager.-
- Update the Wi-Fi credentials in the code:
```
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
```
- Update the server URL. Crucially, replace localhost with the local IP address of your computer. You can find this by running ipconfig in your terminal.
```
String serverUrl = "http://YOUR_COMPUTER_IP/rfid-access-control/server/log_uid.php";
```
- Upload the code to your ESP32 board.

## Step 4: Verification
- Open your web browser and navigate to http://localhost/rfid-access-control/server/index.php. You should see the live log dashboard.
- Open the Serial Monitor in the Arduino IDE to view the ESP32's output.
- Power the ESP32 and scan a card. You should see "access denied" or "access allowed" in the Serial Monitor and a new entry should appear on your web dashboard.

# 🤝 Contributing
Contributions are welcome! If you have any improvements or bug fixes, feel free to submit a pull request.

