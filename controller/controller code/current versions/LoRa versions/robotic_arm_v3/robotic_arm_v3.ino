#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>

uint8_t joystick1_x = 0;
uint8_t joystick1_y = 0;
uint8_t Button1 = 0;

uint8_t joystick2_x = 0;
uint8_t joystick2_y = 0;
uint8_t Button2 = 0;

uint8_t joystick3_x = 0;
uint8_t joystick3_y = 0;
uint8_t Button3 = 0;

uint8_t robotic_arm_position = 0;
uint8_t mode_select = 0;
uint8_t mode_select_2 = 0;

uint8_t pot1_val = 0;
uint8_t pot2_val = 0;
uint8_t pot3_val = 0;

uint8_t payload[12];

// controls
int pot1 = 34;
int pot2 = 35;
int pot3 = 4;
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

float targetAngle;
volatile long posi = 0;
float err = 0;
const int freq = 20000;
const int resolution = 8;
int pwm_ef = 150;
int pwm_homing = 80;
const int pwm = 200;
int ppr = 20000;

float b1; 
float b2; 
float b3;
float b4;

int lmt;

float w2 = 0;; 
int a2 = 0;

float w3 = 0;
int a3 = 0;

float w1 = 0;
int a1 = 0;

int q1,q2,q3;
int c1,c2,c3;
int b11,b22,b33;

int angle1;
int angle2;
int angle3;

int values[4];
float predefinedAngles[][4] = {
  {216, 290, 155, 40}, // Predefined angle set 1
  {230, 250, 180, 60}, // Predefined angle set 2
  {260, 300, 240, 30}  // Predefined angle set 3
};

uint8_t newMACAddress[] = {0xEC, 0x64, 0xC9, 0x5E, 0x11, 0x3C}; //setting a new mac address

// Callback function to handle received data
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&payload, incomingData, sizeof(payload));
  Serial.print("Bytes received: ");
  Serial.println(len);
}

char getJoystickCommand(uint8_t a, char axis, char positive, char negative) {
  if (a >= 0 && a < 5) {
    return negative; // Left or down movement
  } else if (a > 250 && a <= 255) {
    return positive; // Right or up movement
  }
  return axis; // Neutral position
}

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

void setup(){
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);

  //Change ESP32 Mac Address
  esp_err_t err = esp_wifi_set_mac(WIFI_IF_STA, &newMACAddress[0]);
  if (err == ESP_OK) {
    Serial.println("Success changing Mac Address");
  }

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

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

  assignFromPayload(payload);
  resetPayload(payload);
  
  //homing();
  // Register callback to receive data
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
}

void readEncoder() {
  int stateB = digitalRead(encB);
  if (stateB > 0){
    posi = posi + 1;
  }else{
    posi = posi - 1;
  }
}

int robotic_arm_toggle = 0;

void loop() {
  
  assignFromPayload(payload);

  printValues();

  if (mode_select == 1) {
    robotic_arm_toggle = !robotic_arm_toggle;
  }

  Serial.println("toggle: " + String(robotic_arm_toggle));

  if (robotic_arm_toggle) {
    //reading potentiometer values;

    char horizontal1Command = getJoystickCommand(joystick1_x, 'x', 't', 'g');
    char horizontal2Command = getJoystickCommand(joystick2_x, 'x', 'y', 'h');
    char horizontal3Command = getJoystickCommand(joystick3_x, 'x', 'd', 'a');

    char vertical1Command = getJoystickCommand(joystick1_y, 'x', 'f', 'r');
    char vertical2Command = getJoystickCommand(joystick2_y, 'x', 'u', 'j');
    char vertical3Command = getJoystickCommand(joystick3_y, 'x', 'w', 's');

    if (robotic_arm_position >= 160 && robotic_arm_position <= 180) {
      b1 = predefinedAngles[0][0];
      b2 = predefinedAngles[0][1];
      b3 = predefinedAngles[0][2];
      b4 = predefinedAngles[0][3]*ppr/360;
      moveJoints(b1,b2,b3,b4);
    }

    else if (robotic_arm_position >= 110 && robotic_arm_position <= 130) {
      b1 = predefinedAngles[1][0];
      b2 = predefinedAngles[1][1];
      b3 = predefinedAngles[1][2];
      b4 = predefinedAngles[1][3]*ppr/360;
      moveJoints(b1,b2,b3,b4);
    }

    else if (robotic_arm_position >= 60 && robotic_arm_position <= 80) {
      b1 = predefinedAngles[2][0];
      b2 = predefinedAngles[2][1];
      b3 = predefinedAngles[2][2];
      b4 = predefinedAngles[2][3]*ppr/360;
      moveJoints(b1,b2,b3,b4);
    }

    switch (vertical1Command) {
      case 'f':
        Serial.println("tt moves");
        Serial.println("f");
        digitalWrite(dirtt, HIGH);
        ledcWrite(pwmtt, 60);
        break;

      case 'r':
        Serial.println("tt moves other way");
        Serial.println("h");
        digitalWrite(dirtt, LOW);
        ledcWrite(pwmtt, 60);
        break;

      default:
        Serial.println("x");
        ledcWrite(pwmtt, 0);
        break;
    }

    switch (horizontal1Command) {
    
      case 't':
        Serial.println("la1 forward");
        Serial.println("t");
        digitalWrite(dir1, HIGH);
        ledcWrite(p1, 200);
        break;

      case 'g':
        Serial.println("la1 backward");
        Serial.println("g");
        digitalWrite(dir1, LOW);
        ledcWrite(p1, 200);
        break;

      default:
        Serial.println("x");
        ledcWrite(p1, 0);
        break;
    }

    switch (horizontal2Command) {
      case 'y':
        Serial.println("la2 forward");
        Serial.println("y");
        digitalWrite(dir2, HIGH);
        ledcWrite(p2, 200);
        break;

      case 'h':
        Serial.println("la2 backward");
        Serial.println("h");
        digitalWrite(dir2, LOW);
        ledcWrite(p2, 200);
        break;

      default:
        Serial.println("x");
        ledcWrite(p2, 0);
        break;
    }

    switch (vertical2Command) {
      case 'u':
        Serial.println("la3 forward");
        Serial.println("u");
        digitalWrite(dir3, HIGH);
        ledcWrite(p3, 200);
        break;

      case 'j':
        Serial.println("la3 backward");
        Serial.println("j");
        digitalWrite(dir3, LOW);
        ledcWrite(p3, 200);
        break;

      default:
        Serial.println("x");
        ledcWrite(p3, 0);
        break;
    }

    switch (vertical3Command) {
      case 'w':
        // lmt = analogRead(limit);
        // if(lmt > 4000){
        //   //homing();
        //   vertical3Command = 'x';
        // }else{
          Serial.println("w");
          digitalWrite(dirw1, HIGH);
          digitalWrite(dirw2, LOW);
          ledcWrite(pwmw1, 150);
          ledcWrite(pwmw2, 150);
        //}
        break;

      case 's':
        // lmt = analogRead(limit);
        // if(lmt > 4000){
        //   //homing();
        //   vertical3Command = 'x';
        // }else{
          Serial.println("s");
          digitalWrite(dirw1, LOW);
          digitalWrite(dirw2, HIGH);
          ledcWrite(pwmw1, 150);
          ledcWrite(pwmw2, 150);
        //}
        break;

      default:
        Serial.println("x");
        ledcWrite(pwmw1, 0);
        ledcWrite(pwmw2, 0);
        break;
    }

    switch (horizontal3Command) {
      case 'a':
        //lmt = analogRead(limit);
        // if(lmt > 4000){
        //   //homing();
        //   horizontal3Command = 'x';
        // }else{
          Serial.println("a");
          digitalWrite(dirw1, LOW);
          digitalWrite(dirw2, LOW);
          ledcWrite(pwmw1, 150);
          ledcWrite(pwmw2, 150);
        //}
        break;

      case 'd':
        //lmt = analogRead(limit);
        // if(lmt > 4000){
        //   //homing();
        //   horizontal3Command = 'x';
        // }else{
          Serial.println("d");
          digitalWrite(dirw1, HIGH);
          digitalWrite(dirw2, HIGH);
          ledcWrite(pwmw1, 150);
          ledcWrite(pwmw2, 150);
        //}
        break;

      default:
        Serial.println("x");
        ledcWrite(pwmw1, 0);
        ledcWrite(pwmw2, 0);
        break;
    }

    // lmt = analogRead(limit);
    // if(lmt > 4000) {
    //   //homing();
    //   vertical3Command = 'x';
    //   horizontal3Command = 'x';
    // }
  }
}

void assignFromPayload(uint8_t payload[12]) {
  mode_select_2 = payload[0];
  mode_select = payload[1];
  robotic_arm_position = payload[2];

  Button3 = payload[3];
  joystick3_y = payload[4];
  joystick3_x = payload[5];

  Button2 = payload[6];
  joystick2_y = payload[7];
  joystick2_x = payload[8];

  Button1 = payload[9];
  joystick1_y = payload[10];
  joystick1_x = payload[11];
}

void resetPayload(uint8_t payload[12]) {
  payload[0] = 0;
  payload[1] = 0;
  payload[2] = 0;
  payload[3] = 0;
  payload[4] = 128;
  payload[5] = 128;
  payload[6] = 0;
  payload[7] = 128;
  payload[8] = 128;
  payload[9] = 0;
  payload[10] = 128;
  payload[11] = 128;
}

void printValues() {
  Serial.print(joystick1_x); Serial.print(","); Serial.print(joystick1_y); Serial.print(","); Serial.print(Button1); Serial.println("  ");
  Serial.print(joystick2_x); Serial.print(","); Serial.print(joystick2_y); Serial.print(","); Serial.print(Button2); Serial.println("  ");
  Serial.print(joystick3_x); Serial.print(","); Serial.print(joystick3_y); Serial.print(","); Serial.print(Button3); Serial.println("  ");
  Serial.print(robotic_arm_position); Serial.print(","); Serial.print(mode_select); Serial.print(","); Serial.print(mode_select_2); Serial.println("  ");

  Serial.println("  ");
  Serial.println("  ");
  Serial.println("  ");
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
// }