// An Arduino program for controlling valve timing on a 6.5HP Predator engine from Harbor Freight.
//
// License goes here?
//
// Wesley Kagan, 2020
// Ric Allinson, 2020

// Pins assignment.
const int HALL_MAGNET = 2;
const int EXHAUST_V = 12;
const int INTAKE_V = 13;

// Control constants.
const int DEG_PER_MAGNET = 6; // Number of degrees for per magnet.
const int ROTATION_TWO = 760/DEG_PER_MAGNET;     // Number of interrupts in the full cycle.
const int ROTATION_ONE = (760/DEG_PER_MAGNET)-1; // Number of interrupts in a half cycle.

// Runtime mapping variables.
bool intakeMap[ROTATION_TWO] = {};  // Intake open/close mapping is equal the total number of interrupts for two rotations.
bool exhaustMap[ROTATION_TWO] = {}; // Exhaust open/close mapping is equal the total number of interrupts for two rotations.

// ISR variables.
volatile int hallCounter = 0;           // The number of magnets after the last TDC.
volatile bool secondRotation = false;   // "true" if the cam is on its second rotation.
volatile bool printLog = false;         // Used to stop duplicate values from printing in the loop.
volatile unsigned long timeGap = 0;     // Function level time between interrupts.
volatile unsigned long lastTimeGap = 0; // Global level time between interrupts.

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
    // On the second rotation reset the hallCounter.
    if (secondRotation) {
      hallCounter = 0;
    } else {
      hallCounter = ROTATION_ONE; // Forcing the counter in case of drift over a rotation.
    }
    // Flip the secondRotation.
    secondRotation = !secondRotation;
  }

  // Store the last time difference so we can use it in the next interrupt.
  lastTimeGap = timeGap;

  // If the hallCounter is greater than the mapping index reset it.
  // This would only happen when the missing tooth was NOT detected.
  if (hallCounter >= ROTATION_TWO) {
    hallCounter = 0;
  }

  // Use the intake/exhaust maps to open or close the valves.
  digitalWrite(INTAKE_V, intakeMap[hallCounter]);
  digitalWrite(EXHAUST_V, exhaustMap[hallCounter]);

  // Tells the loop() that values have changed.
  printLog = true;
}
