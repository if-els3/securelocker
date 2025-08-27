#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiClient.h> 
#include <HTTPClient.h>
#include <SPI.h>
#include <MFRC522.h>
#include <ESP32Servo.h>
#include <ArduinoJson.h>
#include <Base64.h> 
#include "mbedtls/aes.h"
#include "mbedtls/sha256.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/md.h"

// ----------------- Pin Configuration -----------------
#define RST_PIN     22 
#define SS_PIN      21  
#define SERVO_PIN   4   

// ----------------- WiFi & mDNS -----------------
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";
const char* esp32_hostname = "esp32-rfid"; // Hostname untuk mDNS: esp32-rfid.local

// ----------------- AES-128 -----------------
const unsigned char encryption_key[] = "kunci_rahasia_1234"; //you can change the key but must be 16 bytes (128 bits)
// Initialization Vector must 16 byte
const unsigned char iv[] = "1234567890123456"; 

// ----------------- RFID & Servo -----------------
MFRC522 mfrc522(SS_PIN, RST_PIN);
Servo myservo;

// Add padding PKCS7 to data
void pkcs7_pad(unsigned char* buffer, size_t* len, size_t block_size) {
    size_t pad_len = block_size - (*len % block_size);
    for (size_t i = 0; i < pad_len; i++) {
        buffer[*len + i] = (unsigned char)pad_len;
    }
    *len += pad_len;
}

// Encrypt UID using AES-128-CBC
bool encryptUID(const String& message, unsigned char* encrypted_data, size_t* encrypted_len) {
    mbedtls_aes_context aes;
    unsigned char local_iv[16];
    size_t message_len = message.length();
    unsigned char padded_buffer[message_len + 16];
    memcpy(padded_buffer, message.c_str(), message_len);
    size_t padded_len = message_len;
    pkcs7_pad(padded_buffer, &padded_len, 16);

    memcpy(local_iv, iv, 16);

    mbedtls_aes_init(&aes);
    if (mbedtls_aes_setkey_enc(&aes, encryption_key, 128) != 0) {
        Serial.println("failed to set encryption key.");
        mbedtls_aes_free(&aes);
        return false;
    }

    // Encrypt data
    if (mbedtls_aes_crypt_cbc(&aes, MBEDTLS_AES_ENCRYPT, padded_len, local_iv, padded_buffer, encrypted_data) != 0) {
        Serial.println("Failed to encrypt data.");
        mbedtls_aes_free(&aes);
        return false;
    }

    *encrypted_len = padded_len;
    mbedtls_aes_free(&aes);
    return true;
}

void setup() {
    Serial.begin(115200);
    SPI.begin();
    mfrc522.PCD_Init();
    myservo.attach(SERVO_PIN, 1000, 2000);
    myservo.write(90); //servo set to 90 degree (lock position)

    Serial.println("Connecting to WiFi...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\n✅ Connected to Wifi. IP ESP32: " + WiFi.localIP().toString());
    
    if (!MDNS.begin(esp32_hostname)) {
        Serial.println("❗ failed to start mDNS responder!");
    } else {
        Serial.println("🌐 mDNS responder started: " + String(esp32_hostname) + ".local");
    }

    Serial.println("📡 Waiting for RFID card...");
}

void loop() {
    if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
        delay(50);
        return;
    }

    String uidStr = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
        uidStr += String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
        uidStr += String(mfrc522.uid.uidByte[i], HEX);
    }
    uidStr.toUpperCase();
    Serial.println("🔍 UID find: " + uidStr);

    // --- Proses Enkripsi ---
    unsigned char encrypted_data[128];
    size_t encrypted_len = 0;

    Serial.println("✍️ Encrypting UID...");
    if (encryptUID(uidStr, encrypted_data, &encrypted_len)) {
        Serial.print("   Panjang data terenkripsi: "); Serial.println(encrypted_len);
        
        // Encode data terenkripsi dan IV ke Base64
        String encrypted_uid_b64 = base64::encode(encrypted_data, encrypted_len);
        String iv_b64 = base64::encode(iv, 16);
        Serial.println("   Encrypted UID (Base64): " + encrypted_uid_b64);

        // --- sending to server ---
        if (WiFi.status() == WL_CONNECTED) {
            HTTPClient http;
            String serverUrl = "http://192.168.93.105/rfid_web/log_uid.php"; //change to your server address

            http.begin(serverUrl);
            http.addHeader("Content-Type", "application/json");

            // Buat payload JSON
            JsonDocument doc;
            doc["encrypted_uid"] = encrypted_uid_b64;
            doc["iv"] = iv_b64;
            
            String payload;
            serializeJson(doc, payload);

            Serial.println("🚀 Sending payload " + payload);
            int httpResponseCode = http.POST(payload);

            if (httpResponseCode > 0) {
                String response = http.getString();
                Serial.println("💬 Response from server: " + response);

                JsonDocument responseJson;
                deserializeJson(responseJson, response);
                const char* status = responseJson["status"];

                if (strcmp(status, "ALLOW") == 0) {
                    Serial.println("✅ Access granted, opening the lock...");
                    myservo.write(0); 
                    delay(3000);      
                    myservo.write(90); 
                } else {
                    Serial.println("❌ Access denied.");
                }
            } else {
                Serial.print("❗ HTTP Error: ");
                Serial.println(http.errorToString(httpResponseCode).c_str());
            }
            http.end();
        } else {
            Serial.println("⚠️ Wifi disconnected!");
        }
    } else {
        Serial.println("❗ Failed to encrypt UID.");
    }

    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
    delay(2000); 
}
