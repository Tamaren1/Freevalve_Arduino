// An Arduino program for controlling valve timing on a 6.5HP Predator engine from Harbor Freight.
//
// License goes here.
//
// Wesley Kagan, 2020
// Ric Allinson, 2020

// Pins assignment.
static const int HALL_MAGNET = 2;
static const int ROTATION_MAGNET = 3;
static const int EXHAUST_V = 12;
static const int INTAKE_V = 13;

// Implementation constants (to be changed by implementer).
static const int NUM_STEPS = 60;

// Control constants.
static const int STEPS_PER_ROTATION = NUM_STEPS;            // Number of interrupts in a half cycle.
static const int STEPS_PER_CYCLE = NUM_STEPS * 2;           // Number of interrupts in the full cycle.

// Runtime mapping variables.
bool intakeMap[STEPS_PER_CYCLE] = {};  // Intake open/close mapping is equal the total number of interrupts for two rotations.
bool exhaustMap[STEPS_PER_CYCLE] = {}; // Exhaust open/close mapping is equal the total number of interrupts for two rotations.

// ISR variables.
volatile int hallCounter = 0;           // The number of magnets after the last TDC.
volatile bool secondCycle = false;   // "true" if the cam is on its second rotation.
volatile bool printLog = false;         // Used to stop duplicate values from printing in the loop.

void setup() {
  Serial.begin(115200);
  pinMode(HALL_MAGNET, INPUT);
  attachInterrupt(0, rotationDetect, RISING);
  attachInterrupt(1, magnetDetect, RISING);
  pinMode(INTAKE_V, OUTPUT);
  pinMode(EXHAUST_V, OUTPUT);
  /* 
    Load mappings here.
  */
  // intakeMap = ?
  // exhaustMap = ?
}

void loop() {
  // This may not print all values. Only the most recent from the last interrupt.
  if(printLog){
    Serial.print(hallCounter);
    Serial.println(secondCycle);
    printLog = false;
  }
}

// This function is called once per rotation.
void rotationDetect() {
  if (secondCycle) {
    hallCounter = 0;                  // On the second cycle reset the hallCounter.
  } else {
    hallCounter = STEPS_PER_ROTATION; // Forcing the counter in case of drift over a rotation.
  }
  // Flip the secondCycle flag.
  secondCycle = !secondCycle;
}

// This function is called NUM_STEPS per rotation.
// This function will be called about every ~263.16usec at the maximum RPM of 3800.
// (1 rev / 60 pulses) * (1 min / 3800 rev) * (60 sec / min) = 1 / 3800 sec/pulse = ~263.16usec.
void magnetDetect() {
  // Increment the counter to keep track of the position.
  hallCounter++;

  // If the hallCounter is greater than the mapping index reset it.
  // This would only happen when the missing tooth was NOT detected.
  if (hallCounter >= STEPS_PER_CYCLE) {
    hallCounter = 0;
  }

  // Use the intake/exhaust maps to open or close the valves.
  digitalWrite(INTAKE_V, intakeMap[hallCounter]);
  digitalWrite(EXHAUST_V, exhaustMap[hallCounter]);

  // Tells the loop() that values have changed.
  printLog = true;
}
