//#################################
//#################################

//transmitter code

#include <HardwareSerial.h>

HardwareSerial HC12(2);

#define RX 18
#define TX 19
#define SET 5

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

void structureToArray(controllerData* payload, uint8_t arr[9]) {
  arr[0] = payload->joystick1_x;
  arr[1] = payload->joystick1_y;
  arr[2] = payload->Button1;
  arr[3] = payload->joystick2_x;
  arr[4] = payload->joystick2_y;
  arr[5] = payload->Button2;
  arr[6] = payload->joystick3_x;
  arr[7] = payload->joystick3_y;
  arr[8] = payload->Button3;
}


void arrayToStructure(uint8_t arr[9], controllerData* payload) {
  payload->joystick1_x = arr[0];
  payload->joystick1_y = arr[1];
  payload->Button1 = arr[2];
  payload->joystick2_x = arr[3];
  payload->joystick2_y = arr[4];
  payload->Button2 = arr[5];
  payload->joystick3_x = arr[6];
  payload->joystick3_y = arr[7];
  payload->Button3 = arr[8];
}

void setup() {

  HC12.begin(115200, SERIAL_8N1, RX, TX);
  Serial.begin(115200);

  pinMode(SET, OUTPUT);

  digitalWrite(SET, HIGH);

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

}


controllerData payload;
uint8_t payloadarr[9]; //match the number of elements in the structure

void loop() {

  // Joystick 1
  payload.joystick1_x = analogRead(joy1_x);
  payload.joystick1_y = analogRead(joy1_y);
  payload.Button1 = digitalRead(but1);

  // Joystick 2
  payload.joystick2_x = analogRead(joy2_x);
  payload.joystick2_y = analogRead(joy2_y);
  payload.Button2 = digitalRead(but2);

  //joystick 3
  payload.joystick3_x = analogRead(joy3_x);
  payload.joystick3_y = analogRead(joy3_y);
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

  structureToArray(&payload, payloadarr);
  HC12.write(payloadarr, sizeof(payload));

  delay(20);
}
