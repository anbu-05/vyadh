#include <HardwareSerial.h>

HardwareSerial uart2(2);

#define enL 26
#define in1L 25
#define in2L 33

#define enR 27
#define in1R 12
#define in2R 14

int speed = 150;
int rate = 0;

void gradual_speed(int final_speed, int rate, bool ifOn = 0) {
  if (ifOn){
    for (int speed = 0; speed < final_speed; speed++) {
      analogWrite(enL, speed);
      analogWrite(enR, speed);
      //Serial.println(speed);
      delay(rate);
    }
  }
  analogWrite(enL, final_speed);
  analogWrite(enR, final_speed);
}

void setup() {
  Serial.begin(115200);
  uart2.begin(115200, SERIAL_8N1, 16, 17); //rx,tx

  pinMode(enL, OUTPUT);
  pinMode(in1L, OUTPUT);
  pinMode(in2L, OUTPUT);

  pinMode(enR, OUTPUT);
  pinMode(in1R, OUTPUT);
  pinMode(in2R, OUTPUT);
  
}

char input = ' ';

void loop() {
  if (uart2.available() > 0) {
    input = uart2.read();
    Serial.println(input);
    switch (input){
      // give a third argument of 1 so that the gradual increase function is activated
      case 'w':
        digitalWrite(in1L, HIGH);
        digitalWrite(in2L, LOW); 
        digitalWrite(in1R, HIGH);
        digitalWrite(in2R, LOW);

        gradual_speed(speed, rate, 1);
        Serial.println("forward");
        break;

      case 's':
        digitalWrite(in1L, LOW);
        digitalWrite(in2L, HIGH); 
        digitalWrite(in1R, LOW);
        digitalWrite(in2R, HIGH); 

        gradual_speed(speed, rate, 1);

        Serial.println("backward");
        break;
      case 'd':
        digitalWrite(in1L, HIGH);
        digitalWrite(in2L, LOW); 
        digitalWrite(in1R, LOW);
        digitalWrite(in2R, HIGH); 

        gradual_speed(speed, rate, 1);

        Serial.println("right");
        break;
      case 'a':
        digitalWrite(in1L, LOW);
        digitalWrite(in2L, HIGH); 
        digitalWrite(in1R, HIGH);
        digitalWrite(in2R, LOW); 

        gradual_speed(speed, rate, 1);

        Serial.println("left");
        break;
      case 'x':
        analogWrite(enL, 0);
        digitalWrite(in1L, HIGH);
        digitalWrite(in2L, LOW); 

        analogWrite(enR, 0);
        digitalWrite(in1R, HIGH);
        digitalWrite(in2R, LOW);
        Serial.println("stopped");
        break;
    }
  }
}
