//#################################
//#################################

//transmitter code

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(4, 5);  // CE, CSN

byte address[6] = "00001";

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

  // begining the module/library
  if (!radio.begin()) {
  Serial.println("radio hardware not responding!");
  while (1) {} // hold program in infinite loop to prevent subsequent errors
  }

  Serial.println("start");

  //the RF module's power level:
  //Options: RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
  radio.setPALevel(RF24_PA_MAX);

  //setting this RF module to transmit mode:
  radio.stopListening();

  // Open the writing pipe with the defined address
  // This is the address the data will be sent to
  // use a different address if you wanna send the data to a different module.
  radio.openWritingPipe(address);

}

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

void loop() {

  // Joystick 1
  payload.joystick1_x = analogRead(joy1_x) / 8;
  payload.joystick1_y = analogRead(joy1_y) / 8;
  payload.Button1 = digitalRead(but1);

  // Joystick 2
  payload.joystick2_x = analogRead(joy2_x) / 8;
  payload.joystick2_y = analogRead(joy2_y) / 8;
  payload.Button2 = digitalRead(but2);

  //joystick 3
  payload.joystick3_x = analogRead(joy3_x) / 8;
  payload.joystick3_y = analogRead(joy3_y) / 8;
  payload.Button3 = digitalRead(but3);

  payload.slide1 = digitalRead(slde1);
  payload.slide2 = digitalRead(slde2);


  Serial.print(payload.joystick1_x);
  Serial.print(",");
  Serial.print(payload.joystick1_y);
  Serial.print(",");
  Serial.print(payload.Button1);
  Serial.println("  ");

  Serial.print(payload.joystick2_x);
  Serial.print(",");
  Serial.print(payload.joystick2_y);
  Serial.print(",");
  Serial.print(payload.Button2);
  Serial.println("  ");

  Serial.print(payload.joystick3_x);
  Serial.print(",");
  Serial.print(payload.joystick3_y);
  Serial.print(",");
  Serial.print(payload.Button3);
  Serial.println("  ");

  Serial.print(payload.slide1);
  Serial.print(",");
  Serial.print(payload.slide2);
  Serial.println("  ");

  Serial.println("  ");
  Serial.println("  ");
  Serial.println("  ");

  radio.write(&payload, sizeof(payload));
  Serial.println(sizeof(payload));

  delay(250);
}