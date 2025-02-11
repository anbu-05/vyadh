#include <Wire.h>
#include <BME280I2C.h>         // BME280 sensor library
#include <DHT.h>               // DHT22 sensor library
//#include <DFRobot_AS7341.h>    // AS7341 spectral sensor library

// Pin Definitions
#define MQ5_PIN 32             // Analog pin for MQ5 gas sensor
#define DHT_PIN 4              // GPIO pin for DHT22 sensor
#define DHT_TYPE DHT22         // Define DHT sensor type
#define CO2_PIN 12             // Analog pin for CO2 sensor (MQ-135)
#define MOISTURE_PIN 13        // Analog pin for Moisture sensor
#define I2C_SDA 21             // I2C SDA pin for AS7341 and BME280
#define I2C_SCL 22             // I2C SCL pin for AS7341 and BME280

#define SERIAL_BAUD 115200

// Sensor Objects
DHT dht(DHT_PIN, DHT_TYPE);         // DHT22 temperature and humidity sensor
BME280I2C bme;                      // BME280 environmental sensor
DFRobot_AS7341 as7341;              // AS7341 visible light sensor

// Function Prototypes
void printBME280Data(Stream* client);

void setup() {
  Serial.begin(SERIAL_BAUD);        // Initialize serial communication
  while (!Serial) {}               // Wait for Serial Monitor

  // Initialize pins
  pinMode(MOISTURE_PIN, INPUT);
  pinMode(MQ5_PIN, INPUT);
  pinMode(CO2_PIN, INPUT);

  // Initialize I2C communication
  Wire.begin(I2C_SDA, I2C_SCL);

  // Initialize DHT sensor
  dht.begin();

  // Initialize BME280 sensor
  while (!bme.begin()) {
    Serial.println("Could not find BME280 sensor!");
    delay(1000);
  }
  switch (bme.chipModel()) {
    case BME280::ChipModel_BME280:
      Serial.println("Found BME280 sensor! Success.");
      break;
    case BME280::ChipModel_BMP280:
      Serial.println("Found BMP280 sensor! No Humidity available.");
      break;
    default:
      Serial.println("Found UNKNOWN sensor! Error!");
  }

  // Initialize AS7341 sensor
  /*if (!as7341.begin()) {
    Serial.println("AS7341 initialization failed!");
  } else {
    as7341.enableLed(HIGH);
    Serial.println("AS7341 sensor initialized");
  }*/
}

void loop() {
  // ---- Moisture Sensor ----
  int moisture = analogRead(MOISTURE_PIN);
  Serial.print("Moisture Level: ");
  Serial.print(moisture);
  Serial.print(" - ");
  if (moisture >= 1000) {
    Serial.println("Sensor is not in the soil or DISCONNECTED");
  } else if (moisture >= 600) {
    Serial.println("Soil is DRY");
  } else if (moisture >= 370) {
    Serial.println("Soil is HUMID");
  } else {
    Serial.println("Sensor in WATER");
  }

  // ---- DHT22 Sensor ----
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor");
  } else {
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print(" °C, Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");
  }

  // ---- BME280 Sensor ----
  printBME280Data(&Serial);

  // ---- MQ5 Gas Sensor ----
  int mq5Value = analogRead(MQ5_PIN);
  Serial.print("MQ5 Gas Sensor Value: ");
  Serial.println(mq5Value);

  // ---- CO2 Sensor ----
  int co2Value = analogRead(CO2_PIN);
  Serial.print("CO2 Sensor Value: ");
  Serial.println(co2Value);

  // ---- AS7341 Spectral Sensor ----
 /* as7341.startMeasure(as7341.eF1F4ClearNIR);
  DFRobot_AS7341::sModeOneData_t data1 = as7341.readSpectralDataOne();
  Serial.print("F1 (405-425nm): "); Serial.println(data1.ADF1);
  Serial.print("F2 (435-455nm): "); Serial.println(data1.ADF2);
  Serial.print("F3 (470-490nm): "); Serial.println(data1.ADF3);
  Serial.print("F4 (505-525nm): "); Serial.println(data1.ADF4);

  as7341.startMeasure(as7341.eF5F8ClearNIR);
  DFRobot_AS7341::sModeTwoData_t data2 = as7341.readSpectralDataTwo();
  Serial.print("F5 (545-565nm): "); Serial.println(data2.ADF5);
  Serial.print("F6 (580-600nm): "); Serial.println(data2.ADF6);
  Serial.print("F7 (620-640nm): "); Serial.println(data2.ADF7);
  Serial.print("F8 (670-690nm): "); Serial.println(data2.ADF8);
  Serial.print("Clear: "); Serial.println(data2.ADCLEAR);
  Serial.print("NIR: "); Serial.println(data2.ADNIR);*/

  delay(2000);  // Delay for readability
}

// Function to print BME280 data
void printBME280Data(Stream* client) {
  float temp(NAN), hum(NAN), pres(NAN);

  BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
  BME280::PresUnit presUnit(BME280::PresUnit_Pa);

  bme.read(pres, temp, hum, tempUnit, presUnit);

  client->print("Temp: ");
  client->print(temp);
  client->print("°" + String(tempUnit == BME280::TempUnit_Celsius ? 'C' : 'F'));
  client->print("\t\tHumidity: ");
  client->print(hum);
  client->print("% RH");
  client->print("\t\tPressure: ");
  client->print(pres);
  client->println(" Pa");
}