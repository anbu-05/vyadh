#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <esp_now.h>
#include <WiFi.h>

uint8_t broadcastAddress[] = {0xD4, 0x8A, 0xFC, 0x9D, 0xCB, 0x80}; //reciever's MAC address, (this is sender's code)


struct controllerData{
  uint8_t joystick1_x;
  uint8_t joystick1_y;
  uint8_t Button1;

  uint8_t joystick2_x;
  uint8_t joystick2_y;
  uint8_t Button2;

  uint8_t joystick3_x;
  uint8_t joystick3_y;
  uint8_t Button3;

  uint8_t slide1;
  uint8_t slide2;
};


controllerData payload;

uint8_t pipeNum;

esp_now_peer_info_t slave;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Transmission Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

RF24 radio(4, 5);  // CE, CSN

byte address[6] = "00001";


#define P1 32
#define D1 33

#define P2 26
#define D2 25

void setup() {

  Serial.begin(115200);
  Serial.println("begin");

  //begining the module/library

  if (!radio.begin()) {
    Serial.println(F("radio hardware not responding!"));
  while (1) {} // hold program in infinite loop to prevent subsequent errors
  }

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
  
  //Register peer
  memcpy(slave.peer_addr, broadcastAddress, 6);
  slave.channel = 0;
  slave.encrypt = false;

  if (esp_now_add_peer(&slave) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}


void loop() {
  // payload = {0, 0, 0,
  //            0, 0, 0,
  //            0, 0, 0,
  //            0, 0};
  if (radio.available()) {
    Serial.println("payload available:");
    uint8_t	length = radio.getPayloadSize(); //get the length of the payload from RX FIFO pipe
    
    radio.read(&payload, length); //read the from the pipe the length of the payload from the above line
    Serial.println(length);
    Serial.println(radio.getChannel());

    Serial.println("debug");

    //sending the data through esp now to robotic arm
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &payload, sizeof(payload));

    if (result == ESP_OK) {
    Serial.println("Sent with success");
    }
    else {
      Serial.println("Error sending the data");
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

    if (payload.joystick2_x >= 975) {
      Serial.println("forward");
      forward();
    }

    else if (payload.joystick2_x <= 50) {
      Serial.println("backward");
      backward();
    }
    
    else if (payload.joystick2_y >= 975) {
      Serial.println("right");
      right();
    }

    else if (payload.joystick2_y <= 50) {
      Serial.println("left");
      left();
    }

    else {
      Serial.println("stopped");
      stop();
    }
  }
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