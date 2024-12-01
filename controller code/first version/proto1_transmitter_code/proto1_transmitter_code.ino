//#################################
//#################################

//transmitter code

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(4, 5);  // CE, CSN

byte address[6] = "00001";

#define joy1_x 14
#define joy1_y 12


#define buttons 32

#define joy2_x 26
#define joy2_y 27



void setup() {

  //initializing pins
  // Joystick 1
  pinMode(joy1_x, INPUT);
  pinMode(joy1_y, INPUT);

  //buttons
  pinMode(buttons, INPUT);

  // Joystick 2
  pinMode(joy2_x, INPUT);
  pinMode(joy2_y, INPUT);

  // begining the module/library
  if (!radio.begin()) {
  Serial.println(F("radio hardware not responding!"));
  while (1) {} // hold program in infinite loop to prevent subsequent errors
  }

  Serial.begin(115200);
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
  int joystick1_x;
  int joystick1_y;

  int Button;

  int joystick2_x;
  int joystick2_y;
};

controllerData payload;

void loop() {

  // Joystick 1
  payload.joystick1_x = analogRead(joy1_x);
  payload.joystick1_y = analogRead(joy1_y);

  //buttons
  payload.Button = analogRead(buttons);

  // Joystick 2
  payload.joystick2_x = analogRead(joy2_x);
  payload.joystick2_y = analogRead(joy2_y);

  Serial.print(payload.joystick1_x);
  Serial.print(",");
  Serial.print(payload.joystick1_y);
  Serial.println("  ");

  Serial.print(payload.Button);
  Serial.println(",");

  Serial.print(payload.joystick2_x);
  Serial.print(",");
  Serial.print(payload.joystick2_y);
  Serial.println("  ");

  Serial.println("  ");
  Serial.println("  ");
  Serial.println("  ");

  radio.write(&payload, sizeof(payload));

  delay(20);

}
