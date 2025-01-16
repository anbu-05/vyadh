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




void setup() {

  //initializing pins
  // Joystick 1
  pinMode(joy1_x, INPUT);
  pinMode(joy1_y, INPUT);
  pinMode(but1, INPUT_PULLDOWN);

  // Joystick 2
  pinMode(joy2_x, INPUT);
  pinMode(joy2_y, INPUT);
  pinMode(but2, INPUT_PULLDOWN);

  //joystick 3
  pinMode(joy3_x, INPUT);
  pinMode(joy3_y, INPUT);
  pinMode(but3, INPUT_PULLDOWN);

  //slide switches
  pinMode(ra_pos, INPUT);
  pinMode(mde_slct, INPUT);

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

// uint8_t prev_joystick1_x = 0, prev_joystick1_y = 0, prev_Button1 = 0;
// uint8_t prev_joystick2_x = 0, prev_joystick2_y = 0, prev_Button2 = 0;
// uint8_t prev_joystick3_x = 0, prev_joystick3_y = 0, prev_Button3 = 0;
// uint8_t prev_robotic_arm_position = 0, prev_mode_select = 0;

// Define idle range for joysticks (around center position)
const uint8_t joystick_idle_min = 120; // Minimum idle value
const uint8_t joystick_idle_max = 135; // Maximum idle value

uint8_t payload[11];

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
  mode_select = analogRead(mde_slct) >> 4;

  print_data();

    // Check for significant changes
  bool joysticks_untouched = 
      (joystick1_x >= joystick_idle_min && joystick1_x <= joystick_idle_max) &&
      (joystick1_y >= joystick_idle_min && joystick1_y <= joystick_idle_max) &&
      (joystick2_x >= joystick_idle_min && joystick2_x <= joystick_idle_max) &&
      (joystick2_y >= joystick_idle_min && joystick2_y <= joystick_idle_max) &&
      (joystick3_x >= joystick_idle_min && joystick3_x <= joystick_idle_max) &&
      (joystick3_y >= joystick_idle_min && joystick3_y <= joystick_idle_max);

  bool buttons_and_slides_untouched =
      (Button1 == LOW) && (Button2 == LOW) && (Button3 == LOW) &&
      (robotic_arm_position == LOW) && (mode_select == LOW);

  // Only send data if there's a change
  if (!joysticks_untouched || !buttons_and_slides_untouched) {
    // Assign updated values to payload
    assignToPayload(payload);

    // Send data via LoRa
    LoRa.beginPacket();
    LoRa.write(payload, sizeof(payload));
    LoRa.endPacket();

    // Print updated values for debugging
    Serial.print("Updated values sent: ");
    for (int i = 0; i < 11; i++) {
      Serial.print(payload[i]);
      Serial.println(" ");
    }
    Serial.println();
  }
}

void assignToPayload(uint8_t payload[11]) {
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
}

void print_data() {
  Serial.print(joystick1_x); Serial.print(","); Serial.print(joystick1_y); Serial.print(","); Serial.print(Button1); Serial.println("  ");
  Serial.print(joystick2_x); Serial.print(","); Serial.print(joystick2_y); Serial.print(","); Serial.print(Button2); Serial.println("  ");
  Serial.print(joystick3_x); Serial.print(","); Serial.print(joystick3_y); Serial.print(","); Serial.print(Button3); Serial.println("  ");
  Serial.print(robotic_arm_position); Serial.print(","); Serial.print(mode_select); Serial.println("  ");

  Serial.println("  ");
  Serial.println("  ");
  Serial.println("  ");
}