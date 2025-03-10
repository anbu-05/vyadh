//#################################
//#################################

//LoRa by Sandeep Mistry

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
uint8_t Button1;

uint8_t joystick2_x;
uint8_t joystick2_y;
uint8_t Button2;

uint8_t joystick3_x;
uint8_t joystick3_y;
uint8_t Button3;

uint8_t robotic_arm_position;
uint8_t mode_select;
uint8_t mode_select_2;
uint8_t mode_select_3;

const uint8_t joystick_idle_min = 100; // Minimum idle value
const uint8_t joystick_idle_max = 156; // Maximum idle value

unsigned long last_active_time = 0; // Tracks the last time activity was detected
const unsigned long idle_timeout = 500; // Delay time in milliseconds

uint8_t payload[13];
uint8_t feedbackPayload[3];

void loop() {

  // Joystick 1
  joystick1_x = analogRead(joy1_x) >> 4;
  joystick1_y = analogRead(joy1_y) >> 4;
  Button1 = digitalRead(but1);

  // Joystick 2
  joystick2_x = analogRead(joy2_x) >> 4;
  joystick2_y = analogRead(joy2_y) >> 4;
  Button2 = digitalRead(but2);

  //joystick 3
  joystick3_x = analogRead(joy3_x) >> 4;
  joystick3_y = analogRead(joy3_y) >> 4;
  Button3 = digitalRead(but3);

  robotic_arm_position = analogRead(ra_pos) >> 4;
  mode_select = digitalRead(mde_slct);
  mode_select_2 = digitalRead(mde_slct_2);
  mode_select_3 = digitalRead(mde_slct_3);


    // Check for significant changes
  bool joysticks_idle_now = 
      (joystick1_x >= joystick_idle_min && joystick1_x <= joystick_idle_max) &&
      (joystick1_y >= joystick_idle_min && joystick1_y <= joystick_idle_max) &&
      (joystick2_x >= joystick_idle_min && joystick2_x <= joystick_idle_max) &&
      (joystick2_y >= joystick_idle_min && joystick2_y <= joystick_idle_max) &&
      (joystick3_x >= joystick_idle_min && joystick3_x <= joystick_idle_max) &&
      (joystick3_y >= joystick_idle_min && joystick3_y <= joystick_idle_max);

  bool buttons_and_slides_idle_now =
      (Button1 == 1) && (Button2 == 1) && (Button3 == 1) &&
      (robotic_arm_position == 0) && (mode_select == 0) && (mode_select_2 == 0) && (mode_select_3 == 0);


  if (!joysticks_idle_now || !buttons_and_slides_idle_now) {
    last_active_time = millis(); // Update the last activity timestamp
  }

  // Only send data if there's a change
  if (millis() - last_active_time < idle_timeout) {
    printValues();
    // Assign updated values to payload
    assignToPayload(payload);
    // Send data via LoRa
    LoRa.beginPacket();
    LoRa.write(payload, sizeof(payload));
    LoRa.endPacket();

    // Print updated values for debugging
    Serial.println("Updated values sent: ");
  }

  assignFeedbackFromLoRa();

  Serial.print("traversal mode: ");
  Serial.println(feedbackPayload[2]);
  Serial.print("robotic arm: ");
  Serial.println(feedbackPayload[1]);
  Serial.print("science kit: ");
  Serial.println(feedbackPayload[0]);
  Serial.println("  ");
  Serial.println("  ");
  Serial.println("  ");

}

void assignFeedbackFromLoRa() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    Serial.println("###Received packet###");

    // Read packet contents 
    int i = LoRa.available() - 1;
    while (i) {
      Serial.println(i);
      feedbackPayload[i-1] = LoRa.read();
      Serial.println(feedbackPayload[i-1]);
      i--;
    }

    // Print RSSI (signal strength)
    Serial.print("RSSI: ");
    Serial.println(LoRa.packetRssi());
    //Serial.println(LoRa.packetFrequencyError());
  }
}

void assignToPayload(uint8_t payload[13]) {
  payload[0] = joystick1_x;
  payload[1] = joystick1_y;
  payload[2] = Button1;

  payload[3] = joystick2_x;
  payload[4] = joystick2_y;
  payload[5] = Button2;

  payload[6] = joystick3_x;
  payload[7] = joystick3_y;
  payload[8] = Button3;

  payload[9] = robotic_arm_position;
  payload[10] = mode_select;
  payload[11] = mode_select_2;
  payload[12] = mode_select_3;
}

void printValues() {
  Serial.print(joystick1_x); Serial.print(","); Serial.print(joystick1_y); Serial.print(","); Serial.print(Button1); Serial.println("  ");
  Serial.print(joystick2_x); Serial.print(","); Serial.print(joystick2_y); Serial.print(","); Serial.print(Button2); Serial.println("  ");
  Serial.print(joystick3_x); Serial.print(","); Serial.print(joystick3_y); Serial.print(","); Serial.print(Button3); Serial.println("  ");
  Serial.print(robotic_arm_position); Serial.print(","); Serial.print(mode_select); Serial.print(","); Serial.print(mode_select_2); Serial.print(","); Serial.print(mode_select_3); Serial.println("  "); 

  Serial.println("  ");
  Serial.println("  ");
  Serial.println("  ");
}