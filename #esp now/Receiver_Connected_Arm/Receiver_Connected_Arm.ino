#include <esp_now.h>
#include <WiFi.h>

#define CHANNEL 1

int dir_la1=13; //la 1
int dir_la2=25; //la 2
int dir_la3=19; //la 3
int pwm_la1=12; //la 1
int pwm_la2=26; //la 2
int pwm_la3=18; //la 3
//wrist 
int pwm_w1=27;
int dir_w1=14;
int pwm_w2=15;
int dir_w2=2;
//gripper
int pwm_g=32;
int dir_g=33;
int freq = 4000;
int res = 8;
int pwm=160;
int pwm_ef=40;
char cmd;


// Define the structure
struct controllerData {
  int joystick1_x;
  int joystick1_y;
  int Button1;

  int joystick2_x;
  int joystick2_y;
  int Button2;

  int joystick3_x;
  int joystick3_y;
  int Button3;

  int slide1;
  int slide2;
};

// Variable to hold received data
controllerData receivedPayload;


void setup() {

  Serial.begin(115200);

  WiFi.mode(WIFI_AP);
  WiFi.softAP("RX_1", "RX_1_Password", CHANNEL, 0);
  
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
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
   // Print received payload data
    Serial.print("Joystick1 X: "); Serial.print(receivedPayload.joystick1_x);
    Serial.print(", Joystick1 Y: "); Serial.print(receivedPayload.joystick1_y);
    Serial.print(", button1: "); Serial.println(receivedPayload.Button1);
  
    Serial.print("Joystick2 X: "); Serial.print(receivedPayload.joystick2_x);
    Serial.print(", Joystick2 Y: "); Serial.print(receivedPayload.joystick2_y);
    Serial.print(", button2: "); Serial.println(receivedPayload.Button2);

    Serial.print("Joystick3 X: "); Serial.print(receivedPayload.joystick3_x);
    Serial.print(", Joystick3 Y: "); Serial.print(receivedPayload.joystick3_y);
    Serial.print(", button3: "); Serial.println(receivedPayload.Button3);

    Serial.print("slide switch 1: "); Serial.println(receivedPayload.slide1);
    Serial.print("slide switch 2: "); Serial.println(receivedPayload.slide2);

    Serial.println("  ");
    Serial.println("  ");
    Serial.println("  ");

  if ((receivedPayload.joystick1_x >= 0) && (receivedPayload.joystick1_x < 400)) cmd = 'g';
  else if ((receivedPayload.joystick1_x > 2600) && (receivedPayload.joystick1_x <= 4095)) cmd = 't';
  else if ((receivedPayload.joystick2_x >= 0) && (receivedPayload.joystick2_x < 400)) cmd = 'h';
  else if ((receivedPayload.joystick2_x > 2600) && (receivedPayload.joystick2_x <= 4095)) cmd = 'y';
  else if ((receivedPayload.joystick3_x >= 0) && (receivedPayload.joystick3_x < 400)) cmd = 'j';
  else if ((receivedPayload.joystick3_x > 2600) && (receivedPayload.joystick3_x <= 4095)) cmd = 'u';
  else if ((receivedPayload.joystick1_y >= 0) && (receivedPayload.joystick1_y < 400)) cmd = 'a';
  else if ((receivedPayload.joystick1_y > 2600) && (receivedPayload.joystick1_y <= 4095)) cmd = 'd';
  else if ((receivedPayload.joystick2_y >= 0) && (receivedPayload.joystick2_y < 400)) cmd = 's';
  else if ((receivedPayload.joystick2_y > 2600) && (receivedPayload.joystick2_y <= 4095)) cmd = 'w';
  else if ((receivedPayload.joystick3_y >= 0) && (receivedPayload.joystick3_y < 400)) cmd = 'r';
  else if ((receivedPayload.joystick3_y > 2600) && (receivedPayload.joystick3_y <= 4095)) cmd = 'f';
  else cmd = 'x';


/*
  if (receivedPayload.joystick1_x < 400) cmd = 'g';
  else if (receivedPayload.joystick1_x > 2600) cmd = 't';
  else if (receivedPayload.joystick2_x < 400) cmd = 'h';
  else if (receivedPayload.joystick2_x > 2600) cmd = 'y';
  else if (receivedPayload.joystick3_x < 400) cmd = 'j';
  else if (receivedPayload.joystick3_x > 2600) cmd = 'u';
  else if (receivedPayload.joystick1_y < 400) cmd = 'a';
  else if (receivedPayload.joystick1_y > 2600) cmd = 'd';
  else if (receivedPayload.joystick2_y < 400) cmd = 's';
  else if (receivedPayload.joystick2_y > 2600) cmd = 'w';
  else if (receivedPayload.joystick3_y < 400) cmd = 'r';
  else if (receivedPayload.joystick3_y > 2600) cmd = 'f';
  else cmd = 'x';

  */
  
  switch(cmd) {
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
      Serial.println("x");
      ledcWrite(pwm_la1, 0);
      ledcWrite(pwm_la2, 0);  
      ledcWrite(pwm_la3, 0);
      ledcWrite(pwm_w1, 0); 
      ledcWrite(pwm_w2, 0);  
      ledcWrite(pwm_g,0);
      break;
  }
  delay(20);
}



// Callback function to handle received data
void OnDataRecv(const esp_now_recv_info *recv_info, const uint8_t *data, int data_len) {
  memcpy(&receivedPayload, data, sizeof(receivedPayload));
  Serial.println("Data received:");
  Serial.print("MAC Address: ");
  for (int i = 0; i < 6; ++i) {
    Serial.print(recv_info->src_addr[i], HEX);
    if (i < 5) Serial.print(":");
  }
  Serial.println();
}
