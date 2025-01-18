#include <SPI.h>
#include <LoRa.h>

//uint8_t broadcastAddress[] = {0xEC, 0x64, 0xC9, 0x5E, 0x11, 0x3C}; //reciever's MAC address, (this is sender's code)

#define RXD2 17
#define TXD2 16

uint8_t joystick1_x = 0;
uint8_t joystick1_y = 0;
uint8_t Button1 = 0;

uint8_t joystick2_x = 0;
uint8_t joystick2_y = 0;
uint8_t Button2 = 0;

uint8_t joystick3_x = 0;
uint8_t joystick3_y = 0;
uint8_t Button3 = 0;

uint8_t robotic_arm_position = 0;
uint8_t mode_select = 0;
uint8_t mode_select_2 = 0;
uint8_t mode_select_3 = 0;


uint8_t payload[13];

//esp_now_peer_info_t slave;

int speed = 255;
int slow = 128;
int fast_delay_rate = 3; //delay between each gradual increase step
int slow_delay_rate = 1;

// Pin definitions for LoRa SX1278
#define ss 22         // Slave Select (NSS)
#define rst 21       // Reset
#define dio0 5       // Digital I/O 0

#define P2 32
#define D2 33

#define P1 25
#define D1 26

int traversal_toggle = 0;
int robarm_toggle = 0;
int science_toggle = 0;

void gradual_speed(int final_speed, int delay_rate, bool ifOn = 0) {
  int gradual_increase_flag = 0;
  if (ifOn){
    for (int speed = 0; speed < final_speed; speed++) {
      analogWrite(P1, speed);
      analogWrite(P2, speed);
      Serial.println(speed);
      delay(delay_rate);
      gradual_increase_flag = gradual_inrease_interrupt();
      if (gradual_increase_flag) {
        break;
      }
    }
  }
  analogWrite(P1, final_speed);
  analogWrite(P2, final_speed);
}

int gradual_inrease_interrupt() {

  assignFromLoRa();
  assignFromPayload(payload);

  if (joystick1_x >= 5 || joystick1_x <= 250) {
    return 1;
  }

  if (joystick1_y >= 5 || joystick1_y <= 250) {
    return 1;
  }

  return 0;
}

// void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
//   Serial.print("Transmission Status: ");
//   Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
// }

HardwareSerial uart(1);

void setup() {
  Serial.begin(115200);
  Serial.println("1");
  uart.begin(115200, SERIAL_8N1, RXD2, TXD2);
  Serial.println("2"); 

  Serial.println("LoRa Receiver Setup");

  // Configure LoRa module
  LoRa.setPins(ss, rst, dio0);

  if (!LoRa.begin(433E6)) { // Set frequency to 433 MHz
    Serial.println("Starting LoRa failed! Check connections and power supply.");
    while (1);
  }

  Serial.println("LoRa initialization successful!");

  pinMode(P1, OUTPUT);
  pinMode(P2, OUTPUT);

  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);

  assignFromPayload(payload);
  resetPayload(payload);

  // WiFi.mode(WIFI_STA);
  
  // if (esp_now_init() != ESP_OK) {
  //   Serial.println("Error initializing ESP-NOW");
  //   return;
  // }

  // esp_now_register_send_cb(OnDataSent);
  
  // //Register peer
  // memcpy(slave.peer_addr, broadcastAddress, 6);
  // slave.channel = 0;
  // slave.encrypt = false;

  // if (esp_now_add_peer(&slave) != ESP_OK){
  //   Serial.println("Failed to add peer");
  //   return;
  // }
}

void loop() {

  assignFromLoRa();
  assignFromPayload(payload);
  printValues();

  if (mode_select == 1){
      robarm_toggle = !robarm_toggle;
    }

  if (mode_select_2 == 1) {
    traversal_toggle = !traversal_toggle;
  }

  if (mode_select_3 == 1) {
    science_toggle = !science_toggle;
  }

  Serial.println("traversal toggle: " + String(traversal_toggle));
  Serial.println("robotic arm toggle: " + String(robarm_toggle));
  Serial.println("science toggle: " + String(science_toggle));

  if (traversal_toggle) {
  //fast control
    if (joystick1_x <= 5) {
      Serial.println("left");
      left(speed, fast_delay_rate);
    }

    else if (joystick1_x >= 250) {
      Serial.println("right");
      right(speed, fast_delay_rate);
    }
    
    else if (joystick1_y >= 250) {
      Serial.println("backward");
      backward(speed, fast_delay_rate);
    }

    else if (joystick1_y <= 5) {
      Serial.println("forward");
      forward(speed, fast_delay_rate);
    }

    else {
      Serial.println("stopped");
      stop();
    }

  //slow control

    if (joystick2_x <= 5) {
      Serial.println("left");
      left(slow, slow_delay_rate);
    }

    else if (joystick2_x >= 250) {
      Serial.println("right");
      right(slow, slow_delay_rate);
    }
    
    else if (joystick2_y >= 250) {
      Serial.println("backward");
      backward(slow, slow_delay_rate);
    }

    else if (joystick2_y <= 5) {
      Serial.println("forward");
      forward(slow, slow_delay_rate);
    }

    else {
      Serial.println("stopped");
      stop();
    }
  } 

  if(robarm_toggle) {
    Serial.println("sending robotic arm");
    String prnt = encodeRoboticArm();
    uart.println(prnt);
    Serial.println(prnt);
  }

}

void assignFromPayload(uint8_t payload[13]) {
  mode_select_3 = payload[0];
  mode_select_2 = payload[1];
  mode_select = payload[2];
  robotic_arm_position = payload[3];

  Button3 = payload[4];
  joystick3_y = payload[5];
  joystick3_x = payload[6];

  Button2 = payload[7];
  joystick2_y = payload[8];
  joystick2_x = payload[9];

  Button1 = payload[10];
  joystick1_y = payload[11];
  joystick1_x = payload[12];
}

void assignFromLoRa() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    Serial.println("###Received packet###");

    // Read packet contents 
    int i = LoRa.available();
    while (i) {
      //Serial.println(i);
      payload[i-1] = LoRa.read();
      //Serial.println(payload[i-1]);
      i--;
    }

    // Print RSSI (signal strength)
    Serial.print("RSSI: ");
    Serial.println(LoRa.packetRssi());
    Serial.println(LoRa.packetFrequencyError());

    //sending the data through esp now to robotic arm
    //esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &payload, sizeof(payload));

    // if (result == ESP_OK) {
    // Serial.println("Sent with success");
    // }
    // else {
    //   Serial.println("Error sending the data");
    // }
  }
}

void printValues() {
  Serial.print(joystick1_x); Serial.print(","); Serial.print(joystick1_y); Serial.print(","); Serial.print(Button1); Serial.println("  ");
  Serial.print(joystick2_x); Serial.print(","); Serial.print(joystick2_y); Serial.print(","); Serial.print(Button2); Serial.println("  ");
  Serial.print(joystick3_x); Serial.print(","); Serial.print(joystick3_y); Serial.print(","); Serial.print(Button3); Serial.println("  ");
  Serial.print(robotic_arm_position); Serial.print(","); Serial.print(mode_select); Serial.print(","); Serial.print(mode_select_2); Serial.print(","); Serial.print(mode_select_3); Serial.println("  "); 

  Serial.println("  ");
  Serial.println("  ");
  Serial.println("  ");
}

void resetPayload(uint8_t payload[13]) {
  payload[0] = 0;
  payload[1] = 0;
  payload[2] = 0;
  payload[3] = 0;
  payload[4] = 0;
  payload[5] = 128;
  payload[6] = 128;
  payload[7] = 0;
  payload[8] = 128;
  payload[9] = 128;
  payload[10] = 0;
  payload[11] = 128;
  payload[12] = 128;
}

//movement functions

void forward(int speed, int delay_rate) { //add a speed argument when speed change is decided
  //speed = map(speed, 200, 255, 0, 255);
  digitalWrite(D1, HIGH);
  digitalWrite(D2, LOW);

  gradual_speed(speed, delay_rate, 1);
}

void backward(int speed, int delay_rate) { //add a speed argument when speed change is decided
  //speed = map(speed, 50, 0, 0, 255);
  digitalWrite(D1, LOW);
  digitalWrite(D2, HIGH);

  gradual_speed(speed, delay_rate, 1);
}


void right(int speed, int delay_rate) { //add a speed argument when speed change is decided
  //speed = map(speed, 200, 255, 0, 255);
  digitalWrite(D1, HIGH);
  digitalWrite(D2, HIGH);

  gradual_speed(speed, delay_rate, 1);
}


void left(int speed, int delay_rate) { //add a speed argument when speed change is decided
  //speed = map(speed, 50, 0, 0, 255);
  digitalWrite(D1, LOW);
  digitalWrite(D2, LOW);

  gradual_speed(speed, delay_rate, 1);
}


void stop() { //add a speed argument when speed change is decided
  digitalWrite(D1, HIGH);
  digitalWrite(D2, HIGH);

  gradual_speed(0, 1, 1);
}

char getJoystickCommand(uint8_t a, char axis, char positive, char negative) {
  if (a >= 0 && a < 5) {
    return negative; // Left or down movement
  } else if (a > 250 && a <= 255) {
    return positive; // Right or up movement
  }
  return axis; // Neutral position
}

String encodeRoboticArm(){
    char horizontal1Command = getJoystickCommand(joystick1_x, 'x', 'f', 'r');
    char horizontal2Command = getJoystickCommand(joystick2_x, 'x', 'y', 'h');
    char horizontal3Command = getJoystickCommand(joystick3_x, 'x', 'd', 'a');

    char vertical1Command = getJoystickCommand(joystick1_y, 'x', 't', 'g');
    char vertical2Command = getJoystickCommand(joystick2_y, 'x', 'u', 'j');
    char vertical3Command = getJoystickCommand(joystick3_y, 'x', 'w', 's');

    if (robotic_arm_position >= 160 && robotic_arm_position <= 180) {
      return "p1--";
    }

    else if (robotic_arm_position >= 110 && robotic_arm_position <= 130) {
      return "p2--";
    }

    else if (robotic_arm_position >= 25 && robotic_arm_position <= 40) {
      return "p3--";
    }

    else{
      switch (vertical1Command) {
        case 't':
          return "r-t-";
          break;

        case 'g':
          return "r-g-";
          break;

        case 'x':
          break;
      }

      switch (horizontal2Command) {
        case 'y':
          return "r-y-";
          break;

        case 'h':
          return "r-h-";
          break;

        case 'x':
          break;
      }

      switch (vertical2Command) {
        case 'u':
          return "r-u-";
          break;

        case 'j':
          return "r-j-";
          break;

        case 'x':
          break;
      }

      switch (vertical3Command) {
        case 'w':
          return "w--w";
          break;

        case 's':
          return "w--s";
          break;

        case 'x':
          break;
      }

    switch (horizontal3Command) {
      case 'a':
        return "w--a";
        break;

      case 'd':
        return "w--d";
        break;

      case 'x':
        break;
    }
  }

  return "x---";
}