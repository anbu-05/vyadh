#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <esp_now.h>
#include <WiFi.h>

#define CHANNEL 1

/** This is all the data about the peer **/
esp_now_peer_info_t slave;

/** The all-important data! **/
uint8_t data = 0;

RF24 radio(4, 5);  // CE, CSN

byte address[6] = "00001";


#define P1 32
#define D1 33

#define P2 26
#define D2 25

void setup() {

  //begining the module/library
  if (!radio.begin()) {
    Serial.println(F("radio hardware not responding!"));
  while (1) {} // hold program in infinite loop to prevent subsequent errors
  }

  Serial.begin(115200);
  Serial.println("start");
  
  //the RF module's power level:
  //Options: RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
  radio.setPALevel(RF24_PA_MAX);

  //opening reading "pipe" no.1 at the given address
  radio.openReadingPipe(1, address);

  //setting this RF module to recieve mode:
  radio.startListening();


  pinMode(P1, OUTPUT);
  pinMode(P2, OUTPUT);

  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);

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

struct controllerData{
  int joystick1_x;
  int joystick1_y;
  int Button1;

  int joystick2_x;
  int joystick2_y;
  int Button2;

  int joystick3_x;
  int joystick3_y;
  int Button3;

  int slide1;
  int slide2;
};


void loop() {
  controllerData payload;
  uint8_t pipe1;  //create a buffer to which the 1st pipe writes to
  if (radio.available(&pipe1))
  {  //see if there are any bytes available to read on pipe1
    Serial.println("payload available:");
    uint8_t	length = radio.getDynamicPayloadSize(); //get the length of the payload from RX FIFO pipe
    Serial.println(length);
    radio.read(&payload, length); //read the from the pipe the length of the payload from the above line
    Serial.println(radio.getChannel());
  }

  //traversal code
    //printing the joystick values: - comment this if the rx tx port is needed

    Serial.print("Joystick1 X: "); Serial.print(payload.joystick1_x);
    Serial.print(", Joystick1 Y: "); Serial.print(payload.joystick1_y);
    Serial.print(", button1: "); Serial.println(payload.Button1);
  
    Serial.print("Joystick2 X: "); Serial.print(payload.joystick2_x);
    Serial.print(", Joystick2 Y: "); Serial.print(payload.joystick2_y);
    Serial.print(", button2: "); Serial.println(payload.Button2);

    Serial.print("Joystick3 X: "); Serial.print(payload.joystick3_x);
    Serial.print(", Joystick3 Y: "); Serial.print(payload.joystick3_y);
    Serial.print(", button3: "); Serial.println(payload.Button3);

    Serial.print("slide switch 1: "); Serial.println(payload.slide1);
    Serial.print("slide switch 2: "); Serial.println(payload.slide2);

    Serial.println("  ");
    Serial.println("  ");
    Serial.println("  ");

    if (payload.joystick2_x >= 3900) {
      Serial.println("forward");
      forward();
    }

    else if (payload.joystick2_x <= 200) {
      Serial.println("backward");
      backward();
    }
    
    else if (payload.joystick2_y >= 3900) {
      Serial.println("right");
      right();
    }

    else if (payload.joystick2_y <= 200) {
      Serial.println("left");
      left();
    }

    else {
      Serial.println("stopped");
      stop();
    }



  //ESPNOW transmitter main code
  esp_err_t result = esp_now_send(slave.peer_addr, (uint8_t *) &payload, sizeof(payload));
  if (result == ESP_OK) {
    Serial.print("Data sent successfully: ");
  } else {
    Serial.print("Error sending data: ");
  }
  delay(20);
}


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
  Serial.print("Transmission Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

void forward() { //add a speed argument when speed change is decided
  analogWrite(P1, 255);
  analogWrite(P2, 255);

  digitalWrite(D1, HIGH);
  digitalWrite(D2, HIGH);
}

void backward() { //add a speed argument when speed change is decided
  analogWrite(P1, 255);
  analogWrite(P2, 255);

  digitalWrite(D1, LOW);
  digitalWrite(D2, LOW);
}

void right() { //add a speed argument when speed change is decided
  analogWrite(P1, 255);
  analogWrite(P2, 255);

  digitalWrite(D1, HIGH);
  digitalWrite(D2, LOW);
}

void left() { //add a speed argument when speed change is decided
  analogWrite(P1, 255);
  analogWrite(P2, 255);

  digitalWrite(D1, LOW);
  digitalWrite(D2, HIGH);
}

void stop() { //add a speed argument when speed change is decided
  analogWrite(P1, 0);
  analogWrite(P2, 0);

  digitalWrite(D1, HIGH);
  digitalWrite(D2, HIGH);
}