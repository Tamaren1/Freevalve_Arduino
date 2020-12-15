// An Arduino program for controlling valve timing on a 6.5ph Predator engine from Harbor Freight.
//
// Wesley Kagan, 2020
// Ric Allinson, 2020

// Pins assignment.
const int HALL_MAGNET = 2;
const int EXHAUST_V = 12;
const int INTAKE_V = 13;

// Control constants.
const int DEG_PER_MAGNET = 6; // Number of degrees for per magnet.

// Runtime mapping variables.
bool intakeMap[120] = {};  // Intake open/close mapping from -360 to 360 divided by DEG_PER_MAGNET.
bool exhaustMap[120] = {}; // Exhaust open/close mapping from -360 to 360 divided by DEG_PER_MAGNET.

// ISR variables.
volatile int hallCounter;           // The number of magnets after the last TDC.
volatile bool secondRotation;       // "true" if the cam is on its second rotation.
volatile bool printLog;             // Used to stop duplicate values from printing in the loop.
volatile unsigned long timeGap;     // Function level time between interrupts.
volatile unsigned long lastTimeGap; // Global level time between interrupts.

void setup() {
  Serial.begin(115200);
  pinMode(HALL_MAGNET, INPUT);
  attachInterrupt(0, magnetDetect, RISING);
  pinMode(INTAKE_V, OUTPUT);
  pinMode(EXHAUST_V, OUTPUT);
  // Load mappings here.
  // intakeMap = ?
  // exhaustMap = ?
}

void loop() {
  // This may not print all values. Only the most recent from the last interrupt.
  if(printLog){
    Serial.print(lastTimeGap);
    Serial.print(hallCounter);
    Serial.print(secondRotation);
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
    // On the second rotation `or` if its value is greater than the mapping indexes reset the hallCounter.
    if (secondRotation || hallCounter >= 120) {
      hallCounter = 0;
    } else {
      hallCounter = 59; // Forcing the counter in case of drift over a rotation.
    }
    // Flip the secondRotation.
    secondRotation = !secondRotation;
  }

  // Store the last time difference so we can use it in the next interrupt.
  lastTimeGap = timeGap;

  // Use the intake/exhaust maps to open or close the valves.
  digitalWrite(INTAKE_V, intakeMap[hallCounter]);
  digitalWrite(EXHAUST_V, exhaustMap[hallCounter]);

  // Tells the loop() that values have changed.
  printLog = true;
}
