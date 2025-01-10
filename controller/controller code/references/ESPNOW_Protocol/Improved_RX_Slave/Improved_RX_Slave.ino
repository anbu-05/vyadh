#include <esp_now.h>
#include <WiFi.h>

#define CHANNEL 1

uint8_t newData;

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_AP);
  WiFi.softAP("RX_1", "RX_1_Password", CHANNEL, 0);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  Serial.print("Processed data -> ");
  Serial.println(newData * 5);
  delay(3000);
}

// Updated callback function to use esp_now_recv_info as required
void OnDataRecv(const esp_now_recv_info *recv_info, const uint8_t *data, int data_len) {
  Serial.print("Received data from MAC -> ");
  for (int i = 0; i < 6; i++) {
    Serial.printf("%02X", recv_info->src_addr[i]);
    if (i < 5) Serial.print(":");
  }
  Serial.print("\nData received -> ");
  Serial.println(*data);
  memcpy(&newData, data, sizeof(newData));
}
