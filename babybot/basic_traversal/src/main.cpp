#include <Arduino.h>

const int DIR1 = 26, PWM1 = 25;
const int DIR2 = 33, PWM2 = 32;

int speed = 0, turn = 0;

void setMotor(int dir, int pwm, int val) {
  digitalWrite(dir, val >= 0);
  analogWrite(pwm, min(abs(val), 255));
}

void setup() {
  pinMode(DIR1, OUTPUT); pinMode(PWM1, OUTPUT);
  pinMode(DIR2, OUTPUT); pinMode(PWM2, OUTPUT);
  Serial.begin(9600);
  // Serial.println("Use W/A/S/D to move, Space to stop.");
}

void loop() {
  if (Serial.available()) {
    char c = Serial.read();
    Serial.println(c);

    switch (c) {
      case 'w': speed += 32; break;
      case 's': speed -= 32; break;
      case 'a': turn -= 16; break;
      case 'd': turn += 16; break;
      case ' ': speed = 0; turn = 0; break;
    }

    speed = constrain(speed, -224, 224);
    turn = constrain(turn, -64, 64);

    int l = speed - turn;
    int r = speed + turn;

    setMotor(DIR1, PWM1, l);
    setMotor(DIR2, PWM2, r);
  }
}

