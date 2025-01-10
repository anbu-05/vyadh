#include <SPI.h>
#include <LoRa.h>
#include <string> 

// Pin definitions for LoRa SX1278
#define ss 5          // Slave Select (NSS)
#define rst 2        // Reset
#define dio0 4       // Digital I/O 0

int count = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("LoRa Transmitter Setup");

  // Configure LoRa module
  LoRa.setPins(ss, rst, dio0);

  if (!LoRa.begin(433E6)) { // Set frequency to 433 MHz
    Serial.println("Starting LoRa failed! Check connections and power supply.");
    while (1);
  }

  Serial.println("LoRa initialization successful!");
}

void loop() {
  String message = "Hello from ESP32 Transmitter!";
  String count_string = String(count);

  LoRa.beginPacket();
  LoRa.print(message + count_string);
  LoRa.endPacket();

  Serial.println("Message sent: " + message + " " + count_string);

  count++;

  delay(10);
}
