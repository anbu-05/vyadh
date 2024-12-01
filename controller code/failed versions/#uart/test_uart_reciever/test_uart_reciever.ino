#include <HardwareSerial.h>
#include <vector>

HardwareSerial uart2(2);

#define TXD1 14
#define RXD1 12

struct test {
  int var1;
  int var2;
};


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  uart2.begin(57600, SERIAL_8N1, RXD1, TXD1);
}

test object;
std::vector<int> payload;

void loop() {
  // put your main code here, to run repeatedly:
  if (uart2.available()) {
    uart2.readBytes(&payload, sizeof(object));

    memcpy(&object, &payload, sizeof(object));

    Serial.println(object.var1);
    Serial.println(object.var2);
  }
}

void recieveObject(test* object) {
  uint8_t data[2];
  uart2.readBytes((uint8_t*)&data, sizeof(data));
  object->var1 = data[0];
  object->var2 = data[1];
}