// Receiver Code
#include <SPI.h>
#include <LoRa.h>

// Pin definitions for LoRa SX1278
#define ss 22         // Slave Select (NSS)
#define rst 5       // Reset
#define dio0 21       // Digital I/O 0

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("LoRa Receiver Setup");

  // Configure LoRa module
  LoRa.setPins(ss, rst, dio0);

  if (!LoRa.begin(433E6)) { // Set frequency to 433 MHz
    Serial.println("Starting LoRa failed! Check connections and power supply.");
    while (1);
  }

  Serial.println("LoRa initialization successful!");
}

void loop() {
  // Try to parse a packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    Serial.println("Received packet:");

    // Read packet contents
    String incoming = "";
    while (LoRa.available()) {
      incoming += (char)LoRa.read();
    }

    Serial.println("Message: " + incoming);

    // Print RSSI (signal strength)
    Serial.print("RSSI: ");
    Serial.println(LoRa.packetRssi());
  }
}
