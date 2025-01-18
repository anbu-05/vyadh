#define RXD2 0
#define TXD2 2

// controls
#define pot1 34
#define pot2 35
#define pot3 4
// homing limit 
int zero_pulse = 250;
#define limit 12
#define encA 36  
#define encB 39 

// ig 32 
#define pwmw1 32   
#define dirw1 33  
#define pwmw2 25
#define dirw2 26

// linear actuators
#define p1 27
#define dir1 14
#define p2 23
#define dir2 22
#define p3 17
#define dir3 16

// turn table
#define pwmtt 19
#define dirtt 18

// gripper
#define pwmg 21
#define dirg 13
bool grip = false;
int gripState = 0;  // Tracks state: 0 = open, 1 = stop, 2 = close, 3 = stop

int lmt;
float targetAngle;
volatile long posi = 0;
float err = 0;
const int freq = 20000;
const int resolution = 8;
int ppr = 20000;

// pwm values
int pwm_ef = 150; // not const because it changes 
const int pwm_homing = 80;
const int pwm = 200;
const int pwm_tt = 60;
const int pwm_gripper = 80;

float b1; 
float b2; 
float b3;
float b4;

float w2 = 0;; 
int a2 = 0;

float w3 = 0;
int a3 = 0;

float w1 = 0;
int a1 = 0;

int q1,q2,q3;
int c1,c2,c3;
int b11,b22,b33;


int values[4];
float predefinedAngles[][4] = {
  {216, 290, 155, 40}, // Predefined angle set 1
  {230, 250, 180, 60}, // Predefined angle set 2
  {260, 300, 240, 30}  // Predefined angle set 3
};

void moveJoints(float b1, float b2, float b3, float b4) {
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
    if(millis()-startTime > 10000){
      startTime = 0;
      break;
    }
  }
  
  ledcWrite(p1, 0);
  ledcWrite(p2, 0);
  ledcWrite(p3, 0);

  // wrist 
  err = abs(b4 - posi);
  if(err > 100) {
    while (b4 > posi + 50){
      ledcWrite(pwmw1, pwm_ef);
      digitalWrite(dirw1, LOW);
      ledcWrite(pwmw2, pwm_ef);
      digitalWrite(dirw2, HIGH);
      Serial.print("target: "+(String)b4); Serial.println ("; posi: "+(String)posi);
      //pwm ramping
      pwm_ef = constrain(abs(b4 - posi)/5, 0, 120);
      //safety homing
      // lmt = analogRead(limit);
      // if(lmt > 4000){
      //   //homing();
      //   lmt=0;
      //   break;
      // }
    }
    while (b4 < posi - 50){
      ledcWrite(pwmw1, pwm_ef);
      digitalWrite(dirw1, HIGH);
      ledcWrite(pwmw2, pwm_ef);
      digitalWrite(dirw2, LOW);
      Serial.print("target: "+(String)b4); Serial.println ("; posi: "+(String)posi);
      //pwm ramping
      pwm_ef = constrain(abs(b4 - posi)/5, 0, 120);
      //safety homing
      // lmt = analogRead(limit);
      // if(lmt > 4000){
      //   //homing();
      //   lmt = 0;
      //   break;
      // }
    }
    ledcWrite(pwmw1, 0);
    ledcWrite(pwmw2, 0);
    pwm_ef = 150;
    Serial.println("error corrected");
  }
}

HardwareSerial uart(2);

void setup(){
  Serial.begin(115200);
  uart.begin(115200, SERIAL_8N1, RXD2, TXD2);

  pinMode(dir1,OUTPUT);
  pinMode(p1,OUTPUT);
  ledcAttachChannel(p1, freq, resolution, 1);

  pinMode(dir2,OUTPUT);
  pinMode(p2,OUTPUT);
  ledcAttachChannel(p2, freq, resolution, 2);

  pinMode(dir3,OUTPUT);
  pinMode(p3,OUTPUT);
  ledcAttachChannel(p3, freq, resolution, 3);
  
  pinMode(pwmw1,OUTPUT);
  pinMode(dirw1,OUTPUT);
  ledcAttachChannel(pwmw1, freq, resolution, 4);

  pinMode(pwmw2,OUTPUT);
  pinMode(dirw2,OUTPUT);
  ledcAttachChannel(pwmw2, freq, resolution, 5);

  pinMode(encA,INPUT);
  pinMode(encB,INPUT);
  attachInterrupt(digitalPinToInterrupt(encA), readEncoder, RISING);

  pinMode(pot1, INPUT);
  pinMode(pot2, INPUT);
  pinMode(pot3, INPUT);
  
  //homing();
  // Register callback to receive data
  //esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
}

void readEncoder() {
  int stateB = digitalRead(encB);
  if (stateB > 0){
    posi = posi + 1;
  }else{
    posi = posi - 1;
  }
}

String previous_recievedString = "";
String recievedString = "";
char filter = 0;
char inputChar = 'x';

void loop() {
  if (uart.available()) {
    recievedString =  uart.readStringUntil('\n');
  }

  if (recievedString != previous_recievedString) {
    Serial.println("change");
    previous_recievedString = recievedString;
  }
  else {
    if (recievedString.length() >= 2) {
      filter = recievedString[0];      // First character
      Serial.println(filter);

      switch (filter) {
        case 'p':
          Serial.println("position");
          inputChar = recievedString[1];
          break;
        case 'r':
          Serial.println("actuators");
          inputChar = recievedString[2];
          break;
        case 'w':
          Serial.println("wrist");
          inputChar = recievedString[3];
          break;
        default:
          Serial.println("stop");
          inputChar = 'x';
          break;
      }
      // else {
      //   inputChar = 'x';
      //   Serial.println("all motors killed");
      // }
      if (inputChar == 'e') {
        gripState = (gripState + 1) % 4;
      }
    }  
  }
   

  Serial.print("inputChar: ");
  Serial.println(inputChar);  // Debug output

  if (inputChar == 'x') {
    ledcWrite(p1, 0);
    ledcWrite(p2, 0);
    ledcWrite(p3, 0);
    ledcWrite(pwmw1, 0);
    ledcWrite(pwmw2, 0);
    ledcWrite(pwmtt, 0);
    ledcWrite(pwmg, 0);
    Serial.println("Stopping motion");
  } else if (inputChar >= '1' && inputChar <= '3') {
    int index = inputChar - '1';

    b1 = predefinedAngles[index][0];
    b2 = predefinedAngles[index][1];
    b3 = predefinedAngles[index][2];
    b4 = predefinedAngles[index][3]*ppr/360;

    moveJoints(b1, b2, b3, b4);

    Serial.print("Moved to predefined angle set ");
    Serial.println(inputChar);
  }
  else if (inputChar != '1' && inputChar != '2' && inputChar != '3'){
    switch (inputChar) {
      case 'r':
        Serial.println("r");
        digitalWrite(dirtt, HIGH);
        ledcWrite(pwmtt, pwm_tt);
        break;

      case 'f':
        Serial.println("f");
        digitalWrite(dirtt, LOW);
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
        lmt = analogRead(limit);
        if(lmt > 4000){
          //homing();
          inputChar = 'x';
        }else{
          Serial.println("w");
          digitalWrite(dirw1, HIGH);
          digitalWrite(dirw2, LOW);
          ledcWrite(pwmw1, pwm_ef);
          ledcWrite(pwmw2, pwm_ef);
        }
        break;

      case 'a':
        lmt = analogRead(limit);
        if(lmt > 4000){
          //homing();
          inputChar = 'x';
        }else{
          Serial.println("a");
          digitalWrite(dirw1, LOW);
          digitalWrite(dirw2, LOW);
          ledcWrite(pwmw1, pwm_ef);
          ledcWrite(pwmw2, pwm_ef);
        }
        break;

      case 's':
        lmt = analogRead(limit);
        if(lmt > 4000){
          //homing();
          inputChar = 'x';
        }else{
          Serial.println("s");
          digitalWrite(dirw1, LOW);
          digitalWrite(dirw2, HIGH);
          ledcWrite(pwmw1, pwm_ef);
          ledcWrite(pwmw2, pwm_ef);
        }
        break;

      case 'd':
        lmt = analogRead(limit);
        if(lmt > 4000){
          //homing();
          inputChar = 'x';
        }else{
          Serial.println("d");
          digitalWrite(dirw1, HIGH);
          digitalWrite(dirw2, HIGH);
          ledcWrite(pwmw1, pwm_ef);
          ledcWrite(pwmw2, pwm_ef);
        }
        break;

      case 'e':
        switch (gripState) {
          case 0: // Open gripper (HIGH direction)
            digitalWrite(dirg, HIGH);
            ledcWrite(pwmg, pwm_gripper);
            Serial.println("Gripper opening");
            break;

          case 1: // Stop motor
            ledcWrite(pwmg, 0);
            Serial.println("Motor stopped");
            break;

          case 2: // Close gripper (LOW direction)
            digitalWrite(dirg, LOW);
            ledcWrite(pwmg, pwm_gripper);
            Serial.println("Gripper closing");
            break;

          case 3: // Stop motor
            ledcWrite(pwmg, 0);
            Serial.println("Motor stopped");
            break;
        }
        break;

      case 'x':
        Serial.println("x");
        ledcWrite(p1, 0);
        ledcWrite(p2, 0);  
        ledcWrite(p3, 0);
        ledcWrite(pwmtt, 0);
        ledcWrite(pwmw1, 0); 
        ledcWrite(pwmw2, 0);  
        ledcWrite(pwmg, 0);
        break;
    }
    lmt = analogRead(limit);
    if(lmt > 4000) {
      //homing();
      inputChar = 'x';
    }
  }

    
  
}

// void homing() {
//   lmt = analogRead(limit);
//   while(lmt < 4000){
//     ledcWrite(pwmw1, pwm_homing);
//     digitalWrite(dirw1, HIGH);
//     ledcWrite(pwmw2, pwm_homing);
//     digitalWrite(dirw2, LOW);
//     lmt = analogRead(limit);
//   }
//   posi = 0;
//   ledcWrite(pwmw1, pwm_homing);
//   digitalWrite(dirw1, LOW);
//   ledcWrite(pwmw2, pwm_homing);
//   digitalWrite(dirw2, HIGH);
//   delay(200);
//   if (posi > 0){
//     while(posi <= zero_pulse){
//       ledcWrite(pwmw1, pwm_homing);
//       digitalWrite(dirw1, LOW);
//       ledcWrite(pwmw2, pwm_homing);
//       digitalWrite(dirw2, HIGH);
//     }
//   }else{
//     while(posi >= zero_pulse){
//       ledcWrite(pwmw1, pwm_homing);
//       digitalWrite(dirw1, LOW);
//       ledcWrite(pwmw2, pwm_homing);
//       digitalWrite(dirw2, HIGH);
//     }
//   }
//   ledcWrite(pwmw1,0);
//   ledcWrite(pwmw2,0);
//   Serial.println("limit switch hit");
//   Serial.println("Reaching zero degree");
//   posi = 0;
//   Serial.println("homing complete");
// }