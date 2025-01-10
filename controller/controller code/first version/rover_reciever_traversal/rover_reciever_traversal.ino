//#################################
//#################################

//Reciever code

//current status of the code: this does both reception and locomotion

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(4, 5);  // CE, CSN

byte address[6] = "00001";


#define P1 32
#define D1 33

#define P2 26
#define D2 25


String speed; //change this to int later

void setup() {

  //begining the module/library
  // if (!radio.begin()) {
  //   Serial.println(F("radio hardware not responding!"));
  //   while (1) {} // hold program in infinite loop to prevent subsequent errors
  // }

  Serial.begin(115200);
  Serial.println("start");

  radio.begin();

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
  }
}


void forward() { //add a speed argument when speed change is decided
  analogWrite(P1, 200);
  analogWrite(P2, 200);

  digitalWrite(D1, HIGH);
  digitalWrite(D2, HIGH);
}

void backward() { //add a speed argument when speed change is decided
  analogWrite(P1, 200);
  analogWrite(P2, 200);

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
