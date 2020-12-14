// This code is full of test strings and is generally a conglomerate of code from StackExchange and
// bad form. If it looks stupid, I probably wrote it.
// Wesley Kagan, 2020
// Ric Allinson, 2020

// Control variables.
const int MAGNET_PER_DEG = 6;
const int FREEVALVE_ANGLE = 180;

// Pins assignment.
const int HALL_MAGNET = 2;
const int EXHAUST_V = 12;
const int INTAKE_V = 13;

int cad;                   // Crank Angle Degrees (CAD).
int hallCounter;           // The number of magnets after the last TDC.
bool cycle;                // "true" for Intake, "false" for Exhaust.
bool printLog;             // Used to trigger a print in the loop.
unsigned long timeGap;     // Function level time between interrupts.
unsigned long lastTimeGap; // Global level time between interrupts.

void setup() {
  Serial.begin(115200);
  pinMode(HALL_MAGNET, INPUT);
  attachInterrupt(0, magnetDetect, RISING);
  pinMode(INTAKE_V, OUTPUT);
  pinMode(EXHAUST_V, OUTPUT);
}

void loop() {
  if(printLog){
    Serial.print(lastTimeGap);
    Serial.print(cycle);
    Serial.print(cad);
    printLog = false;
  }
}

void magnetDetect() {
  // Increment the counter to keep track of the position.
  hallCounter++;
  // Get the time gap between this interrupt and the last.
  timeGap = millis() - lastTimeGap;

  // Find missing tooth for Top Dead Center (TDC).
  if (timeGap >= lastTimeGap * 3 / 2) {
    // Reset the hall counter.
    hallCounter = 1;
    // Flip the cycle phase.
    cycle = !cycle;
  }

  // Store the last time difference so we can use it in the next interrupt.
  lastTimeGap = timeGap;

  // Store the current crank angle for logging.
  cad = hallCounter * MAGNET_PER_DEG;

  // Every rotation of the crank alternates between Intake and Exhaust.
  if (cycle) {
    // If the crank angle degree is in the intake range open it, otherwise close it.
    digitalWrite(INTAKE_V,  cad <= FREEVALVE_ANGLE); 
  } else {
    // If the crank angle degree is in the exhaust range open it, otherwise close it.
    digitalWrite(EXHAUST_V, FREEVALVE_ANGLE <= cad);
  }
  printLog = true;
}
