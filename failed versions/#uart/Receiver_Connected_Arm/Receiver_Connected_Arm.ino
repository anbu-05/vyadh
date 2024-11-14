#include <HardwareSerial.h>

#define TXD1 18
#define RXD1 19

// Use Serial1 for UART communication
HardwareSerial uart2(1);

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
int freq = 10000;
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

// Variable to hold received data and initialise it to zero
controllerData payload;


void setup() {

  Serial.begin(115200);

  uart2.begin(115200, SERIAL_8N1, RXD1, TXD1);

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
}

void loop() {

  if (uart2.available()) {

    Serial.println(uart2.readBytes((uint16_t*)&payload, sizeof(payload)));
    // Print received payload data
    Serial.print("Joystick1 X: "); Serial.print(payload.joystick1_x);
    Serial.print(", Joystick1 Y: "); Serial.print(payload.joystick1_y);
    Serial.print(", button1: "); Serial.println(payload.Button1);

    Serial.print("Joystick2 X: "); Serial.print(payload.joystick2_x);
    Serial.print(", Joystick2 Y: "); Serial.print(payload.joystick2_y);
    Serial.print(", button2: "); Serial.println(payload.Button2);

    Serial.print("Joystick3 X: "); Serial.print(payload.joystick3_x);
    Serial.print(", Joystick3 Y: "); Serial.print(payload.joystick3_y);
    Serial.print(", button3: "); Serial.println(payload.Button3);

    Serial.print("slide switch 1: "); Serial.println(payload.slide1);
    Serial.print("slide switch 2: "); Serial.println(payload.slide2);

    Serial.println("  ");
    Serial.println("  ");
    Serial.println("  ");

    if(payload.joystick1_x >= 0 && payload.joystick1_x < 1800) cmd='g';
    else if(payload.joystick1_x > 2200 && payload.joystick1_x <= 4096) cmd='t';
    else cmd='x';

    if(payload.joystick2_x >= 0 && payload.joystick2_x < 1800) cmd='h';
    else if(payload.joystick2_x > 2200 && payload.joystick2_x <= 4096) cmd='y';
    else cmd='x';

    if(payload.joystick3_x >= 0 && payload.joystick3_x < 1800) cmd='j';
    else if(payload.joystick3_x > 2200 && payload.joystick3_x <= 4096) cmd='u';
    else cmd='x';

    if(payload.joystick1_y >= 0 && payload.joystick1_y < 1800) cmd='a';
    else if(payload.joystick1_y > 2200 && payload.joystick1_y <= 4096) cmd='d';
    else cmd='x';

    if(payload.joystick2_y >= 0 && payload.joystick2_y < 1800) cmd='s';
    else if(payload.joystick2_y > 2200 && payload.joystick2_y <= 4096) cmd='w';
    else cmd='x';

    if(payload.joystick3_y >= 0 && payload.joystick3_y < 1800) cmd='r';
    else if(payload.joystick3_y > 2200 && payload.joystick3_y <= 4096) cmd='f';
    else cmd='x';
    
    switch(cmd){
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
  }
}
