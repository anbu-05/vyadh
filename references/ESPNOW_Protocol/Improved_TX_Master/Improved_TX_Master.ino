#include <esp_now.h>
#include <WiFi.h>

#define CHANNEL 1

/** This is all the data about the peer **/
esp_now_peer_info_t slave;

/** The all-important data! **/
uint8_t data = 0;

void setup() { 
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_send_cb(OnDataSent);
  
  ScanForSlave(); // Search for the receiver with SSID containing "RX"
  
  if (esp_now_add_peer(&slave) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
}

void loop() {
  esp_err_t result = esp_now_send(slave.peer_addr, &data, sizeof(data));
  if (result == ESP_OK) {
    Serial.print("Data sent successfully: ");
  } else {
    Serial.print("Error sending data: ");
  }
  Serial.println(data);
  data++;
  delay(3000);
}

/** Scan for slaves in AP mode and add as peer if found **/
void ScanForSlave() {
  int8_t scanResults = WiFi.scanNetworks();
  bool slaveFound = false;

  for (int i = 0; i < scanResults; ++i) {
    String SSID = WiFi.SSID(i);
    String BSSIDstr = WiFi.BSSIDstr(i);

    if (SSID.indexOf("RX") == 0) {
      int mac[6];
      if (sscanf(BSSIDstr.c_str(), "%x:%x:%x:%x:%x:%x",  &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]) == 6) {
        for (int ii = 0; ii < 6; ++ii) {
          slave.peer_addr[ii] = (uint8_t) mac[ii];
        }
        slave.channel = CHANNEL;
        slave.encrypt = 0;
        slaveFound = true;
        break;
      }
    }
  }

  if (slaveFound) {
    Serial.println("Slave found and added as peer.");
  } else {
    Serial.println("No slave found with the SSID 'RX'");
  }
}

/** Callback when data is sent from Master to Slave **/
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("I sent my data -> ");
  Serial.print(data);
  Serial.print(" with status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Failure");
}

