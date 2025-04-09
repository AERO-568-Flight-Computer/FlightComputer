#include <Arduino.h>
#include <Wire.h>
#include <esp_now.h>
#include <WiFi.h>
#include <CRC16.h>
#include <CRC.h>
#include <AllSensors_DLHR.h>
#include <ms4525do.h>
#include <SparkFunMPL3115A2.h>

// All Sensors DLHR Sensor
AllSensors_DLHR_L30D_8 DLHR(&Wire);
// TE MS4525DO Sensor
bfs::Ms4525do MS4525;
// Sparkfun MPL3115 Sensor
MPL3115A2 MPL3115A2;
// CRC16 declaration
CRC16 crc;

int rearFlagPin = A0;   // select the input pin for the rear flag potentiometer, 12 bit ADC
int frontFlagPin = A1;  // select the input pin for the front flag potentiometer, 12 bit ADC
float rearFlagMaxAoA = -9.5;
float rearFlagMinAoA = 9;
float frontFlagMinYaw = 9;
float frontFlagMaxYaw = -20.5;
int flagADCmin = 0;
int flagADCmax = 4095;

// REPLACE WITH YOUR RECEIVER MAC Address
uint8_t broadcastAddress[] = { 0x35, 0x85, 0x18, 0x7A, 0xF4, 0x1C };

// Structure example to send data
// Must match the receiver structure
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

// Create a struct_message called myData
struct_message dataPackage;

esp_now_peer_info_t peerInfo;

// Variables will change:
int ledState = LOW;  // ledState used to set the LED

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;  // will store last time LED was updated

// constants won't change:
const long interval = 1000;  // interval at which to blink (milliseconds)

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  // Serial.print("\r\nLast Packet Send Status:\t");
  // Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setup() {
  //////////////////////////////////// Data /////////////////////////////////////////
  /* Serial to display data */
  Serial.begin(115200);
  // while(!Serial){}
  Wire.begin();
  // Wire.setClock(400000);

  MS4525.Config(&Wire, 0x28, 1.0f, -1.0f);
  /* Starting communication with the pressure transducer */
  if (!MS4525.Begin()) {
    Serial.println("Error communicating with sensor");
    // while(1);
  }

  DLHR.setPressureUnit(AllSensors_DLHR::PressureUnit::PASCAL);

  MPL3115A2.begin();  // Get sensor online

  // Configure the sensor
  //MPL3115A2.setModeAltimeter(); // Measure altitude above sea level in meters
  MPL3115A2.setModeBarometer();  // Measure pressure in Pascals from 20 to 110 kPa

  MPL3115A2.setOversampleRate(1);  // Set Oversample to the recommended 128 // 10 Hz at 5, 110 Hz at 1, 3 Hz at 128
  MPL3115A2.enableEventFlags();    // Enable all three pressure and temp event flags

  digitalWrite(LED_BLUE, LOW);
  delay(500);
  digitalWrite(LED_BLUE, HIGH);
  delay(500);
  digitalWrite(LED_BLUE, LOW);
  delay(500);
  digitalWrite(LED_BLUE, HIGH);
  delay(500);

  //////////////////////////////// Communication ///////////////////////////////////
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Transmitted packet
  esp_now_register_send_cb(OnDataSent);

  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
  digitalWrite(LED_RED, LOW);
  delay(500);
  digitalWrite(LED_RED, HIGH);
  delay(500);
  digitalWrite(LED_RED, LOW);
  delay(500);
  digitalWrite(LED_RED, HIGH);
  delay(500);
}

// uint8_t crcCheck;

void loop() {
  crc.restart();
  ///////////////////////////////////////// Data //////////////////////////////////////////////
  DLHR.startMeasurement();
  DLHR.readData(true);
  MS4525.Read();
  int rearFlagValue = analogRead(rearFlagPin);
  int frontFlagValue = analogRead(frontFlagPin);

  dataPackage.rearFlagAoA = (rearFlagValue - flagADCmin) * (rearFlagMinAoA - rearFlagMaxAoA) / (flagADCmax - flagADCmin) + rearFlagMaxAoA;
  dataPackage.frontFlagYaw = (frontFlagValue - flagADCmin) * (frontFlagMaxYaw - frontFlagMinYaw) / (flagADCmax - flagADCmin) + frontFlagMinYaw;

  // DLHR Sensor
  dataPackage.diffPressureDLHR = DLHR.pressure;
  dataPackage.diffSenseTempDLHR = DLHR.temperature;
  // MS4525 Sensor
  dataPackage.diffPressureMS4525 = MS4525.pres_pa();
  dataPackage.diffSenseTempMS4525 = MS4525.die_temp_c();

  dataPackage.absPressure = MPL3115A2.readPressure();
  dataPackage.absSenseTemp = MPL3115A2.readTemp();

  dataPackage.milliTime = millis();

  crc.add(dataPackage.rearFlagAoA);
  crc.add(dataPackage.frontFlagYaw);
  crc.add(dataPackage.diffPressureMS4525);
  crc.add(dataPackage.diffSenseTempMS4525);
  crc.add(dataPackage.diffPressureDLHR);
  crc.add(dataPackage.diffSenseTempDLHR);
  crc.add(dataPackage.absPressure);
  crc.add(dataPackage.absSenseTemp);
  crc.add(dataPackage.milliTime);
  dataPackage.crcCheck = crc.calc();

  Serial.print("CRC: ");
  Serial.print(dataPackage.crcCheck);

  Serial.print("\tTime (ms): ");
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

  //////////////////////////////////// Communication //////////////////////////////////////////
  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&dataPackage, sizeof(dataPackage));

  if (result == ESP_OK) {
    // Serial.println("Sent with success");
  } else {
    Serial.println("Error sending the data");
    digitalWrite(LED_RED, LOW);
  }

  delay(10); // used to not oversample the DLHR sensor (returns non-sense value)

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }

    // set the LED with the ledState of the variable:
    digitalWrite(LED_GREEN, ledState);
  }
}
