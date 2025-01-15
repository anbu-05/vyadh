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

uint8_t pot1_val = 0;
uint8_t pot2_val = 0;
uint8_t pot3_val = 0;

uint8_t payload[11];

// Callback function to handle received data
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&payload, incomingData, sizeof(payload));
  Serial.print("Bytes received: ");
  Serial.println(len);
}

// t y u
//  g h j
// m1, m2, m3

int dir_la1 = 14;
int dir_la2 =  22;
int dir_la3 = 16;

int pwm_la1 = 27;
int pwm_la2 = 23;
int pwm_la3 = 17;

int pot1 = 34;
int pot2 = 35;
int pot3 = 4;

uint8_t newMACAddress[] = {0xEC, 0x64, 0xC9, 0x5E, 0x11, 0x3C}; //setting a new mac address

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

  pinMode(dir_la1,OUTPUT);
  pinMode(pwm_la1,OUTPUT);
  pinMode(dir_la2,OUTPUT);
  pinMode(pwm_la2,OUTPUT);
  pinMode(dir_la3,OUTPUT);
  pinMode(pwm_la3,OUTPUT);

  pinMode(pot1, INPUT);
  pinMode(pot2, INPUT);
  pinMode(pot3, INPUT);

  assignFromPayload(payload);
  resetPayload(payload);

  // Register callback to receive data
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));

}

//pot 1 - 216 - 290
//pot 2 - 220 - 300
//pot 3 - 115 - 240


void loop() {

  char incoming;
  
  assignFromPayload(payload);

  // Print received payload data
  Serial.println("payload:");
  Serial.print(joystick1_x); Serial.print(", "); Serial.print(joystick1_y);  Serial.print(", "); Serial.println(Button1);
  Serial.print(joystick2_x); Serial.print(", "); Serial.print(joystick2_y);  Serial.print(", "); Serial.println(Button2);
  Serial.print(joystick3_x); Serial.print(", "); Serial.print(joystick3_y);  Serial.print(", "); Serial.println(Button3);
  Serial.print(slide1); Serial.print(", "); Serial.println(slide2);

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

  if (!Button2) {
    roboticArmZero();
  }

  switch (horizontal1Command) {
    case 't':
      Serial.println("la1 forward");
      Serial.println("t");
      digitalWrite(dir_la1, HIGH);
      analogWrite(pwm_la1, 200);
      break;

    case 'g':
      Serial.println("la1 backward");
      Serial.println("g");
      digitalWrite(dir_la1, LOW);
      analogWrite(pwm_la1, 200);
      break;

    default:
      Serial.println("x");
      analogWrite(pwm_la1, 0);
      break;
  }

  switch (horizontal2Command) {
    case 'y':
      Serial.println("la2 forward");
      Serial.println("y");
      digitalWrite(dir_la2, HIGH);
      analogWrite(pwm_la2, 200);
      break;

    case 'h':
      Serial.println("la2 backward");
      Serial.println("h");
      digitalWrite(dir_la2, LOW);
      analogWrite(pwm_la2, 200);
      break;

    default:
      Serial.println("x");
      analogWrite(pwm_la2, 0);
      break;
  }

  switch (vertical2Command) {
    case 'w':
      Serial.println("la3 forward");
      Serial.println("u");
      digitalWrite(dir_la3, HIGH);
      analogWrite(pwm_la3, 200);
      break;

    case 's':
      Serial.println("la3 backward");
      Serial.println("j");
      digitalWrite(dir_la3, LOW);
      analogWrite(pwm_la3, 200);
      break;

    default:
      Serial.println("x");
      analogWrite(pwm_la3, 0);
      break;
  }
}

void assignFromPayload(uint8_t payload[11]) {
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
    digitalWrite(dir_la1, LOW);
    analogWrite(pwm_la1, 200);
  }
  if (pot2_val < 245) {
    digitalWrite(dir_la2, LOW);
    analogWrite(pwm_la2, 200);
  }
  if (pot3_val > 115) {
    digitalWrite(dir_la3, LOW);
    analogWrite(pwm_la3, 200);
  }
}

