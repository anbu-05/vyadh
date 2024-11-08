//defining tasks to run in core 0 and 1
TaskHandle_t Task0;
TaskHandle_t Task1;


//------------------gps stuff------------------


#include <TinyGPSPlus.h>
#include <micro_ros_arduino.h>
#include <stdio.h>
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <std_msgs/msg/string.h>

// GPS Pins
#define RX 32
#define TX 33

// GPS setup
HardwareSerial neogps(1);
TinyGPSPlus gps;

// ROS 2 variables
rcl_publisher_t gps_data_publisher;
std_msgs__msg__String gps_data_msg;
rclc_executor_t executor;
rclc_support_t support;
rcl_allocator_t allocator;
rcl_node_t node;
rcl_timer_t timer;

#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){error_loop();}}
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){}}




void error_loop() {
  while (1) {
    delay(100);
  }
}

// Helper function to add leading zeros to time
String format_two_digits(int number) {
  return (number < 10 ? "0" + String(number) : String(number));
}

// Timer callback function to read GPS data and publish
void timer_callback(rcl_timer_t *timer, int64_t last_call_time) {
  RCLC_UNUSED(last_call_time);
  if (timer != NULL) {
    // Check if GPS data is valid
    if (gps.location.isValid()) {
      // Retrieve GPS data
      String latitude = String(gps.location.lat(), 6);
      String longitude = String(gps.location.lng(), 6);
      String altitude = String(gps.altitude.meters(), 2);  // Altitude with 2 decimal places

      // Format time with leading zeros for hours, minutes, and seconds
      String time_str = format_two_digits(gps.time.hour()) + ":" + 
                        format_two_digits(gps.time.minute()) + ":" + 
                        format_two_digits(gps.time.second());

      // Set GPS data in the required format
      String data = "Latitude: " + latitude + 
                    ", Longitude: " + longitude + 
                    ", Altitude: " + altitude + " m, " +
                    "Time: " + time_str;

      // Publish the GPS data
      snprintf(gps_data_msg.data.data, gps_data_msg.data.capacity, "%s", data.c_str());
      gps_data_msg.data.size = strlen(gps_data_msg.data.data);  // Update the message size
      RCSOFTCHECK(rcl_publish(&gps_data_publisher, &gps_data_msg, NULL));
    }
  }
}


//------------------controller stuff------------------

#include <HardwareSerial.h>

HardwareSerial uart2(2);

#define enL 26
#define in1L 25
#define in2L 33

#define enR 27
#define in1R 12
#define in2R 14

int speed = 150;
int rate = 0;

void gradual_speed(int final_speed, int rate) {
  for (int speed = 0; speed < final_speed; speed++) {
    analogWrite(enL, speed);
    analogWrite(enR, speed);
    //Serial.println(speed);
    delay(rate);
  }
  analogWrite(enL, final_speed);
  analogWrite(enR, final_speed);
}

//----------------------------------------------------------------------------

void setup() {
  set_microros_transports();
  Serial.begin(115200);
  uart2.begin(115200, SERIAL_8N1, 16, 17); //rx,tx

  pinMode(enL, OUTPUT);
  pinMode(in1L, OUTPUT);
  pinMode(in2L, OUTPUT);

  pinMode(enR, OUTPUT);
  pinMode(in1R, OUTPUT);
  pinMode(in2R, OUTPUT);
  neogps.begin(9600, SERIAL_8N1, RX, TX);

  delay(2000);

  allocator = rcl_get_default_allocator();

  // Initialize ROS 2 support
  RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));

  // Create ROS node
  RCCHECK(rclc_node_init_default(&node, "micro_ros_gps_node", "", &support));

  // Create a publisher for the GPS data
  RCCHECK(rclc_publisher_init_default(
    &gps_data_publisher,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, String),
    "gps_data"
  ));

  // Create timer for periodic callbacks (every 1 second)
  const unsigned int timer_timeout = 1000;  // 1 second
  RCCHECK(rclc_timer_init_default(
    &timer,
    &support,
    RCL_MS_TO_NS(timer_timeout),
    timer_callback
  ));

  // Create executor to handle the timer
  RCCHECK(rclc_executor_init(&executor, &support.context, 1, &allocator));
  RCCHECK(rclc_executor_add_timer(&executor, &timer));

  // Allocate memory for GPS message
  gps_data_msg.data.data = (char*)malloc(200 * sizeof(char));  // Adjust size as needed
  gps_data_msg.data.size = 0;
  gps_data_msg.data.capacity = 200;

  //----------------------------------------------------------

  xTaskCreatePinnedToCore(
      gps_loop, /* Function to implement the task */
      "Task1", /* Name of the task */
      10000,  /* Stack size in words */
      NULL,  /* Task input parameter */
      1,  /* Priority of the task */
      &Task0,  /* Task handle. */
      0);
  
  xTaskCreatePinnedToCore(
      controller_loop, /* Function to implement the task */
      "Task1", /* Name of the task */
      10000,  /* Stack size in words */
      NULL,  /* Task input parameter */
      0,  /* Priority of the task */
      &Task1,  /* Task handle. */
      1);
}

void gps_loop( void * pvParameters ) {
  for(;;) {
    // Continuously process incoming GPS data
    while (neogps.available() > 1) {
      gps.encode(neogps.read()); // Encodes the GPS data, no need to check if valid here
    }

    // Spin the ROS 2 executor to process callbacks
    RCSOFTCHECK(rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100)));
  }
}

char input = ' ';

void controller_loop( void * pvParameters ) {
  for(;;) {
    if (uart2.available() > 0) {
    input = uart2.read();
    Serial.println(input);
    switch (input){
      case 'w':
        digitalWrite(in1L, HIGH);
        digitalWrite(in2L, LOW); 
        digitalWrite(in1R, HIGH);
        digitalWrite(in2R, LOW);

        gradual_speed(speed, rate);
        Serial.println("forward");
        break;

      case 's':
        digitalWrite(in1L, LOW);
        digitalWrite(in2L, HIGH); 
        digitalWrite(in1R, LOW);
        digitalWrite(in2R, HIGH); 

        gradual_speed(speed, rate);
        Serial.println("backward");
        break;
      case 'd':
        digitalWrite(in1L, HIGH);
        digitalWrite(in2L, LOW); 
        digitalWrite(in1R, LOW);
        digitalWrite(in2R, HIGH); 

        gradual_speed(speed, rate);
        Serial.println("right");
        break;
      case 'a':
        digitalWrite(in1L, LOW);
        digitalWrite(in2L, HIGH); 
        digitalWrite(in1R, HIGH);
        digitalWrite(in2R, LOW); 

        gradual_speed(speed, rate);
        Serial.println("left");
        break;
      case 'x':
        analogWrite(enL, 0);
        digitalWrite(in1L, HIGH);
        digitalWrite(in2L, LOW); 

        analogWrite(enR, 0);
        digitalWrite(in1R, HIGH);
        digitalWrite(in2R, LOW);
        Serial.println("stopped");
        break;
      }
    }
  }
}

void loop() {
}


