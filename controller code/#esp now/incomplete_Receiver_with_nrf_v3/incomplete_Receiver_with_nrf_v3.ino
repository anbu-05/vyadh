#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <esp_now.h>
#include <WiFi.h>

uint8_t broadcastAddress[] = {0xEC, 0x64, 0xC9, 0x5E, 0x11, 0x3C}; //reciever's MAC address, (this is sender's code)


struct joystickData{
  int joystick1_x;
  int joystick1_y;
//  int Button1;

  int joystick2_x;
  int joystick2_y;
//  int Button2;

  int joystick3_x;
  int joystick3_y;
//  int Button3;
};

struct slideData{
  int slide1;
  int slide2;
};

joystickData joystickPayload;
slideData slidePayload;

esp_now_peer_info_t slave;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Transmission Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

RF24 radio(4, 5);  // CE, CSN

byte address[6] = "822081"; //maybe an address that's not 000001 will help


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
  joystickPayload = {0, 0,
                     0, 0,
                     0, 0};
  slidePayload = {0, 0};
  uint8_t pipe1;  //create a buffer to which the 1st pipe writes to
  if (radio.available(&pipe1)) {  //see if there are any bytes available to read on pipe1
    //Serial.println("payload available:");
    
    // uint8_t	length = radio.getDynamicPayloadSize(); //get the length of the payload from RX FIFO pipe
    // Serial.println(length);
    
    //read the from the pipe the length of the payload from the above line
    // Read joystick data
    esp_err_t result;
    if (radio.getPayloadSize() == sizeof(joystickPayload)) {
      radio.read(&joystickPayload, sizeof(joystickPayload));
      //sending the data through esp now to robotic arm
      //result = esp_now_send(broadcastAddress, (uint8_t *) &joystickPayload, sizeof(joystickPayload));
    }

    // Read slide data
    if (radio.getPayloadSize() == sizeof(slidePayload)) {
      radio.read(&slidePayload, sizeof(slidePayload));
      //sending the data through esp now to robotic arm
      //result = esp_now_send(broadcastAddress, (uint8_t *) &slidePayload, sizeof(slidePayload));
    }
    
    Serial.println(radio.getChannel());


    // if (result == ESP_OK) {
    //   Serial.println("Sent with success");
    //   }
    // else {
    //   Serial.println("Error sending the data");
    //   }
  }
  
  if (!slidePayload.slide2) {
    //traversal code
    //printing the joystick values: - comment this if the rx tx port is needed

    Serial.print("Joystick1 X: "); Serial.print(joystickPayload.joystick1_x);
    Serial.print(", Joystick1 Y: "); Serial.println(joystickPayload.joystick1_y);
    //Serial.print(", button1: "); Serial.println(joystickPayload.Button1);

    Serial.print("Joystick2 X: "); Serial.print(joystickPayload.joystick2_x);
    Serial.print(", Joystick2 Y: "); Serial.println(joystickPayload.joystick2_y);
    //Serial.print(", button2: "); Serial.println(joystickPayload.Button2);

    Serial.print("Joystick3 X: "); Serial.print(joystickPayload.joystick3_x);
    Serial.print(", Joystick3 Y: "); Serial.println(joystickPayload.joystick3_y);
    //Serial.print(", button3: "); Serial.println(joystickPayload.Button3);

    Serial.print("slide switch 1: "); Serial.println(slidePayload.slide1);
    Serial.print("slide switch 2: "); Serial.println(slidePayload.slide2);

    Serial.println("  ");
    Serial.println("  ");
    Serial.println("  ");

    if (joystickPayload.joystick2_x >= 3900) {
      Serial.println("forward");
      forward();
    }

    else if (joystickPayload.joystick2_x <= 200) {
      Serial.println("backward");
      backward();
    }
    
    else if (joystickPayload.joystick2_y >= 3900) {
      Serial.println("right");
      right();
    }

    else if (joystickPayload.joystick2_y <= 200) {
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