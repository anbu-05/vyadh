// low high is going up
// high low is going down
// (encoded IG32) (Just IG32)
// pwmw1, dirw1 for encoded IG

#define RXD2 0
#define TXD2 2

HardwareSerial uart(2);
char previous_recievedChar = 'x';
char recievedChar = 'x';
char inputChar = 'x';

//Linear actuators
//temporary pins
// #define pot1 34
// #define p1 27
// #define dir1 14

// #define pot2 35
// #define p2 25
// #define dir2 26

// #define pot3 4
// #define p3 32
// #define dir3 33

// og pins
#define pot1 34
#define dir1 14
#define p1 27

#define pot2 35
#define dir2 22
#define p2 23

#define pot3 4
#define dir3 16
#define p3 17

// wrist
#define encA 36  
#define encB 39  

// temporary pins 
// #define pwmw1 23  
// #define dirw1 22  
// #define pwmw2 17
// #define dirw2 16

// og pins
#define pwmw1 32   
#define dirw1 33  
#define pwmw2 25
#define dirw2 26

// turn table
#define pwmtt 19 
#define dirtt 18 
int pwm_tt = 0;

// gripper
#define pwmg 21
#define dirg 13
int pwm_gripper = 80;

// angle
float targetAngle;
volatile long posi = 0;
float err = 0;
int pwm_ef = 60;
int pwm_homing = 80;
int ppr = 20000;

// homing limit 
#define limit1 12
int lmt1;
#define limit2 15
int lmt2;

const int freq = 20000;
const int resolution = 8;
int pwm = 200;

float w2 = 0;
float b2; 
int a2 = 0;

float w3 = 0;
float b3; 
int a3 = 0;

float w1 = 0;
float b1; 
int a1 = 0;

int q1,q2,q3;
int c1,c2,c3;
int b11,b22,b33;

float b4;

int values[4];
float predefinedAngles[][4] = {
  {216, 290, 155, 40}, // Predefined angle set 1
  {230, 250, 180, 60}, // Predefined angle set 2
  {260, 300, 240, 30}  // Predefined angle set 3
};

void readEncoder() {
  posi = posi + 1;
}

void killMotors() {
  ledcWrite(p1, 0);
  ledcWrite(p2, 0);  
  ledcWrite(p3, 0);
  for(pwm_tt; pwm_tt > 0; pwm_tt = pwm_tt - 5){
    ledcWrite(pwmtt, pwm_tt);
    delay(60);
  }
  ledcWrite(pwmtt, 0);
  ledcWrite(pwmw1, 0); 
  ledcWrite(pwmw2, 0);  
  ledcWrite(pwmg, 0);
  delay(20);
}

void homing1() {
  lmt1 = analogRead(limit1);
  while(lmt1 < 4000){
    ledcWrite(pwmw1, pwm_homing);
    digitalWrite(dirw1, LOW);
    ledcWrite(pwmw2, pwm_homing);
    digitalWrite(dirw2, HIGH);
    lmt1 = analogRead(limit1);
  }
  posi = 0;
  ledcWrite(pwmw1,0); 
  ledcWrite(pwmw2,0); 
  digitalWrite(dirw1, HIGH);
  ledcWrite(pwmw1, pwm_homing);
  digitalWrite(dirw2, LOW);
  ledcWrite(pwmw2, pwm_homing);
  delay(500);
  ledcWrite(pwmw1,0);
  ledcWrite(pwmw2,0);
  posi = 0;
}

void homing2() { 
  digitalWrite(dirw1, LOW);
  ledcWrite(pwmw1, pwm_homing);
  digitalWrite(dirw2, HIGH);
  ledcWrite(pwmw2, pwm_homing);
  delay(50);
  homing1();
}

void safety_homing() {
  lmt1 = analogRead(limit1);
  lmt2 = analogRead(limit2);
  if(lmt1 > 4000){
    //go down
    digitalWrite(dirw1, HIGH);
    ledcWrite(pwmw1, 100);
    digitalWrite(dirw2, LOW);
    ledcWrite(pwmw2, 100);
    inputChar = 'x';
    killMotors();
  }
  if(lmt2 > 4000){
    //go up 
    digitalWrite(dirw1,LOW);
    ledcWrite(pwmw1, 100);
    digitalWrite(dirw2,HIGH);
    ledcWrite(pwmw2, 100);
    inputChar = 'x';
    killMotors();
  }
}

void moveJoints(float b1, float b2, float b3, float b4) {
  // wrist 
  // low high is going up
  // high low is going down
  while (b4 - posi > 50){
    ledcWrite(pwmw1, pwm_ef);
    digitalWrite(dirw1, HIGH);
    ledcWrite(pwmw2, pwm_ef);
    digitalWrite(dirw2, LOW);  
    // pwm ramping
    pwm_ef = constrain(abs(b4 - posi)/5, 0, 120);
    // safety homing
    lmt1 = analogRead(limit1);
    lmt2 = analogRead(limit2);
    if(lmt1 > 4000){
      homing1();
      break;
    }
    if(lmt2 >4000){
      homing2();
      break;
    }
  }
  ledcWrite(pwmw1, 0);
  ledcWrite(pwmw2, 0);
  pwm_ef = 120;

  int c1 = analogRead(pot1);
  int c2 = analogRead(pot2);
  int c3 = analogRead(pot3);

  int q1 = map(c1, 0, 4095, 0, 300);
  int q2 = map(c2, 0, 4095, 0, 300);
  int q3 = map(c3, 0, 4095, 0, 300);

  digitalWrite(dir1, b1 > q1 ? HIGH : LOW);
  digitalWrite(dir2, b2 > q2 ? LOW : HIGH);
  digitalWrite(dir3, b3 > q3 ? HIGH : LOW);

  long startTime = millis();
  while (abs(q1 - b1) > 2 || abs(q2 - b2) > 2 || abs(q3 - b3) > 2) {
    if (abs(q1 - b1) > 2) {
      ledcWrite(p1, pwm);
    } else {
      ledcWrite(p1, 0);
    }

    if (abs(q2 - b2) > 2) {
      ledcWrite(p2, pwm);
    } else {
      ledcWrite(p2, 0);
    }

    if (abs(q3 - b3) > 2) {
      ledcWrite(p3, pwm);
    } else {
      ledcWrite(p3, 0);
    }

    c1 = analogRead(pot1);
    c2 = analogRead(pot2);
    c3 = analogRead(pot3);

    q1 = map(c1, 0, 4095, 0, 300);
    q2 = map(c2, 0, 4095, 0, 300);
    q3 = map(c3, 0, 4095, 0, 300);
    if(millis() - startTime > 10000){
      startTime = 0;
      break;
    }
  }
  ledcWrite(p1, 0);
  ledcWrite(p2, 0);
  ledcWrite(p3, 0);
  inputChar = 'x';
  Serial.println("error corrected");
}

void setup() {
  Serial.begin(115200);
  uart.begin(115200, SERIAL_8N1, RXD2, TXD2);

  pinMode(pot1, INPUT);
  pinMode(dir1, OUTPUT);
  pinMode(p1, OUTPUT);
  ledcAttachChannel(p1, freq, resolution, 1);

  pinMode(pot2, INPUT);
  pinMode(dir2, OUTPUT); 
  pinMode(p2, OUTPUT);
  ledcAttachChannel(p2, freq, resolution, 2);

  pinMode(pot3, INPUT);
  pinMode(dir3, OUTPUT);
  pinMode(p3, OUTPUT);
  ledcAttachChannel(p3, freq, resolution, 3);

  pinMode(pwmw1,OUTPUT);
  pinMode(dirw1,OUTPUT);
  ledcAttachChannel(pwmw1, freq, resolution, 4);
  
  pinMode(pwmw2,OUTPUT);
  pinMode(dirw2,OUTPUT);
  ledcAttachChannel(pwmw2, freq, resolution, 5);

  pinMode(pwmtt, OUTPUT);
  pinMode(dirtt, OUTPUT);
  ledcAttachChannel(pwmtt, freq, resolution, 6);

  pinMode(pwmg, OUTPUT);
  pinMode(dirg, OUTPUT);
  ledcAttachChannel(pwmg, freq, resolution, 7);

  pinMode(encA,INPUT);
  pinMode(encB,INPUT);
  attachInterrupt(digitalPinToInterrupt(encA), readEncoder, RISING);
  ledcWrite(p1, 0);
  ledcWrite(p2, 0);
  ledcWrite(p3, 0);
  ledcWrite(pwmw1, 0);
  ledcWrite(pwmw2, 0);
  ledcWrite(pwmtt, 0);
  ledcWrite(pwmg, 0);
}

void loop() {
  if (uart.available() > 0) {
    recievedChar = uart.read();
    if (recievedChar != previous_recievedChar) {
      inputChar = recievedChar;
      previous_recievedChar = recievedChar;
    }
  }
  switch (inputChar) {
    case '1':
      killMotors();
      homing1();
      b1 = predefinedAngles[0][0];
      b2 = predefinedAngles[0][1];
      b3 = predefinedAngles[0][2];
      b4 = predefinedAngles[0][3]*ppr/360;
      moveJoints(b1, b2, b3, b4);
      inputChar = 'x';
      break;

    case '2':
      killMotors();
      homing1();
      b1 = predefinedAngles[1][0];
      b2 = predefinedAngles[1][1];
      b3 = predefinedAngles[1][2];
      b4 = predefinedAngles[1][3]*ppr/360;
      moveJoints(b1, b2, b3, b4);
      inputChar = 'x';
      break;

    case '3':
      killMotors(); 
      homing1();
      b1 = predefinedAngles[2][0];
      b2 = predefinedAngles[2][1];
      b3 = predefinedAngles[2][2];
      b4 = predefinedAngles[2][3]*ppr/360;
      moveJoints(b1, b2, b3, b4);
      inputChar = 'x';
      break;
      
    case 'r':
      for(pwm_tt; pwm_tt > 0; pwm_tt = pwm_tt - 5){
        ledcWrite(pwmtt, pwm_tt);
        delay(40);
      }
      digitalWrite(dirtt, HIGH);
      for(pwm_tt; pwm_tt < 80; pwm_tt = pwm_tt + 5){
        ledcWrite(pwmtt, pwm_tt);
        delay(60);
      }
      Serial.println("r");
      ledcWrite(pwmtt, pwm_tt);
      break;

    case 'f':
      for(pwm_tt; pwm_tt > 0; pwm_tt = pwm_tt - 5){
        ledcWrite(pwmtt, pwm_tt);
        delay(40);
      } 
      digitalWrite(dirtt, LOW);
      for(pwm_tt; pwm_tt < 80; pwm_tt = pwm_tt + 5){
        ledcWrite(pwmtt, pwm_tt);
        delay(60);
      }
      Serial.println("f");
      ledcWrite(pwmtt, pwm_tt);
      break;

    case 't':
      Serial.println("t");
      digitalWrite(dir1, HIGH);
      ledcWrite(p1, 200);
      break;

    case 'g':
      Serial.println("g");
      digitalWrite(dir1, LOW);
      ledcWrite(p1, 200);
      break;

    case 'y':
      Serial.println("y");
      digitalWrite(dir2, HIGH);
      ledcWrite(p2, pwm);
      break;

    case 'h':
      Serial.println("h");
      digitalWrite(dir2, LOW);
      ledcWrite(p2, pwm);
      break;

    case 'u':
      Serial.println("u");
      digitalWrite(dir3, HIGH);
      ledcWrite(p3, pwm);
      break;

    case 'j':
      Serial.println("j");
      digitalWrite(dir3, LOW);
      ledcWrite(p3, pwm);
      break;

    case 'w':
      Serial.println("w");
      digitalWrite(dirw1, LOW);
      digitalWrite(dirw2, HIGH);
      ledcWrite(pwmw1, pwm_ef);
      ledcWrite(pwmw2, pwm_ef);
      safety_homing();
      break;

    case 'a':
      Serial.println("a");
      digitalWrite(dirw1, LOW);
      digitalWrite(dirw2, LOW);
      ledcWrite(pwmw1, pwm_ef);
      ledcWrite(pwmw2, pwm_ef);
      safety_homing();
      break;

    case 's':
      Serial.println("s");
      digitalWrite(dirw1, HIGH);
      digitalWrite(dirw2, LOW);
      ledcWrite(pwmw1, pwm_ef);
      ledcWrite(pwmw2, pwm_ef);
      safety_homing();
      break;

    case 'd':
      Serial.println("d"); 
      digitalWrite(dirw1, HIGH);
      digitalWrite(dirw2, HIGH);
      ledcWrite(pwmw1, pwm_ef);
      ledcWrite(pwmw2, pwm_ef);
      safety_homing();
      break;

    case 'i':
      Serial.println("i");
      digitalWrite(dirg, HIGH);
      ledcWrite(pwmg, pwm_gripper);
      break;

    case 'k':
      Serial.println("k");
      digitalWrite(dirg, LOW);
      ledcWrite(pwmg, pwm_gripper);
      break;

    case 'x':
      Serial.println("x");
      killMotors();
      break;
  }
  safety_homing();
}

// single button code just in case 

//convert to IK
// case 'e':
//   switch (gripState) {
//     ledcWrite(pwmg, 0); delay(10);
//     digitalWrite(dirg, HIGH); delay(10);
//     digitalWrite(dirg, LOW); delay(10);
//     case 0: // Open gripper (HIGH direction)
//       digitalWrite(dirg, HIGH);
//       ledcWrite(pwmg, pwm_gripper);
//       Serial.println("Gripper opening");
//       break;

//     case 1: // Stop motor
//       ledcWrite(pwmg, 0);
//       Serial.println("Motor stopped");
//       break;

//     case 2: // Close gripper (LOW direction)
//       digitalWrite(dirg, LOW);
//       ledcWrite(pwmg, pwm_gripper);
//       Serial.println("Gripper closing");
//       break;

//     case 3: // Stop motor
//       ledcWrite(pwmg, 0);
//       Serial.println("Motor stopped");
//       break;
//   }
//   break;