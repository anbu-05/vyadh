//#################################
//#################################

//Reciever code


#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <HardwareSerial.h>

//HardwareSerial uart2(2);

RF24 radio(4, 5);  // CE, CSN

byte address[6] = "00001";


#define enL 26
#define in1L 25
#define in2L 33

#define enR 27
#define in1R 12
#define in2R 14

int speed = 150;
int rate = 0;

void gradual_speed(int final_speed, int rate) {
  for (int speed = 0; speed < final_speed; speed++) {
    analogWrite(enL, speed);
    analogWrite(enR, speed);
    //Serial.println(speed);
    delay(rate);
  }
  analogWrite(enL, final_speed);
  analogWrite(enR, final_speed);
}

void setup() {

  //begining the module/library
  if (!radio.begin()) {
    Serial.println(F("radio hardware not responding!"));
  while (1) {} // hold program in infinite loop to prevent subsequent errors
  }

  Serial.begin(115200);
  //uart2.begin(115200, SERIAL_8N1, 16, 17); //rx,tx
  Serial.println("start");

  //the RF module's power level:
  //Options: RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
  radio.setPALevel(RF24_PA_MAX);

  //opening reading "pipe" no.1 at the given address
  radio.openReadingPipe(1, address);

  //setting this RF module to recieve mode:
  radio.startListening();


  pinMode(enL, OUTPUT);
  pinMode(in1L, OUTPUT);
  pinMode(in2L, OUTPUT);

  pinMode(enR, OUTPUT);
  pinMode(in1R, OUTPUT);
  pinMode(in2R, OUTPUT);
}

struct controllerpayload{
  int joystick1_x;
  int joystick1_y;

  int Button;

  int joystick2_x;
  int joystick2_y;
};

int[2] direction_vector = {0,0};

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

    //-------------------------button--------------------

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


    //------------digital inputs-------------------------
    /*
    if (payload.joystick2_x >= 3900) {
      Serial.println("forward");
      //uart2.println("w");
      forward();
    }

    else if (payload.joystick2_x <= 200) {
      Serial.println("backward");
      //uart2.println("s");
      backward();
    }
    
    else if (payload.joystick2_y >= 3900) {
      Serial.println("right");
      //uart2.println("d");
      right();
    }

    else if (payload.joystick2_y <= 200) {
      Serial.println("left");
      //uart2.println("a");
      left();
    }

    else {
      Serial.println("stopped");
      //uart2.println("x");
      stop();
    }
    */

    //------------------analog inputs----------------------------
    direction_vector = {payload.joystick1_x-2048, payload.joystick1_y-2048};
    
    
  }
}

//------------digital inputs-------------------------
void forward() { 
  digitalWrite(in1L, HIGH);
  digitalWrite(in2L, LOW); 
  digitalWrite(in1R, HIGH);
  digitalWrite(in2R, LOW);

  gradual_speed(speed, rate);
  Serial.println("forward");
}

void backward() { 
  digitalWrite(in1L, LOW);
  digitalWrite(in2L, HIGH); 
  digitalWrite(in1R, LOW);
  digitalWrite(in2R, HIGH); 

  gradual_speed(speed, rate);
  Serial.println("backward");
}

void right() { 
  digitalWrite(in1L, HIGH);
  digitalWrite(in2L, LOW); 
  digitalWrite(in1R, LOW);
  digitalWrite(in2R, HIGH); 

  gradual_speed(speed, rate);
  Serial.println("right");
}

void left() { 
  digitalWrite(in1L, LOW);
  digitalWrite(in2L, HIGH); 
  digitalWrite(in1R, HIGH);
  digitalWrite(in2R, LOW); 

  gradual_speed(speed, rate);
  Serial.println("left");
}

void stop() { 
  analogWrite(enL, 0);
  digitalWrite(in1L, HIGH);
  digitalWrite(in2L, LOW); 

  analogWrite(enR, 0);
  digitalWrite(in1R, HIGH);
  digitalWrite(in2R, LOW);
  Serial.println("stopped");
}

//------------------analog inputs----------------------------

















