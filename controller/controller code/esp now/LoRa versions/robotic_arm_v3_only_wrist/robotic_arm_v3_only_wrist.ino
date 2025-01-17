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

  // pinMode(dir1,OUTPUT);
  // pinMode(p1,OUTPUT);
  // ledcAttachChannel(p1, freq, resolution, 1);

  // pinMode(dir2,OUTPUT);
  // pinMode(p2,OUTPUT);
  // ledcAttachChannel(p2, freq, resolution, 2);

  // pinMode(dir3,OUTPUT);
  // pinMode(p3,OUTPUT);
  // ledcAttachChannel(p3, freq, resolution, 3);
  
  pinMode(pwmw1,OUTPUT);
  pinMode(dirw1,OUTPUT);
  ledcAttachChannel(pwmw1, freq, resolution, 4);

  pinMode(pwmw2,OUTPUT);
  pinMode(dirw2,OUTPUT);
  ledcAttachChannel(pwmw2, freq, resolution, 5);

  pinMode(encA,INPUT);
  pinMode(encB,INPUT);

  pinMode(pot1, INPUT);
  pinMode(pot2, INPUT);
  pinMode(pot3, INPUT);

  assignFromPayload(payload);
  resetPayload(payload);
  
  //homing();
  // Register callback to receive data
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
}

int robotic_arm_toggle = 0;

void loop() {
  
  assignFromPayload(payload);

  printValues();

  if (mode_select == 36) {
    robotic_arm_toggle = !robotic_arm_toggle;
  }

  Serial.println("toggle: " + String(robotic_arm_toggle));

  if (robotic_arm_toggle) {
    //reading potentiometer values;

    char horizontal1Command = getJoystickCommand(joystick1_x, 'x', 't', 'g');
    char horizontal2Command = getJoystickCommand(joystick2_x, 'x', 'd', 'a');
    char horizontal3Command = getJoystickCommand(joystick3_x, 'x', 'y', 'h');

    char vertical1Command = getJoystickCommand(joystick1_y, 'x', 'f', 'r');
    char vertical2Command = getJoystickCommand(joystick2_y, 'x', 'u', 'j');
    char vertical3Command = getJoystickCommand(joystick3_y, 'x', 'w', 's');


    switch (vertical3Command) {
      case 'w':
          Serial.println("w");
          digitalWrite(dirw1, HIGH);
          digitalWrite(dirw2, LOW);
          analogWrite(pwmw1, 150);
          analogWrite(pwmw2, 150);
        break;

      case 's':
          Serial.println("s");
          digitalWrite(dirw1, LOW);
          digitalWrite(dirw2, HIGH);
          analogWrite(pwmw1, 150);
          analogWrite(pwmw2, 150);
        break;

      default:
        Serial.println("x");
        analogWrite(pwmw1, 0);
        analogWrite(pwmw2, 0);
        break;
    }

    switch (horizontal2Command) {
      case 'a':
          Serial.println("a");
          digitalWrite(dirw1, LOW);
          digitalWrite(dirw2, LOW);
          analogWrite(pwmw1, 150);
          analogWrite(pwmw2, 150);
        break;

      case 'd':
          Serial.println("d");
          digitalWrite(dirw1, HIGH);
          digitalWrite(dirw2, HIGH);
          analogWrite(pwmw1, 150);
          analogWrite(pwmw2, 150);
        break;

      default:
        Serial.println("x");
        analogWrite(pwmw1, 0);
        analogWrite(pwmw2, 0);
        break;
    }
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

void printValues() {
  Serial.println("payload:");
  Serial.print(joystick1_x); Serial.print(", "); Serial.print(joystick1_y);  Serial.print(", "); Serial.println(Button1);
  Serial.print(joystick2_x); Serial.print(", "); Serial.print(joystick2_y);  Serial.print(", "); Serial.println(Button2);
  Serial.print(joystick3_x); Serial.print(", "); Serial.print(joystick3_y);  Serial.print(", "); Serial.println(Button3);
  Serial.print(robotic_arm_position); Serial.print(", "); Serial.println(mode_select);

  Serial.println("  ");
  Serial.println("  ");
  Serial.println("  ");
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