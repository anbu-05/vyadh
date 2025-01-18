//#################################
//#################################

//LoRa by Sandeep Mistry

//transmitter code

#include <SPI.h>
#include <LoRa.h>

// Pin definitions for LoRa SX1278
#define ss 5          // Slave Select (NSS)
#define rst 2        // Reset
#define dio0 4       // Digital I/O 0


#define joy1_x 26
#define joy1_y 27
#define but1 13

#define joy2_x 14
#define joy2_y 12
#define but2 25

#define joy3_x 33
#define joy3_y 32
#define but3 35

#define slde1 39
#define slde2 34




void setup() {

  //initializing pins
  // Joystick 1
  pinMode(joy1_x, INPUT);
  pinMode(joy1_y, INPUT);
  pinMode(but1, INPUT);

  // Joystick 2
  pinMode(joy2_x, INPUT);
  pinMode(joy2_y, INPUT);
  pinMode(but2, INPUT);

  //joystick 3
  pinMode(joy3_x, INPUT);
  pinMode(joy3_y, INPUT);
  pinMode(but3, INPUT);

  //slide switches
  pinMode(slde1, INPUT);
  pinMode(slde2, INPUT);

  Serial.begin(115200);
  Serial.println("begin");
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

uint8_t payload[11];

void loop() {

  // Joystick 1
  joystick1_x = analogRead(joy1_x) >> 4;
  joystick1_y = analogRead(joy1_y) >> 4;
  Button1 = digitalRead(but1) >> 4;

  // Joystick 2
  joystick2_x = analogRead(joy2_x) >> 4;
  joystick2_y = analogRead(joy2_y) >> 4;
  Button2 = digitalRead(but2) >> 4;

  //joystick 3
  joystick3_x = analogRead(joy3_x) >> 4;
  joystick3_y = analogRead(joy3_y) >> 4;
  Button3 = digitalRead(but3) >> 4;

  slide1 = digitalRead(slde1);
  slide2 = digitalRead(slde2);


  Serial.print(joystick1_x);
  Serial.print(",");
  Serial.print(joystick1_y);
  Serial.print(",");
  Serial.print(Button1);
  Serial.println("  ");

  Serial.print(joystick2_x);
  Serial.print(",");
  Serial.print(joystick2_y);
  Serial.print(",");
  Serial.print(Button2);
  Serial.println("  ");

  Serial.print(joystick3_x);
  Serial.print(",");
  Serial.print(joystick3_y);
  Serial.print(",");
  Serial.print(Button3);
  Serial.println("  ");

  Serial.print(slide1);
  Serial.print(",");
  Serial.print(slide2);
  Serial.println("  ");

  Serial.println("  ");
  Serial.println("  ");
  Serial.println("  ");

  assignToPayload(payload);
  
  LoRa.beginPacket();
  LoRa.write(payload, sizeof(payload));
  LoRa.endPacket();

  delay(20);
}

void assignToPayload(uint8_t payload[11]) {
  payload[0] = joystick1_x;
  payload[1] = joystick1_y;
  payload[2] = Button1;

  payload[3] = joystick2_x;
  payload[4] = joystick2_y;
  payload[5] = Button2;

  payload[6] = joystick3_x;
  payload[7] = joystick3_y;
  payload[8] = Button3;

  payload[9] = slide1;
  payload[10] = slide2;
}