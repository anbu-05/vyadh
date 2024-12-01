//#################################
//#################################

//Reciever code

//current status of the code: im using this as a reciever module and communicating to another esp32 using uart

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <HardwareSerial.h>

HardwareSerial uart2(2);

RF24 radio(4, 5);  // CE, CSN

byte address[6] = "00001";

/*
#define PWM2 22
#define DIR2 21

#define PWM1 27
#define DIR1 14

#define PWM3 25
#define DIR3 26

#define PWM4 32
#define DIR4 33
*/

String speed; //change this to int later

void setup() {

  //begining the module/library
  if (!radio.begin()) {
    Serial.println(F("radio hardware not responding!"));
  while (1) {} // hold program in infinite loop to prevent subsequent errors
  }

  Serial.begin(115200);
  uart2.begin(115200, SERIAL_8N1, 16, 17); //rx,tx
  Serial.println("start");

  //the RF module's power level:
  //Options: RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
  radio.setPALevel(RF24_PA_MAX);

  //opening reading "pipe" no.1 at the given address
  radio.openReadingPipe(1, address);

  //setting this RF module to recieve mode:
  radio.startListening();

/*
  pinMode(PWM1, OUTPUT);
  pinMode(PWM2, OUTPUT);
  pinMode(PWM3, OUTPUT);
  pinMode(PWM4, OUTPUT);

  pinMode(DIR1, OUTPUT);
  pinMode(DIR2, OUTPUT);
  pinMode(DIR3, OUTPUT);
  pinMode(DIR4, OUTPUT);
  */
}

struct controllerpayload{
  int joystick1_x;
  int joystick1_y;

  int Button;

  int joystick2_x;
  int joystick2_y;
};


void loop() {
  
  controllerpayload payload;
  uint8_t pipe1;  //create a buffer to which the 1st pipe writes to
  if (radio.available(&pipe1)){  //see if there are any bytes available to read on pipe1
    Serial.println("payload available:");
    uint8_t	length = radio.getDynamicPayloadSize(); //get the length of the payload from RX FIFO pipe
    Serial.println(length);
    radio.read(&payload, length); //read the from the pipe the length of the payload from the above line

    //printing the joystick values:

    Serial.print("Joystick1 X: "); Serial.print(payload.joystick1_x);
    Serial.print(", Joystick1 Y: "); Serial.println(payload.joystick1_y);

    Serial.print("button value: "); Serial.print(payload.Button); Serial.println(", ");
  
    Serial.print("Joystick2 X: "); Serial.print(payload.joystick2_x);
    Serial.print(", Joystick2 Y: "); Serial.println(payload.joystick2_y);


    int button = payload.Button;

    if ((button >= 700) && (button <= 800)) {
      speed = "speed 1"; //give speed values later
    }

    else if ((button >= 1600) && (button <= 1700)) {
      speed = "speed 2";
    }

    else if ((button >= 2500) && (button <= 2600)) {
      speed = "speed 3";
    }

    else if (button == 4095) {
      speed = "speed 4";
    }

    Serial.print("speed: "); Serial.println(speed);
    
    
    Serial.println("  ");
    Serial.println("  ");
    Serial.println("  ");

    if (payload.joystick2_x >= 3900) {
      Serial.println("forward");
      uart2.println("w");
      //forward();
    }

    else if (payload.joystick2_x <= 200) {
      Serial.println("backward");
      uart2.println("s");
      //backward();
    }
    
    else if (payload.joystick2_y >= 3900) {
      Serial.println("right");
      uart2.println("d");
      //right();
    }

    else if (payload.joystick2_y <= 200) {
      Serial.println("left");
      uart2.println("a");
      //left();
    }

    else {
      Serial.println("stopped");
      uart2.println("x");
      //stop();
    }
  }
}

/*
void forward() { //add a speed argument when speed change is decided
  analogWrite(PWM1, 150);
  analogWrite(PWM2, 150);
  analogWrite(PWM3, 150);
  analogWrite(PWM4, 150);

  digitalWrite(DIR1, HIGH);
  digitalWrite(DIR2, HIGH);
  digitalWrite(DIR3, HIGH);
  digitalWrite(DIR4, HIGH);
}

void backward() { //add a speed argument when speed change is decided
  analogWrite(PWM1, 150);
  analogWrite(PWM2, 150);
  analogWrite(PWM3, 150);
  analogWrite(PWM4, 150);

  digitalWrite(DIR1, LOW);
  digitalWrite(DIR2, LOW);
  digitalWrite(DIR3, LOW);
  digitalWrite(DIR4, LOW);
}

void right() { //add a speed argument when speed change is decided
  analogWrite(PWM1, 255);
  analogWrite(PWM2, 255);
  analogWrite(PWM3, 255);
  analogWrite(PWM4, 255);

  digitalWrite(DIR1, HIGH);
  digitalWrite(DIR2, LOW);
  digitalWrite(DIR3, HIGH);
  digitalWrite(DIR4, LOW);
}

void left() { //add a speed argument when speed change is decided
  analogWrite(PWM1, 255);
  analogWrite(PWM2, 255);
  analogWrite(PWM3, 255);
  analogWrite(PWM4, 255);

  digitalWrite(DIR1, LOW);
  digitalWrite(DIR2, HIGH);
  digitalWrite(DIR3, LOW);
  digitalWrite(DIR4, HIGH);
}

void stop() { //add a speed argument when speed change is decided
  analogWrite(PWM1, 0);
  analogWrite(PWM2, 0);
  analogWrite(PWM3, 0);
  analogWrite(PWM4, 0);

  digitalWrite(DIR1, HIGH);
  digitalWrite(DIR2, HIGH);
  digitalWrite(DIR3, HIGH);
  digitalWrite(DIR4, HIGH);
}
*/