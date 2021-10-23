#include <Servo.h>
Servo releaseServo;
Servo wingServo;
String state; //Stores current state of craft

bool ledOn; //Bool for dynamic blinking of LED
float ledDeltaTime; //Time since last LED switch [ms]

float lastExecutionTime; //Time of last loop execution

#include <Adafruit_LSM6DS33.h>
// For SPI mode, we need a CS pin
#define LSM_CS 10
// For software-SPI mode we need SCK/MOSI/MISO pins
#define LSM_SCK 13
#define LSM_MISO 12
#define LSM_MOSI 11

Adafruit_LSM6DS33 lsm6ds33;

//BME Setup
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme;

long ledDelay = 2000;
long preDelay = 1000; //Different launch states led frequencies
long launchDelay = 500;

int commsDelay = 1000; //Delay on xBee, so we don't flood the airwaves
long commsDeltaTime;
long lastTransmission;

bool buzzing = false;

long lastTime = 0;

float lastAltitude = 0;
float offsetAltitude = 0;

int led = 13;
int externalLED = 11;

int buzzer = 10;

void setup() {
  releaseServo.attach(3);
  releaseServo.write(130);
  wingServo.attach(4);
  wingServo.write(110);
  state = "preLaunch";
  ledDeltaTime = 0;
  lastExecutionTime = 0;
  lastTransmission = 0;
  Serial.begin(9600); //Serial port for comms

  pinMode(led, OUTPUT);
  pinMode(externalLED, OUTPUT);

  pinMode(buzzer, OUTPUT);

  //BME Initialization
  unsigned status;
    
    // default settings
    status = bme.begin();  
    // You can also pass in a Wire library object like &Wire2
    // status = bme.begin(0x76, &Wire2)
    if (!status) {
        Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
        Serial.print("SensorID was: 0x"); Serial.println(bme.sensorID(),16);
        Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
        Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
        Serial.print("        ID of 0x60 represents a BME 280.\n");
        Serial.print("        ID of 0x61 represents a BME 680.\n");
        while (1) delay(10);
    }

  // Start the serial port for the transmitter
  Serial3.begin(9600);

  //Start the serial port for the openlog
  Serial5.begin(9600);

  Serial2.begin(9600);

  offsetAltitude = bme.readAltitude(SEALEVELPRESSURE_HPA);

  Serial.println("Adafruit LSM6DS33 test!");

  if (!lsm6ds33.begin_I2C()) {
    // if (!lsm6ds33.begin_SPI(LSM_CS)) {
    // if (!lsm6ds33.begin_SPI(LSM_CS, LSM_SCK, LSM_MISO, LSM_MOSI)) {
    Serial.println("Failed to find LSM6DS33 chip");
    while (1) {
      delay(10);
    }
  }

  Serial.println("LSM6DS33 Found!");

  // lsm6ds33.setAccelRange(LSM6DS_ACCEL_RANGE_2_G);
  Serial.print("Accelerometer range set to: ");
  switch (lsm6ds33.getAccelRange()) {
  case LSM6DS_ACCEL_RANGE_2_G:
    Serial.println("+-2G");
    break;
  case LSM6DS_ACCEL_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case LSM6DS_ACCEL_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case LSM6DS_ACCEL_RANGE_16_G:
    Serial.println("+-16G");
    break;
  }

  // lsm6ds33.setGyroRange(LSM6DS_GYRO_RANGE_250_DPS);
  Serial.print("Gyro range set to: ");
  switch (lsm6ds33.getGyroRange()) {
  case LSM6DS_GYRO_RANGE_125_DPS:
    Serial.println("125 degrees/s");
    break;
  case LSM6DS_GYRO_RANGE_250_DPS:
    Serial.println("250 degrees/s");
    break;
  case LSM6DS_GYRO_RANGE_500_DPS:
    Serial.println("500 degrees/s");
    break;
  case LSM6DS_GYRO_RANGE_1000_DPS:
    Serial.println("1000 degrees/s");
    break;
  case LSM6DS_GYRO_RANGE_2000_DPS:
    Serial.println("2000 degrees/s");
    break;
  case ISM330DHCX_GYRO_RANGE_4000_DPS:
    break; // unsupported range for the DS33
  }

  // lsm6ds33.setAccelDataRate(LSM6DS_RATE_12_5_HZ);
  Serial.print("Accelerometer data rate set to: ");
  switch (lsm6ds33.getAccelDataRate()) {
  case LSM6DS_RATE_SHUTDOWN:
    Serial.println("0 Hz");
    break;
  case LSM6DS_RATE_12_5_HZ:
    Serial.println("12.5 Hz");
    break;
  case LSM6DS_RATE_26_HZ:
    Serial.println("26 Hz");
    break;
  case LSM6DS_RATE_52_HZ:
    Serial.println("52 Hz");
    break;
  case LSM6DS_RATE_104_HZ:
    Serial.println("104 Hz");
    break;
  case LSM6DS_RATE_208_HZ:
    Serial.println("208 Hz");
    break;
  case LSM6DS_RATE_416_HZ:
    Serial.println("416 Hz");
    break;
  case LSM6DS_RATE_833_HZ:
    Serial.println("833 Hz");
    break;
  case LSM6DS_RATE_1_66K_HZ:
    Serial.println("1.66 KHz");
    break;
  case LSM6DS_RATE_3_33K_HZ:
    Serial.println("3.33 KHz");
    break;
  case LSM6DS_RATE_6_66K_HZ:
    Serial.println("6.66 KHz");
    break;
  }

  // lsm6ds33.setGyroDataRate(LSM6DS_RATE_12_5_HZ);
  Serial.print("Gyro data rate set to: ");
  switch (lsm6ds33.getGyroDataRate()) {
  case LSM6DS_RATE_SHUTDOWN:
    Serial.println("0 Hz");
    break;
  case LSM6DS_RATE_12_5_HZ:
    Serial.println("12.5 Hz");
    break;
  case LSM6DS_RATE_26_HZ:
    Serial.println("26 Hz");
    break;
  case LSM6DS_RATE_52_HZ:
    Serial.println("52 Hz");
    break;
  case LSM6DS_RATE_104_HZ:
    Serial.println("104 Hz");
    break;
  case LSM6DS_RATE_208_HZ:
    Serial.println("208 Hz");
    break;
  case LSM6DS_RATE_416_HZ:
    Serial.println("416 Hz");
    break;
  case LSM6DS_RATE_833_HZ:
    Serial.println("833 Hz");
    break;
  case LSM6DS_RATE_1_66K_HZ:
    Serial.println("1.66 KHz");
    break;
  case LSM6DS_RATE_3_33K_HZ:
    Serial.println("3.33 KHz");
    break;
  case LSM6DS_RATE_6_66K_HZ:
    Serial.println("6.66 KHz");
    break;
  }

  lsm6ds33.configInt1(false, false, true); // accelerometer DRDY on INT1
  lsm6ds33.configInt2(false, true, false); // gyro DRDY on INT2
}

void loop() {

  
  //Time Determination
  ledDeltaTime = millis() - lastExecutionTime ;
  commsDeltaTime = millis() - lastTransmission;
  long deltaTime = millis() - lastTime;

  //These are essential functions for flight that are executed regardless of flight state - for now they are just empty functions, 
  //but as more sensors/equipment get hooked up to the craft, they will be filled in
  float altitudeHeight = bme.readAltitude(SEALEVELPRESSURE_HPA) - offsetAltitude;
  float pressure = bme.readPressure();
  float temperature = bme.readTemperature();
  float humidity = bme.readHumidity();
  //String gpsPosition = getPosition(); //Will this decleration come back to bite me? Yes. Yes it will

  //State machine
  if (state == "preLaunch") {
    if (Serial3.readString() == "go") {
      state = "goForLaunch";
    }
    ledDelay = 3000;
  } else if (state == "goForLaunch") {
    ledDelay = 500;
    commsDelay = 500;
    ledDelay = launchDelay;
    if (altitudeHeight > 5) {
      state = "ascending";
    }
  } else if (state == "ascending") {
    if (altitudeHeight > 300  ) {
      state = "dropping";
    }
  } else if (state == "dropping") {
    drop();
    state = "descending"; {
    }
  } else if (state == "descending") {
    ledDelay = 2000;
    if (altitudeHeight < 10) {
      buzzing = true;
    }
  } else if (state == "landing") {
    buzzing = true;
  }

  //override
  if (Serial3.readString() == "drop") {
    drop();
  }

  if (buzzing) {
    digitalWrite(buzzer, HIGH);
  }

  //  /* Get a new normalized sensor event */
  sensors_event_t accel;
  sensors_event_t gyro;
  sensors_event_t temp;
  lsm6ds33.getEvent(&accel, &gyro, &temp);

  Serial.print("\t\tTemperature ");
  Serial.print(temp.temperature);
  Serial.println(" deg C");

  /* Display the results (acceleration is measured in m/s^2) */
  Serial.print("\t\tAccel X: ");
  Serial.print(accel.acceleration.x);
  float xAccel = accel.acceleration.x;
  Serial.print(" \tY: ");
  Serial.print(accel.acceleration.y);
  float yAccel = accel.acceleration.y;
  Serial.print(" \tZ: ");
  Serial.print(accel.acceleration.z);
  float zAccel = accel.acceleration.z;
  Serial.println(" m/s^2 ");
  

  /* Display the results (rotation is measured in rad/s) */
  Serial.print("\t\tGyro X: ");
  Serial.print(gyro.gyro.x);
  float gyroX = gyro.gyro.x;
  Serial.print(" \tY: ");
  Serial.print(gyro.gyro.y);
  float gyroY = gyro.gyro.y;
  Serial.print(" \tZ: ");
  Serial.print(gyro.gyro.z);
  float gyroZ = gyro.gyro.z;
  Serial.println(" radians/s ");
  Serial.println();
  
  //Blink the LED
  blinkStatus(ledDelay);

  String currentData = String(millis()) +"," + state +","+ String(altitudeHeight) +","+ String(pressure) +","+ String(temperature) +","+ String(humidity) +","+ String(xAccel) +","+ String(yAccel) +","+ String(zAccel) +","+ String(gyroX) +","+ String(gyroY) +","+ String(gyroZ);
  //Transmit
  transmit(commsDelay, currentData);
  //transmit(commsDelay, state + " " + String(zAccel));

  //Save
  Serial5.println(currentData);
}

//Also going to be doing SD card saving here, as data points will be the same 
void transmit(long delayTime, String data) {
  if (commsDeltaTime > delayTime) {
    Serial3.println(data);
    lastTransmission = millis();
  }
}

void blinkStatus(long delayTime) {
  if (ledDeltaTime >= delayTime) {
    if (ledOn == 0) {
      digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level
      digitalWrite(externalLED, HIGH);
      ledOn = 1;
    } else {
      digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
      digitalWrite(externalLED, LOW);
      ledOn = 0;
    }
    lastExecutionTime = millis();
  }
}

void drop() {
  releaseServo.write(40);
  wingServo.write(20);
}
