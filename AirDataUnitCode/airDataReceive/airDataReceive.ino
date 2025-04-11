#include "Arduino.h"
#include <esp_now.h>
#include <WiFi.h>
#include "CRC16.h"
#include "CRC.h"

CRC16 crc;

uint16_t crcMatch;
int8_t syncbite = -1;

// Structure example to receive data
// Must match the sender structure
typedef struct struct_message {
  int milliTime;
  float absPressure;
  float absSenseTemp;
  float diffPressureMS4525;
  float diffSenseTempMS4525;
  float diffPressureDLHR;
  float diffSenseTempDLHR;
  float rearFlagAoA;
  float frontFlagYaw;
  uint16_t crcCheck;
} struct_message;

// Create a struct_message called dataPackage
struct_message dataPackage;

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  crc.restart();
  memcpy(&dataPackage, incomingData, sizeof(dataPackage));

  crc.add(dataPackage.rearFlagAoA);
  crc.add(dataPackage.frontFlagYaw);
  crc.add(dataPackage.diffPressureMS4525);
  crc.add(dataPackage.diffSenseTempMS4525);
  crc.add(dataPackage.diffPressureDLHR);
  crc.add(dataPackage.diffSenseTempDLHR);
  crc.add(dataPackage.absPressure);
  crc.add(dataPackage.absSenseTemp);
  crc.add(dataPackage.milliTime);
  crcMatch = crc.calc();

  Serial.print("Bytes received: ");
  Serial.print(len);

  Serial.print("\tCRC Recieved: ");
  Serial.print(dataPackage.crcCheck);

  Serial.print("\tCRC Calculated: ");
  Serial.println(crcMatch);

  Serial.print("Time (ms): ");
  Serial.print(dataPackage.milliTime);

  Serial.print("\tAOA: ");
  Serial.print(dataPackage.rearFlagAoA);
  Serial.print("\tYaw: ");
  Serial.print(dataPackage.frontFlagYaw);

  Serial.print("\tMS4525 Diff Pressure (Pa): ");
  Serial.print(dataPackage.diffPressureMS4525);
  Serial.print("\tDiff Temp (C): ");
  Serial.print(dataPackage.diffSenseTempMS4525);

  Serial.print("\tDLHR Diff Pressure (Pa): ");
  Serial.print(dataPackage.diffPressureDLHR);
  Serial.print("\tDiff Temp (C): ");
  Serial.print(dataPackage.diffSenseTempDLHR);

  Serial.print("  \tAbs Pressure (Pa): ");
  Serial.print(dataPackage.absPressure);
  Serial.print("\tAbs Temp(C): ");
  Serial.println(dataPackage.absSenseTemp);

  if (dataPackage.crcCheck == crcMatch) {
    Serial.println("Data received correctly!\n");

    byte byteArray[sizeof(float)];
    byte byteArrayInt[sizeof(int8_t)];
    byte byteArray16Int[sizeof(uint16_t)];


    memcpy(byteArrayInt, &syncbite, sizeof(int8_t));
    Serial0.write(syncbite);

    memcpy(byteArray, &dataPackage.milliTime, sizeof(int));
    Serial0.write(byteArray, sizeof(int));

    memcpy(byteArray, &dataPackage.absPressure, sizeof(float));
    Serial0.write(byteArray, sizeof(float));

    memcpy(byteArray, &dataPackage.absSenseTemp, sizeof(float));
    Serial0.write(byteArray, sizeof(float));

    memcpy(byteArray, &dataPackage.diffPressureMS4525, sizeof(float));
    Serial0.write(byteArray, sizeof(float));

    memcpy(byteArray, &dataPackage.diffSenseTempMS4525, sizeof(float));
    Serial0.write(byteArray, sizeof(float));

    memcpy(byteArray, &dataPackage.diffPressureDLHR, sizeof(float));
    Serial0.write(byteArray, sizeof(float));

    memcpy(byteArray, &dataPackage.diffSenseTempDLHR, sizeof(float));
    Serial0.write(byteArray, sizeof(float));

    memcpy(byteArray, &dataPackage.rearFlagAoA, sizeof(float));
    Serial0.write(byteArray, sizeof(float));

    memcpy(byteArray, &dataPackage.frontFlagYaw, sizeof(float));
    Serial0.write(byteArray, sizeof(float));

    memcpy(byteArray16Int, &dataPackage.crcCheck, sizeof(uint16_t));
    Serial0.write(byteArray16Int, sizeof(uint16_t));

    memcpy(byteArray16Int, &crcMatch, sizeof(uint16_t));
    Serial0.write(byteArray16Int, sizeof(uint16_t));

  } else {
    Serial.println("CRC error! Not all Data received!\n");
  }

/*
  byte byteArray[sizeof(float)];
  byte byteArrayInt[sizeof(int8_t)];
  byte byteArray16Int[sizeof(uint16_t)];

  
  memcpy(byteArrayInt, &syncbite, sizeof(int8_t));
  Serial0.write(syncbite);

  memcpy(byteArray, &dataPackage.milliTime, sizeof(int));
  Serial0.write(byteArray, sizeof(int));

  memcpy(byteArray, &dataPackage.absPressure, sizeof(float));
  Serial0.write(byteArray, sizeof(float));

  memcpy(byteArray, &dataPackage.absSenseTemp, sizeof(float));
  Serial0.write(byteArray, sizeof(float));

  memcpy(byteArray, &dataPackage.diffPressure, sizeof(float));
  Serial0.write(byteArray, sizeof(float));

  memcpy(byteArray, &dataPackage.diffSenseTemp, sizeof(float));
  Serial0.write(byteArray, sizeof(float));

  memcpy(byteArray, &dataPackage.rearFlagAoA, sizeof(float));
  Serial0.write(byteArray, sizeof(float));

  memcpy(byteArray, &dataPackage.frontFlagYaw, sizeof(float));
  Serial0.write(byteArray, sizeof(float));

  memcpy(byteArray16Int, &dataPackage.crcCheck, sizeof(uint16_t));
  Serial0.write(byteArray16Int, sizeof(uint16_t));

  memcpy(byteArray16Int, &crcMatch, sizeof(uint16_t));
  Serial0.write(byteArray16Int, sizeof(uint16_t));
  */
}

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  Serial0.begin(115200);
  delay(1500);

  //Serial.println("printing");

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // I think this is all it needed, i added the crc check too, works on my end hopefully it works for you too, luv you bryan ;)
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  // Serial0.println("test");
}


/*
#include "Arduino.h"
#include <esp_now.h>
#include <WiFi.h>
#include "CRC16.h"
#include "CRC.h"

CRC16 crc;

int8_t syncbite = -1;

// Structure example to receive data

typedef struct struct_message {
  int milliTime;
  float absPressure;
  float absSenseTemp;
  float diffPressureMS4525;
  float diffSenseTempMS4525;
  float diffPressureDLHR;
  float diffSenseTempDLHR;
  float rearFlagAoA;
  float frontFlagYaw;
  uint16_t crcCheck;
} struct_message;

// Create a struct_message called dataPackage
struct_message dataPackage;

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  crc.restart();

  memcpy(&dataPackage, incomingData, sizeof(dataPackage));

  crc.add(dataPackage.rearFlagAoA);
  crc.add(dataPackage.frontFlagYaw);
  crc.add(dataPackage.diffPressureMS4525);
  crc.add(dataPackage.diffSenseTempMS4525);
  crc.add(dataPackage.diffPressureDLHR);
  crc.add(dataPackage.diffSenseTempDLHR);
  crc.add(dataPackage.absPressure);
  crc.add(dataPackage.absSenseTemp);
  crc.add(dataPackage.milliTime);
  uint16_t crcMatch = crc.calc();

  if (crcMatch == dataPackage.crcCheck) {

    uint8_t sendingData[sizeof(dataPackage) - 1]; 
    uint8_t syncByte = 255;

    sendingData[0] = syncByte;  // Directly set the first byte to 255

    size_t offset = 1;

    memcpy(sendingData + offset, &dataPackage, sizeof(dataPackage) - 4);
    
    uint8_t crcData[sizeof(sendingData) - 2];

    memcpy(crcData, sendingData, sizeof(crcData));

    uint16_t crcMatch = crc16_custom(crcData, sizeof(crcData));

    offset += sizeof(crcData) - 1;

    memcpy(sendingData + offset, &crcMatch, sizeof(crcMatch));

    Serial0.write(sendingData, sizeof(sendingData));

    delay(1000);

  }
  
}


uint16_t crc16_custom(const uint8_t *data, size_t length) {

    uint16_t crc = 0x0000; // Initial value for 16-bit CRC
    uint16_t poly = 0x8005; // Common polynomial for 16-bit CRC (x^16 + x^15 + x^2 + 1)

    for (size_t i = 0; i < length; i++) {
        crc ^= static_cast<uint16_t>(data[i]) << 8;
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 0x8000) { // Check the leftmost bit
                crc = (crc << 1) ^ poly;
            } else {
                crc <<= 1;
            }
        }
    }

    return crc;
}


void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  Serial0.begin(9600); 
  delay(1500);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // I think this is all it needed, i added the crc check too, works on my end hopefully it works for you too, luv you bryan ;)
  esp_now_register_recv_cb(OnDataRecv);

}

void loop() {
  
}


*/