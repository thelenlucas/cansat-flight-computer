String state; //Stores current state of craft

bool ledOn; //Bool for dynamic blinking of LED
float ledDeltaTime; //Time since last LED switch [ms]

float lastExecutionTime; //Time of last loop execution

long ledDelay = 2000;
long preDelay = 2000; //Different launch states led frequencies

int commsDelay = 1000; //Delay on xBee, so we don't flood the airwaves
long commsDeltaTime;
long lastTransmission;

bool buzzing = false;

long lastTime = 0;

float lastAltitude = 0;

void setup() {
  // put your setup code here, to run once:
  state = "preLaunch";
  ledDeltaTime = 0;
  lastExecutionTime = 0;
  lastTransmission = 0;
  Serial.begin(9600);
  
}

void loop() {
  //Time Determination
  ledDeltaTime = millis() - lastExecutionTime ;
  commsDeltaTime = millis() - lastTransmission;
  long deltaTime = millis() - lastTime;

  //Blink the LED
  blinkStatus(ledDelay);

  //These are essential functions for flight that are executed regardless of flight state - for now they are just empty functions, but as more sensors/equipment get hooked up to the craft, they will be filled in
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
}


void transmit(long delayTime) {
  if (commsDeltaTime > delayTime) {
    //transmit
    lastTransmission = millis();
  }
}

void blinkStatus(long delayTime) {
  if (ledDeltaTime > delayTime) {
    if (ledOn) {
      //Turn led off
    } else {
      //Turn led on
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
