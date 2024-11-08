int dir1 = 33;  // D1 in the second code
int pwm1 = 32;  // P1 in the second code

int dir2 = 26;  // D2 in the second code
int pwm2 = 25;  // P2 in the second code

const int freq = 20000;
const int pwmChannel1 = 0;
const int pwmChannel2 = 1;
const int resolution = 8;
int pwm = 128;

void setup() {
  pinMode(dir1, OUTPUT);
  pinMode(pwm1, OUTPUT);
  pinMode(dir2, OUTPUT);
  pinMode(pwm2, OUTPUT);

  // Attach channels to pins with corresponding PWM settings
  ledcAttachPin(pwm1, pwmChannel1);
  ledcAttachPin(pwm2, pwmChannel2);

  ledcSetup(pwmChannel1, freq, resolution);
  ledcSetup(pwmChannel2, freq, resolution);

  Serial.begin(9600);
}

void loop() {
  if (Serial.available() > 0) {
    char a = Serial.read();
    switch (a) {
      case 'w':  // Move forward
        digitalWrite(dir1, HIGH);
        digitalWrite(dir2, HIGH);
        gradualSpeedUp(pwm);
        break;

      case 's':  // Move backward
        digitalWrite(dir1, LOW);
        digitalWrite(dir2, LOW);
        gradualSpeedUp(pwm);
        break;

      case 'a':  // Move left
        digitalWrite(dir1, LOW);
        digitalWrite(dir2, HIGH);
        gradualSpeedUp(pwm);
        break;

      case 'd':  // Move right
        digitalWrite(dir1, HIGH);
        digitalWrite(dir2, LOW);
        gradualSpeedUp(pwm);
        break;

      case 'x':  // Stop
        gradualSpeedDown();
        break;
    }
  }
}

void gradualSpeedUp(int final_speed) {
  for (int i = 0; i <= final_speed; i++) {
    ledcWrite(pwmChannel1, i);
    ledcWrite(pwmChannel2, i);
    delay(5);
  }
}

void gradualSpeedDown() {
  for (int i = pwm; i >= 0; i--) {
    ledcWrite(pwmChannel1, i);
    ledcWrite(pwmChannel2, i);
    delay(10);
  }
}
