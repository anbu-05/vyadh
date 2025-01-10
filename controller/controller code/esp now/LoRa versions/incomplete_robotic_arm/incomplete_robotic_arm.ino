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

uint8_t slide1 = 0;
uint8_t slide2 = 0;

uint8_t payload[11];

// Callback function to handle received data
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&payload, incomingData, sizeof(payload));
  Serial.print("Bytes received: ");
  Serial.println(len);
}


int dir_la1=13; //la 1
int dir_la2=25; //la 2
int dir_la3=19; //la 3
int pwm_la1=12; //la 1
int pwm_la2=26; //la 2
int pwm_la3=18; //la 3
//wrist 
int pwm_w1=27;
int dir_w1=14;
int pwm_w2=17;
int dir_w2=2;
//gripper
int pwm_g=32;
int dir_g=33;
int freq = 4000;
int res = 8;
int pwm=160;
int pwm_ef=40;
char cmd_la1;
char cmd_la2;
char cmd_la3;
char cmd_wrist;
char cmd_gripper;

uint8_t newMACAddress[] = {0xEC, 0x64, 0xC9, 0x5E, 0x11, 0x3C}; //setting a new mac address

void setup() {

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

   //linear actuators 
  pinMode(dir_la1,OUTPUT);
  pinMode(pwm_la1,OUTPUT);
  pinMode(dir_la2,OUTPUT);
  pinMode(pwm_la2,OUTPUT);
  pinMode(dir_la3,OUTPUT);
  pinMode(pwm_la3,OUTPUT);
  //wrist motors
  pinMode(pwm_w1,OUTPUT);
  pinMode(dir_w1,OUTPUT);
  pinMode(pwm_w2,OUTPUT);
  pinMode(dir_w2,OUTPUT);
  //gripper
  pinMode(pwm_g,OUTPUT);
  pinMode(dir_g,OUTPUT);
  //pwm channels
  ledcAttach(pwm_la1,freq,res);
  ledcAttach(pwm_la2,freq,res);
  ledcAttach(pwm_la3,freq,res);
  ledcAttach(pwm_w1,freq,res);
  ledcAttach(pwm_w2,freq,res);
  ledcAttach(pwm_g,freq,res);

  // Register callback to receive data
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
}

void loop() {

  assignFrompayload(payload);
  // Print received payload data
  Serial.print("Joystick1 X: "); Serial.print(joystick1_x);
  Serial.print(", Joystick1 Y: "); Serial.print(joystick1_y);
  //Serial.print(", button1: "); Serial.println(Button1);

  Serial.print("Joystick2 X: "); Serial.print(joystick2_x);
  Serial.print(", Joystick2 Y: "); Serial.print(joystick2_y);
  //Serial.print(", button2: "); Serial.println(Button2);

  Serial.print("Joystick3 X: "); Serial.print(joystick3_x);
  Serial.print(", Joystick3 Y: "); Serial.print(joystick3_y);
  //Serial.print(", button3: "); Serial.println(Button3);

  Serial.print("slide switch 1: "); Serial.println(slide1);
  Serial.print("slide switch 2: "); Serial.println(slide2);

  Serial.println("  ");
  Serial.println("  ");
  Serial.println("  ");

  if (slide2) {
    // encoding
    if ((joystick1_x >= 0) && (joystick1_x < 400)) cmd_la1 = 'g';
    else if ((joystick1_x > 3600) && (joystick1_x <= 4095)) cmd_la1 = 't';
    else cmd_la1 = 'x';

    if ((joystick2_x >= 0) && (joystick2_x < 400)) cmd_la2 = 'h';
    else if ((joystick2_x > 3600) && (joystick2_x <= 4095)) cmd_la2 = 'y';
    else cmd_la2 = 'x';

    if ((joystick3_x >= 0) && (joystick3_x < 400)) cmd_la3 = 'u';
    else if ((joystick3_x > 3600) && (joystick3_x <= 4095)) cmd_la3 = 'j';
    else cmd_la3 = 'x';

    if ((joystick1_y >= 0) && (joystick1_y < 400)) cmd_wrist = 'a';
    else if ((joystick1_y > 3600) && (joystick1_y <= 4095)) cmd_wrist = 'd';
    else if ((joystick2_y >= 0) && (joystick2_y < 400)) cmd_wrist = 's';
    else if ((joystick2_y > 3600) && (joystick2_y <= 4095)) cmd_wrist = 'w';
    else cmd_wrist = 'x';

    if ((joystick3_y >= 0) && (joystick3_y < 400)) cmd_gripper = 'r';
    else if ((joystick3_y > 3600) && (joystick3_y <= 4095)) cmd_gripper = 'f';
    else cmd_gripper = 'x';


    // LA1
    switch(cmd_la1){
      case('t'):
        Serial.println("t");
        digitalWrite(dir_la1,HIGH);
        ledcWrite(pwm_la1,200);
        break;

      case('g'):
        Serial.println("g");
        digitalWrite(dir_la1,LOW);
        ledcWrite(pwm_la1,200);
        break;

      case('x'):
        Serial.println('LA1 off');
        ledcWrite(pwm_la1,0);
        break;
    }

    // LA2
    switch(cmd_la2){
      case('y'):
        Serial.println("y");
        digitalWrite(dir_la2,HIGH);
        ledcWrite(pwm_la2,pwm);
        break;

      case('h'):
        Serial.println("h");
        digitalWrite(dir_la2,LOW);
        ledcWrite(pwm_la2,pwm);
        break;
      
      case('x'):
        Serial.println('LA2 off');
        ledcWrite(pwm_la2,0);
        break;
    }

    // LA 3
    switch(cmd_la3){
      case('u'):
        Serial.println("u");
        digitalWrite(dir_la3,HIGH);
        ledcWrite(pwm_la3,pwm);
        break;

      case('j'):
        Serial.println("j");
        digitalWrite(dir_la3,LOW);
        ledcWrite(pwm_la3,pwm);
        break;

      case('x'):
        Serial.println('LA3 off');
        ledcWrite(pwm_la3,0);
        break;
    }

    // WRIST
    switch(cmd_wrist){
      case('w'):
        Serial.println("w");
        digitalWrite(dir_w1,HIGH);
        digitalWrite(dir_w2,HIGH);
        ledcWrite(pwm_w1, pwm_ef);
        ledcWrite(pwm_w2, pwm_ef);
        break;

      case('s'): 
        Serial.println("s");
        digitalWrite(dir_w1,LOW);
        digitalWrite(dir_w2,LOW);
        ledcWrite(pwm_w1, pwm_ef);
        ledcWrite(pwm_w2, pwm_ef);
        break;

      case('a'):
        Serial.println("a");
        digitalWrite(dir_w1,LOW);
        digitalWrite(dir_w2,HIGH);
        ledcWrite(pwm_w1, pwm_ef);
        ledcWrite(pwm_w2, pwm_ef);
        break;

      case('d'):
        Serial.println("d");
        digitalWrite(dir_w1, HIGH);
        digitalWrite(dir_w2,LOW);
        ledcWrite(pwm_w1, pwm_ef);
        ledcWrite(pwm_w2, pwm_ef);
        break;  
      
      case('x'):
        Serial.println("wrist off");
        ledcWrite(pwm_w1, 0);
        ledcWrite(pwm_w2, 0);
        break;
    }

    // GRIPPER
    switch(cmd_gripper){
      case('r'):
        Serial.println("r");
        digitalWrite(dir_g,HIGH);
        ledcWrite(pwm_g,pwm);
        break;
        
      case('f'):
        Serial.println("f");
        digitalWrite(dir_g,LOW);
        ledcWrite(pwm_g,pwm);
        break;
      
      case('x'):
        Serial.println('wrist off');
        ledcWrite(pwm_g,0);
        break;
    }
  }
  delay(20);
}

void assignFrompayload(uint8_t payload[11]) {
  slide2 = payload[0];
  slide1 = payload[1];

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