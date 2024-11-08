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
  
  esp_now_register_recv_cb(OnDataRecv); // Register receive callback
}

void loop() {
  Serial.print("I did this to data -> ");
  Serial.println(newData * 5);
  delay(3000);
}

/** Callback when data is received from Master **/
void OnDataRecv(const esp_now_recv_info *recv_info, const uint8_t *data, int data_len) {
  Serial.print("I just received -> ");
  Serial.println(*data);
  memcpy(&newData, data, sizeof(newData));

/*
Serial.print("MAC Address: ");
for (int i = 0; i < 6; i++) {
    Serial.printf("%02X", recv_info->src_addr[i]);
    if (i < 5) Serial.print(":");
}
Serial.println();
*/
}
