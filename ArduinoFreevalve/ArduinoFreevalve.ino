// An Arduino program for controlling valve timing on a 6.5ph Predator engine from Harbor Freight.
//
// Wesley Kagan, 2020
// Ric Allinson, 2020

// Pins assignment.
const int HALL_MAGNET = 2;
const int EXHAUST_V = 12;
const int INTAKE_V = 13;

// Control variables.
const int DEG_PER_MAGNET = 6;    // Number of degrees for each magnet.
const bool INTAKE_MAP[120] = {};  // Intake open/close mapping from -360 to 360.
const bool EXHAUST_MAP[120] = {}; // Exhaust open/close mapping from -360 to 360.

int hallCounter;           // The number of magnets after the last TDC.
bool cycle;                // "true" for Intake, "false" for Exhaust.
bool printLog;             // Used to stop duplicate values from printing in the loop.
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
    Serial.print(hallCounter);
    Serial.print(cycle);
    printLog = false;
  }
}

// This function will be called about every 15.79ms at the maximum RPM of 3800.
void magnetDetect() {
  // Increment the counter to keep track of the position.
  hallCounter++;
  // Get the time gap between this interrupt and the last.
  timeGap = millis() - lastTimeGap;

  // Find the missing tooth for Top Dead Center (TDC).
  if (timeGap >= lastTimeGap * 3 / 2) {
    // On the second rotation reset the hall counter.
    if (cycle) {
      hallCounter = 0;
    }
    // Flip the cycle phase.
    cycle = !cycle;
  }

  // Store the last time difference so we can use it in the next interrupt.
  lastTimeGap = timeGap;

  // Use the intake/exhaust maps to open or close the valves.
  digitalWrite(INTAKE_V, INTAKE_MAP[hallCounter]);
  digitalWrite(EXHAUST_V, EXHAUST_MAP[hallCounter]);

  printLog = true;
}
