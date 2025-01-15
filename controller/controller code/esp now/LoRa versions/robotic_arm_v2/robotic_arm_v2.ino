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

uint8_t mode_select = 0;
uint8_t slide2 = 0;

uint8_t pot1_val = 0;
uint8_t pot2_val = 0;
uint8_t pot3_val = 0;

uint8_t payload[11];

// t y u
//  g h j
// m1, m2, m3

int dir1 = 14;
int dir2 =  22;
int dir3 = 16;

int p1 = 27;
int p2 = 23;
int p3 = 17;

int pot1 = 34;
int pot2 = 35;
int pot3 = 4;

#define encA 36  
#define encB 39  
#define pwmw1 32   
#define dirw1 33  
#define pwmw2 25
#define dirw2 26

float targetAngle;
volatile long posi = 0;
float err = 0;
int pwm_ef = 120;
int pwm_homing = 80;
int ppr = 20000;
float b4;
// homing limit 
int zero_pulse = 250;
#define limit 12
int lmt;
const int freq = 20000;
const int resolution = 8;
const int pwm = 200;
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
  pinMode(encA,INPUT);
  pinMode(encB,INPUT);
  pinMode(pwmw1,OUTPUT);
  pinMode(dirw1,OUTPUT);
  pinMode(pwmw2,OUTPUT);
  pinMode(dirw2,OUTPUT);
  ledcAttachChannel(pwmw1, freq, resolution, 4);
  ledcAttachChannel(pwmw2, freq, resolution, 4);
  attachInterrupt(digitalPinToInterrupt(encA), readEncoder, RISING);
  pinMode(pot1, INPUT);
  pinMode(pot2, INPUT);
  pinMode(pot3, INPUT);

  assignFromPayload(payload);
  resetPayload(payload);

  // Register callback to receive data
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
  homing();

}

//pot 1 - 216 - 290
//pot 2 - 220 - 300
//pot 3 - 115 - 240
void readEncoder() {
  int stateB = digitalRead(encB);
  if (stateB > 0){
    posi = posi + 1;
  }else{
    posi = posi - 1;
  }
}

void loop() {

  char incoming;
  
  assignFromPayload(payload);

  // Print received payload data
  Serial.println("payload:");
  Serial.print(joystick1_x); Serial.print(", "); Serial.print(joystick1_y);  Serial.print(", "); Serial.println(Button1);
  Serial.print(joystick2_x); Serial.print(", "); Serial.print(joystick2_y);  Serial.print(", "); Serial.println(Button2);
  Serial.print(joystick3_x); Serial.print(", "); Serial.print(joystick3_y);  Serial.print(", "); Serial.println(Button3);
  Serial.print(mode_select); Serial.print(", "); Serial.println(slide2);

  Serial.println(pot1_val);
  Serial.println(pot2_val);
  Serial.println(pot3_val);

  Serial.println("  ");
  Serial.println("  ");
  Serial.println("  ");

  //reading potentiometer values;
  pot1_val = analogRead(pot1) >> 4;
  pot2_val = analogRead(pot2) >> 4;
  pot3_val = analogRead(pot3) >> 4;

  char horizontal1Command = getJoystickCommand(joystick1_x, 'x', 't', 'g');
  char horizontal2Command = getJoystickCommand(joystick2_x, 'x', 'y', 'h');
  char horizontal3Command = getJoystickCommand(joystick3_x, 'x', 'u', 'j');

  char vertical1Command = getJoystickCommand(joystick1_y, 'x', 'd', 'a');
  char vertical2Command = getJoystickCommand(joystick2_y, 'x', 'w', 's');
  char vertical3Command = getJoystickCommand(joystick3_y, 'x', 'f', 'r');

  if (mode_select == 170) {
    b1 = predefinedAngles[0][0];
    b2 = predefinedAngles[0][1];
    b3 = predefinedAngles[0][2];
    b4 = predefinedAngles[0][3]*ppr/360;
    moveJoints(b1,b2,b3,b4);
  }

  else if (mode_select == 119) {
    b1 = predefinedAngles[1][0];
    b2 = predefinedAngles[1][1];
    b3 = predefinedAngles[1][2];
    b4 = predefinedAngles[1][3]*ppr/360;
    moveJoints(b1,b2,b3,b4);
  }

  else if (mode_select == 76) {
    b1 = predefinedAngles[2][0];
    b2 = predefinedAngles[2][1];
    b3 = predefinedAngles[2][2];
    b4 = predefinedAngles[2][3]*ppr/360;
    moveJoints(b1,b2,b3,b4);
  }

  switch (horizontal1Command) {
    case 't':
      Serial.println("la1 forward");
      Serial.println("t");
      digitalWrite(dir1, HIGH);
      analogWrite(p1, 200);
      break;

    case 'g':
      Serial.println("la1 backward");
      Serial.println("g");
      digitalWrite(dir1, LOW);
      analogWrite(p1, 200);
      break;

    default:
      Serial.println("x");
      analogWrite(p1, 0);
      break;
  }

  switch (horizontal2Command) {
    case 'y':
      Serial.println("la2 forward");
      Serial.println("y");
      digitalWrite(dir2, HIGH);
      analogWrite(p2, 200);
      break;

    case 'h':
      Serial.println("la2 backward");
      Serial.println("h");
      digitalWrite(dir2, LOW);
      analogWrite(p2, 200);
      break;

    default:
      Serial.println("x");
      analogWrite(p2, 0);
      break;
  }

  switch (vertical2Command) {
    case 'w':
      Serial.println("la3 forward");
      Serial.println("u");
      digitalWrite(dir3, HIGH);
      analogWrite(p3, 200);
      break;

    case 's':
      Serial.println("la3 backward");
      Serial.println("j");
      digitalWrite(dir3, LOW);
      analogWrite(p3, 200);
      break;

    default:
      Serial.println("x");
      analogWrite(p3, 0);
      break;
  }
}

void assignFromPayload(uint8_t payload[11]) {
  slide2 = payload[0];
  mode_select = payload[1];

  Button3 = payload[2];
  joystick3_y = payload[3];
  joystick3_x = payload[4];

  Button2 = payload[5];
  joystick2_y = payload[6];
  joystick2_x = payload[7];

  Button1 = payload[8];
  joystick1_y = payload[9];
  joystick1_x = payload[10];
}

void resetPayload(uint8_t payload[11]) {
  payload[0] = 0;
  payload[1] = 0;
  payload[2] = 0;
  payload[3] = 128;
  payload[4] = 128;
  payload[5] = 0;
  payload[6] = 128;
  payload[7] = 128;
  payload[8] = 0;
  payload[9] = 128;
  payload[10] = 128;
}

void homing() {
  lmt = analogRead(limit);
  while(lmt < 4000){
    ledcWrite(pwmw1, pwm_homing);
    digitalWrite(dirw1, HIGH);
    ledcWrite(pwmw2, pwm_homing);
    digitalWrite(dirw2, LOW);
    lmt = analogRead(limit);
  }
  ledcWrite(pwmw1,0);
  ledcWrite(pwmw2,0);
  Serial.println("limit switch hit");
  Serial.println("Reaching zero degree");
  posi = 0;
  while(posi <= zero_pulse){
    ledcWrite(pwmw1, pwm_homing);
    digitalWrite(dirw1, LOW);
    ledcWrite(pwmw2, pwm_homing);
    digitalWrite(dirw2, HIGH);
  }
  posi = 0;
  ledcWrite(pwmw1,0);
  ledcWrite(pwmw2,0);
  Serial.println("homing complete");
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
      lmt = analogRead(limit);
      if(lmt > 4000){
        homing();
        lmt=0;
        break;
      }
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
      lmt = analogRead(limit);
      if(lmt > 4000){
        homing();
        lmt = 0;
        break;
      }
    }
    ledcWrite(pwmw1, 0);
    ledcWrite(pwmw2, 0);
    Serial.println("error corrected");
  }

}

char getJoystickCommand(uint8_t a, char axis, char positive, char negative) {
  if (a >= 0 && a < 5) {
    return negative; // Left or down movement
  } else if (a > 250 && a <= 255) {
    return positive; // Right or up movement
  }
  return axis; // Neutral position
}

void roboticArmZero() {
  Serial.println("zero position");
  if (pot1_val > 185) {
    digitalWrite(dir1, LOW);
    analogWrite(p1, 200);
  }
  if (pot2_val < 245) {
    digitalWrite(dir2, LOW);
    analogWrite(p2, 200);
  }
  if (pot3_val > 115) {
    digitalWrite(dir3, LOW);
    analogWrite(p3, 200);
  }
}

