#include <SPI.h>
#include <LoRa.h>

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

//toggles

uint8_t prev_robotic_arm_position = 0;
uint8_t prev_mode_select = 0;
uint8_t prev_mode_select_2 = 0;
uint8_t prev_mode_select_3 = 0;


uint8_t payload[13];


int fast = 255;
int slow = 128;
int speed = 0;

// Pin definitions for LoRa SX1278
#define ss 22         // Slave Select (NSS)
#define rst 21       // Reset
#define dio0 5       // Digital I/O 0

#define P2 32
#define D2 33

#define P1 25
#define D1 26

int traversal_toggle = 1;
int robarm_toggle = 0;
int science_toggle = 0;

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
}

void loop() {

  assignFromLoRa();
  assignFromPayload(payload);
  printValues();

//mode select
  if (mode_select == 1) {
    if (mode_select != prev_mode_select) {
      traversal_toggle = !traversal_toggle;
      prev_mode_select = mode_select;
    }
    if (traversal_toggle) {
      robarm_toggle = 0;
    }
  }

  if (mode_select_2 == 1){
    if (mode_select_2 != prev_mode_select_2) {
      robarm_toggle = !robarm_toggle;
      prev_mode_select_2 = mode_select_2;
    }
    if (robarm_toggle) {
      traversal_toggle = 0;
    }
  }

  if (mode_select_3 == 1) {
    if (mode_select_3 != prev_mode_select_3) {
      science_toggle = !science_toggle;
      prev_mode_select_3 = mode_select_3;
    }
    if (science_toggle) {
      traversal_toggle = 0;
      robarm_toggle = 0;
    }
  }
//
  Serial.println("traversal toggle: " + String(traversal_toggle));
  Serial.println("robotic arm toggle: " + String(robarm_toggle));
  Serial.println("science toggle: " + String(science_toggle));

//traversal
  if (traversal_toggle) {
  //fast control
    if (joystick1_x <= 5) {
      Serial.println("fast");
      Serial.println("left");
      left(fast);
    }

    else if (joystick1_x >= 250) {
      Serial.println("fast");
      Serial.println("right");
      right(fast);
    }
    
    else if (joystick1_y >= 250) {
      Serial.println("fast");
      Serial.println("backward");
      backward(fast);
    }

    else if (joystick1_y <= 5) {
      Serial.println("fast");
      Serial.println("forward");
      forward(fast);
    }

  //slow control

    else if (joystick2_x <= 5) {
      Serial.println("slow");
      Serial.println("left");
      left(slow);
    }

    else if (joystick2_x >= 250) {
      Serial.println("slow");
      Serial.println("right");
      right(slow);
    }
    
    else if (joystick2_y >= 250) {
      Serial.println("slow");
      Serial.println("backward");
      backward(slow);
    }

    else if (joystick2_y <= 5) {
      Serial.println("slow");
      Serial.println("forward");
      forward(slow);
    }

    else {
      Serial.println("stopped");
      stop();
    }
  }

//robotic arm
  if(robarm_toggle) {
    Serial.println("sending robotic arm");
    String prnt = encodeRoboticArm();
    uart.println(prnt);
    Serial.println(prnt);
  }

//science kit
  if(robarm_toggle) {
    Serial.println("sending science kit");
    String prnt = encodeScienceKit();
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
  payload[4] = 1;
  payload[5] = 128;
  payload[6] = 128;
  payload[7] = 1;
  payload[8] = 128;
  payload[9] = 128;
  payload[10] = 1;
  payload[11] = 128;
  payload[12] = 128;
}


void forward(int select_speed) { //add a speed argument when speed change is decided
  //speed = map(speed, 200, 255, 0, 255);
  digitalWrite(D1, HIGH);
  digitalWrite(D2, LOW);

  if (select_speed == fast) {
    for (speed; speed <= fast; speed += 10) {
      analogWrite(P1, speed);
      analogWrite(P2, speed);
    }
  }

  else if (select_speed == slow) {
    for (speed; speed <= slow; speed += 10) {
      analogWrite(P1, speed);
      analogWrite(P2, speed);
    }
  }
}

void backward(int select_speed) { //add a speed argument when speed change is decided

  digitalWrite(D1, LOW);
  digitalWrite(D2, HIGH);

  if (select_speed == fast) {
    for (speed; speed <= fast; speed += 10) {
      analogWrite(P1, speed);
      analogWrite(P2, speed);
    }
  }

  else if (select_speed == slow) {
    for (speed; speed <= slow; speed += 10) {
      analogWrite(P1, speed);
      analogWrite(P2, speed);
    }
  }
}


void right(int select_speed) { //add a speed argument when speed change is decided
  //speed = map(speed, 200, 255, 0, 255);
  digitalWrite(D1, HIGH);
  digitalWrite(D2, HIGH);

  if (select_speed == fast) {
    for (speed; speed <= fast; speed += 10) {
      analogWrite(P1, speed);
      analogWrite(P2, speed);
    }
  }

  else if (select_speed == slow) {
    for (speed; speed <= slow; speed += 10) {
      analogWrite(P1, speed);
      analogWrite(P2, speed);
    }
  }
}


void left(int select_speed) { //add a speed argument when speed change is decided

  digitalWrite(D1, LOW);
  digitalWrite(D2, LOW);

  if (select_speed == fast) {
    for (speed; speed <= fast; speed += 10) {
      analogWrite(P1, speed);
      analogWrite(P2, speed);
    }
  }

  else if (select_speed == slow) {
    for (speed; speed <= slow; speed += 10) {
      analogWrite(P1, speed);
      analogWrite(P2, speed);
    }
  }
}


void stop() { //add a speed argument when speed change is decided

  digitalWrite(D1, HIGH);
  digitalWrite(D2, HIGH);

  for (speed; speed >= 0; speed -= 10) {
    analogWrite(P1, speed);
    analogWrite(P2, speed);
  }
}


char getJoystickCommand(uint8_t a, char axis, char positive, char negative) {
  if (a >= 0 && a < 5) {
    return negative; // Left or down movement
  } else if (a > 250 && a <= 255) {
    return positive; // Right or up movement
  }
  return axis; // Neutral position
}

int robotic_arm_position_timer = 0;

String encodeRoboticArm() {
    char horizontal1Command = getJoystickCommand(joystick1_x, 'x', 'f', 'r');
    char horizontal2Command = getJoystickCommand(joystick2_x, 'x', 'y', 'h');
    char horizontal3Command = getJoystickCommand(joystick3_x, 'x', 'd', 'a');

    char vertical1Command = getJoystickCommand(joystick1_y, 'x', 't', 'g');
    char vertical2Command = getJoystickCommand(joystick2_y, 'x', 'u', 'j');
    char vertical3Command = getJoystickCommand(joystick3_y, 'x', 'w', 's');

    if (robotic_arm_position >= 160 && robotic_arm_position <= 180) {
      robotic_arm_position_timer++;
      Serial.println("command timer: " + robotic_arm_position_timer);
      if (robotic_arm_position_timer > 5) {
        robotic_arm_position_timer = 0;
        return "1";
      }
    }

    else if (robotic_arm_position >= 110 && robotic_arm_position <= 130) {
      robotic_arm_position_timer++;
      Serial.println("command timer: " + robotic_arm_position_timer);
      if (robotic_arm_position_timer > 5) {
        robotic_arm_position_timer = 0;
        return "2";
      }
    }

    else if (robotic_arm_position >= 25 && robotic_arm_position <= 40) {
      robotic_arm_position_timer++;
      Serial.println("command timer: " + robotic_arm_position_timer);
      if (robotic_arm_position_timer > 5) {
        robotic_arm_position_timer = 0;
        return "3";
      }
    }

    else if (Button3 == 0) {
      return "i";
    }

    else if (Button2 == 0) {
      return "k";
    }

    else {
      switch (vertical1Command) {
        case 't':
          return "t";
          break;

        case 'g':
          return "g";
          break;

        case 'x':
          break;
      }

      switch (horizontal1Command) {
        case 'r':
          return "r";
          break;

        case 'f':
          return "f";
          break;

        case 'x':
          break;
      }

      switch (horizontal2Command) {
        case 'y':
          return "y";
          break;

        case 'h':
          return "h";
          break;

        case 'x':
          break;
      }

      switch (vertical2Command) {
        case 'u':
          return "u";
          break;

        case 'j':
          return "j";
          break;

        case 'x':
          break;
      }

      switch (vertical3Command) {
        case 'w':
          return "w";
          break;

        case 's':
          return "s";
          break;

        case 'x':
          break;
      }

    switch (horizontal3Command) {
      case 'a':
        return "a";
        break;

      case 'd':
        return "d";
        break;

      case 'x':
        break;
    }
  }

  return "x";
}

String encodeScienceKit() {
  if (Button1 == 0) {
      return "p";
    }

  else if (Button2 == 0) {
    return "l";
  }
  
  return "x";
}