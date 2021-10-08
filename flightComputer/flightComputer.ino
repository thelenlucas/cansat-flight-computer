String state; //Stores current state of craft

bool ledOn; //Bool for dynamic blinking of LED
float ledDeltaTime; //Time since last LED switch [ms]

float lastExecutionTime; //Time of last loop execution

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

int led = 13;
int externalLED = 4;

void setup() {
  state = "preLaunch";
  ledDeltaTime = 0;
  lastExecutionTime = 0;
  lastTransmission = 0;
  Serial.begin(9600); //Serial port for comms

  pinMode(led, OUTPUT);
  pinMode(externalLED, OUTPUT);

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
  Serial5.begin(9600);
    
}

void loop() {
  //Time Determination
  ledDeltaTime = millis() - lastExecutionTime ;
  commsDeltaTime = millis() - lastTransmission;
  long deltaTime = millis() - lastTime;

  //These are essential functions for flight that are executed regardless of flight state - for now they are just empty functions, 
  //but as more sensors/equipment get hooked up to the craft, they will be filled in
  float altitudeHeight = bme.readAltitude(SEALEVELPRESSURE_HPA);
  float pressure = bme.readPressure();
  float temperature = bme.readTemperature();
  float humidity = bme.readHumidity();
  //String gpsPosition = getPosition(); //Will this decleration come back to bite me? Yes. Yes it will

  float deltaAltitude = (altitudeHeight - lastAltitude)/(deltaTime/1000); //Unless I'm very bad at both math and programming, this should be our vertical velocity, in m/s

  //State machine
  if (state == "preLaunch") {
    if (Serial5.readString() == "go") {
      ledDelay = preDelay;
      state = "goForLaunch";
    }
    ledDelay = preDelay;
  } else if (state == "goForLaunch") {
    commsDelay = 500;
    ledDelay = launchDelay;
    if (altitudeHeight > 10) {
      state = "ascending";
    }
  } else if (state == "ascending") {
    if (altitudeHeight > 500) {
      state = "dropping";
    }
  } else if (state == "dropping") {
    drop();
    state = "descending"; {
    }
  } else if (state == "descending") {
    if (deltaAltitude < 1) {
      state == "landing";
    }
  } else if (state == "landing") {
    buzzing = true;
  }

  if (buzzing) {
    //Buzz
  }

  
  //Blink the LED
  blinkStatus(ledDelay);

  //Transmit
  transmit(commsDelay, String(deltaAltitude));
}

//Also going to be doing SD card saving here, as data points will be the same 
void transmit(long delayTime, String data) {
  if (commsDeltaTime > delayTime) {
    Serial5.println(data);
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
  
}
