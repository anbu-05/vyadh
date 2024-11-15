#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>

// Define the structure
struct controllerData{
  int joystick1_x;
  int joystick1_y;
//  int Button1;

  int joystick2_x;
  int joystick2_y;
//  int Button2;

  int joystick3_x;
  int joystick3_y;
//  int Button3;

  int slide1;
  int slide2;
};

// Variable to hold received data
controllerData Payload;

// Callback function to handle received data
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&Payload, incomingData, sizeof(Payload));
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
   // Print received payload data
    Serial.print("Joystick1 X: "); Serial.print(Payload.joystick1_x);
    Serial.print(", Joystick1 Y: "); Serial.print(Payload.joystick1_y);
    //Serial.print(", button1: "); Serial.println(Payload.Button1);
  
    Serial.print("Joystick2 X: "); Serial.print(Payload.joystick2_x);
    Serial.print(", Joystick2 Y: "); Serial.print(Payload.joystick2_y);
    //Serial.print(", button2: "); Serial.println(Payload.Button2);

    Serial.print("Joystick3 X: "); Serial.print(Payload.joystick3_x);
    Serial.print(", Joystick3 Y: "); Serial.print(Payload.joystick3_y);
    //Serial.print(", button3: "); Serial.println(Payload.Button3);

    Serial.print("slide switch 1: "); Serial.println(Payload.slide1);
    Serial.print("slide switch 2: "); Serial.println(Payload.slide2);

    Serial.println("  ");
    Serial.println("  ");
    Serial.println("  ");

    if (Payload.slide2) {
      // encoding
      if ((Payload.joystick1_x >= 0) && (Payload.joystick1_x < 400)) cmd_la1 = 'g';
      else if ((Payload.joystick1_x > 3600) && (Payload.joystick1_x <= 4095)) cmd_la1 = 't';
      else cmd_la1 = 'x';

      if ((Payload.joystick2_x >= 0) && (Payload.joystick2_x < 400)) cmd_la2 = 'h';
      else if ((Payload.joystick2_x > 3600) && (Payload.joystick2_x <= 4095)) cmd_la2 = 'y';
      else cmd_la2 = 'x';

      if ((Payload.joystick3_x >= 0) && (Payload.joystick3_x < 400)) cmd_la3 = 'u';
      else if ((Payload.joystick3_x > 3600) && (Payload.joystick3_x <= 4095)) cmd_la3 = 'j';
      else cmd_la3 = 'x';

      if ((Payload.joystick1_y >= 0) && (Payload.joystick1_y < 400)) cmd_wrist = 'a';
      else if ((Payload.joystick1_y > 3600) && (Payload.joystick1_y <= 4095)) cmd_wrist = 'd';
      else if ((Payload.joystick2_y >= 0) && (Payload.joystick2_y < 400)) cmd_wrist = 's';
      else if ((Payload.joystick2_y > 3600) && (Payload.joystick2_y <= 4095)) cmd_wrist = 'w';
      else cmd_wrist = 'x';

      if ((Payload.joystick3_y >= 0) && (Payload.joystick3_y < 400)) cmd_gripper = 'r';
      else if ((Payload.joystick3_y > 3600) && (Payload.joystick3_y <= 4095)) cmd_gripper = 'f';
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