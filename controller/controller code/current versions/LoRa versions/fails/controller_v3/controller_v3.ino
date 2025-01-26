//#################################
//#################################

//LoRa library by Sandeep Mistry

//transmitter code

#include <SPI.h>
#include <LoRa.h>

// Pin definitions for LoRa SX1278
#define ss 5          // Slave Select (NSS)
#define rst 2        // Reset
#define dio0 4       // Digital I/O 0


#define joy1_x 27
#define joy1_y 14
#define but1 12

#define joy2_x 32
#define joy2_y 35
#define but2 34

#define joy3_x 26
#define joy3_y 25
#define but3 33

#define ra_pos 13
#define mde_slct 22
#define mde_slct_2 21
#define mde_slct_3 16

// #define t_LED 27
// #define r_LED 14
// #define s_LED 12




void setup() {

  //initializing pins
  // Joystick 1
  pinMode(joy1_x, INPUT);
  pinMode(joy1_y, INPUT);
  pinMode(but1, INPUT);

  // Joystick 2
  pinMode(joy2_x, INPUT);
  pinMode(joy2_y, INPUT);
  pinMode(but2, INPUT);

  //joystick 3
  pinMode(joy3_x, INPUT);
  pinMode(joy3_y, INPUT);
  pinMode(but3, INPUT);

  //slide switches
  pinMode(ra_pos, INPUT);
  pinMode(mde_slct, INPUT);
  pinMode(mde_slct_2, INPUT);
  pinMode(mde_slct_3, INPUT);

  Serial.begin(115200);
  Serial.println("begin");
  while (!Serial);

  Serial.println("LoRa Transmitter Setup");

  // Configure LoRa module
  LoRa.setPins(ss, rst, dio0);

  if (!LoRa.begin(433E6)) { // Set frequency to 433 MHz
    Serial.println("Starting LoRa failed! Check connections and power supply.");
    while (1);
  }

  Serial.println("LoRa initialization successful!");
}

uint8_t joystick1_x;
uint8_t joystick1_y;
uint8_t button1;

uint8_t joystick2_x;
uint8_t joystick2_y;
uint8_t button2;

uint8_t joystick3_x;
uint8_t joystick3_y;
uint8_t button3;

uint8_t robotic_arm_position;
uint8_t mode_select;
uint8_t mode_select_2;
uint8_t mode_select_3;

const uint8_t joystick_idle_min = 96; // Minimum idle value
const uint8_t joystick_idle_max = 148; // Maximum idle value

unsigned long last_active_time = 0; // Tracks the last time activity was detected
const unsigned long idle_timeout = 50; // Delay time in milliseconds

uint8_t payload[13];

#define BUFFER_SIZE 2

uint8_t joystick1_x_buffer[BUFFER_SIZE] = {0};
uint8_t joystick1_y_buffer[BUFFER_SIZE] = {0};
uint8_t button1_buffer[BUFFER_SIZE] = {1};

uint8_t joystick2_x_buffer[BUFFER_SIZE] = {0};
uint8_t joystick2_y_buffer[BUFFER_SIZE] = {0};
uint8_t button2_buffer[BUFFER_SIZE] = {1};

uint8_t joystick3_x_buffer[BUFFER_SIZE] = {0};
uint8_t joystick3_y_buffer[BUFFER_SIZE] = {0};
uint8_t button3_buffer[BUFFER_SIZE] = {1};

uint8_t robotic_arm_position_buffer[BUFFER_SIZE] = {0};
uint8_t mode_select_buffer[BUFFER_SIZE] = {0};
uint8_t mode_select_2_buffer[BUFFER_SIZE] = {0};
uint8_t mode_select_3_buffer[BUFFER_SIZE] = {0};

int buffer_index = 0;

uint8_t joystick_movingAverage(uint8_t *buffer, uint8_t new_value) {
  buffer[buffer_index] = new_value;

  int sum = 0;
  for (int i = 0; i < BUFFER_SIZE; i++) {
    sum += buffer[i];
    // Serial.print(buffer[i]);
    // Serial.print(" ");
  }
  // Serial.print(" = ");
  // Serial.print(sum);
  // Serial.println();

  return sum / BUFFER_SIZE; 
}

uint8_t button_movingAverage(uint8_t *buffer, uint8_t new_value) {
  buffer[buffer_index] = new_value;

  float sum = 0;
  for (int i = 0; i < BUFFER_SIZE; i++) {
    sum += buffer[i];
    sum += buffer[i];
    // Serial.print(buffer[i]);
    // Serial.print(" ");
  }

  // Serial.print(" = ");
  // Serial.print(sum);
  // Serial.println();

  sum / BUFFER_SIZE; 
  uint8_t button_state = (sum >= 0.5) ? 1 : 0;
  return button_state;
}

void loop() {

  //buffer index cycle
  buffer_index++;
  if (buffer_index >= BUFFER_SIZE) {
    buffer_index = 0;
  }

  // Joystick 1
  joystick1_x = analogRead(joy1_x) >> 4;
  joystick1_y = analogRead(joy1_y) >> 4;
  button1 = digitalRead(but1);

  // Joystick 2
  joystick2_x = analogRead(joy2_x) >> 4;
  joystick2_y = analogRead(joy2_y) >> 4;
  button2 = digitalRead(but2);

  //joystick 3
  joystick3_x = analogRead(joy3_x) >> 4;
  joystick3_y = analogRead(joy3_y) >> 4;
  button3 = digitalRead(but3);

  robotic_arm_position = analogRead(ra_pos) >> 4;
  mode_select = digitalRead(mde_slct);
  mode_select_2 = digitalRead(mde_slct_2);
  mode_select_3 = digitalRead(mde_slct_3);

  //applying average

  // Joystick 1
  //Serial.println("j1 avg");
  joystick1_x = joystick_movingAverage(joystick1_x_buffer, joystick1_x);
  joystick1_y = joystick_movingAverage(joystick1_y_buffer, joystick1_y);
  button1 = button_movingAverage(button1_buffer, button1);

  //Serial.println("j2 avg");
  // Joystick 2
  joystick2_x = joystick_movingAverage(joystick2_x_buffer, joystick2_x);
  joystick2_y = joystick_movingAverage(joystick2_y_buffer, joystick2_y);
  button2 = button_movingAverage(button2_buffer, button2);

  //Serial.println("j3 avg");
  //joystick 3
  joystick3_x = joystick_movingAverage(joystick3_x_buffer, joystick3_x);
  joystick3_y = joystick_movingAverage(joystick3_y_buffer, joystick3_y);
  button3 = button_movingAverage(button3_buffer, button3);

  //Serial.println("others avg");
  //robotic_arm_position = joystick_movingAverage(robotic_arm_position_buffer, robotic_arm_position);
  mode_select = button_movingAverage(mode_select_buffer, mode_select);
  mode_select_2 = button_movingAverage(mode_select_2_buffer, mode_select_2);
  mode_select_3 = button_movingAverage(mode_select_3_buffer, mode_select_3);

  

    // Check for significant changes
  bool joysticks_idle_now = 
      (joystick1_x >= joystick_idle_min && joystick1_x <= joystick_idle_max) &&
      (joystick1_y >= joystick_idle_min && joystick1_y <= joystick_idle_max) &&
      (joystick2_x >= joystick_idle_min && joystick2_x <= joystick_idle_max) &&
      (joystick2_y >= joystick_idle_min && joystick2_y <= joystick_idle_max) &&
      (joystick3_x >= joystick_idle_min && joystick3_x <= joystick_idle_max) &&
      (joystick3_y >= joystick_idle_min && joystick3_y <= joystick_idle_max);

  bool buttons_and_slides_idle_now =
      (button1 == 1) && (button2 == 1) && (button3 == 1) &&
      (robotic_arm_position == 0) && (mode_select == 0) && (mode_select_2 == 0) && (mode_select_3 == 0);

  // Only send data if there's a change

  if (!joysticks_idle_now || !buttons_and_slides_idle_now) {
    last_active_time = millis(); // Update the last activity timestamp
  }

  if (millis() - last_active_time < idle_timeout) {

    assignToPayload(payload);
    printValues();

    LoRa.beginPacket();
    LoRa.write(payload, sizeof(payload));
    LoRa.endPacket();

    Serial.println("Updated values sent: ");
  }
}

void assignToPayload(uint8_t payload[13]) {
  payload[0] = joystick1_x;
  payload[1] = joystick1_y;
  payload[2] = button1;

  payload[3] = joystick2_x;
  payload[4] = joystick2_y;
  payload[5] = button2;

  payload[6] = joystick3_x;
  payload[7] = joystick3_y;
  payload[8] = button3;

  payload[9] = robotic_arm_position;
  payload[10] = mode_select;
  payload[11] = mode_select_2;
  payload[12] = mode_select_3;
}

void printValues() {
  Serial.print(joystick1_x); Serial.print(","); Serial.print(joystick1_y); Serial.print(","); Serial.print(button1); Serial.println("  ");
  Serial.print(joystick2_x); Serial.print(","); Serial.print(joystick2_y); Serial.print(","); Serial.print(button2); Serial.println("  ");
  Serial.print(joystick3_x); Serial.print(","); Serial.print(joystick3_y); Serial.print(","); Serial.print(button3); Serial.println("  ");
  Serial.print(robotic_arm_position); Serial.print(","); Serial.print(mode_select); Serial.print(","); Serial.print(mode_select_2); Serial.print(","); Serial.print(mode_select_3); Serial.println("  "); 

  Serial.println("  ");
  Serial.println("  ");
  Serial.println("  ");
}