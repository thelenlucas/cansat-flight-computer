String state; //Stores current state of craft

bool ledOn; //Bool for dynamic blinking of LED
float ledDeltaTime; //Time since last LED switch [ms]

float lastExecutionTime; //Time of last loop execution

long ledDelay = 2000;
long preDelay = 1000; //Different launch states led frequencies

int commsDelay = 1000; //Delay on xBee, so we don't flood the airwaves
long commsDeltaTime;
long lastTransmission;

bool buzzing = false;

long lastTime = 0;

float lastAltitude = 0;

int led = 11;

void setup() {
  state = "preLaunch";
  ledDeltaTime = 0;
  lastExecutionTime = 0;
  lastTransmission = 0;
  Serial.begin(9600); //Serial port for comms

  pinMode(led, OUTPUT);
}

void loop() {
  //Time Determination
  ledDeltaTime = millis() - lastExecutionTime ;
  commsDeltaTime = millis() - lastTransmission;
  long deltaTime = millis() - lastTime;

  //These are essential functions for flight that are executed regardless of flight state - for now they are just empty functions, 
  //but as more sensors/equipment get hooked up to the craft, they will be filled in
  //TODO: Add humidity
  float altitude = getAltitude();
  float pressure = getPressure();
  float temperature = getTemp();
  String gpsPosition = getPosition(); //Will this decleration come back to bite me? Yes. Yes it will

  float deltaAltitude = (altitude - lastAltitude)/(deltaTime/1000); //Unless I'm very bad at both math and programming, this should be our vertical velocity, in m/s
  
  //State machine
  if (state == "preLaunch") {
    if (Serial.readString() == "go") {
      state = "goForLaunch";
    }
    ledDelay = preDelay;
  } else if (state == "goForLaunch") {
    commsDelay = 500;
    if (altitude > 10) {
      state = "ascending";
    }
  } else if (state == "ascending") {
    if (altitude > 500) {
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
  transmit(commsDelay);
}

//Also going to be doing SD card saving here, as data points will be the same 
void transmit(long delayTime) {
  if (commsDeltaTime > delayTime) {
    //transmit
    lastTransmission = millis();
  }
}

void blinkStatus(long delayTime) {
  Serial.println(ledOn);
  if (ledDeltaTime >= delayTime) {
    if (ledOn == 0) {
      digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level
      ledOn = 1;
    } else {
      digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
      ledOn = 0;
    }
    lastExecutionTime = millis();
  }
}

float getAltitude() {
  
}

float getPressure() {
  
}

float getTemp() {
  
}

String getPosition() {
  
}

void drop() {
  
}
