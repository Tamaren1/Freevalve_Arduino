// An Arduino program for controlling valve timing on a 6.5HP Predator engine from Harbor Freight.
//
// License goes here.
//
// Wesley Kagan, 2020
// Ric Allinson, 2020

// Pins assignment.
static const int HALL_MAGNET = 2;
static const int EXHAUST_V = 12;
static const int INTAKE_V = 13;

// Implementation constants (to be changed by implementer).
static const int NUM_STEPS = 60;
static const int NUM_MISSING_STEPS = 1;

// Control constants.
static const int NUM_TRIGGER_STEPS = NUM_STEPS - NUM_MISSING_STEPS; // Number of interrupt steps counted.
static const int STEPS_PER_ROTATION = NUM_TRIGGER_STEPS;            // Number of interrupts in a half cycle.
static const int STEPS_PER_CYCLE = NUM_TRIGGER_STEPS * 2;           // Number of interrupts in the full cycle.

// Runtime mapping variables.
bool intakeMap[STEPS_PER_CYCLE] = {};  // Intake open/close mapping is equal the total number of interrupts for two rotations.
bool exhaustMap[STEPS_PER_CYCLE] = {}; // Exhaust open/close mapping is equal the total number of interrupts for two rotations.

// ISR variables.
volatile int hallCounter = 0;           // The number of magnets after the last TDC.
volatile bool secondCycle = false;   // "true" if the cam is on its second rotation.
volatile bool printLog = false;         // Used to stop duplicate values from printing in the loop.
volatile unsigned long timeGap = 0;     // Function level time between interrupts.
volatile unsigned long lastTimeGap = 0; // Global level time between interrupts.

void setup() {
  Serial.begin(115200);
  pinMode(HALL_MAGNET, INPUT);
  attachInterrupt(0, magnetDetect, RISING);
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
    Serial.print(lastTimeGap);
    Serial.print(hallCounter);
    Serial.println(secondCycle);
    printLog = false;
  }
}

// This function will be called about every ~263.16usec at the maximum RPM of 3800.
// The missing tooth will come by roughly every 15.79 ms.
// (1 rev / 60 pulses) * (1 min / 3800 rev) * (60 sec / min) = 1 / 3800 sec/pulse = ~263.16usec.
void magnetDetect() {
  // Increment the counter to keep track of the position.
  hallCounter++;

  // Get the time gap between this interrupt and the last.
  // 
  // The following will be a problem...
  //
  // Returns the number of microseconds since the Arduino board began running the current program.
  // This number will overflow (go back to zero), after approximately 70 minutes.
  // On 16 MHz Arduino boards (e.g. Duemilanove and Nano),
  // this function has a resolution of four microseconds (i.e. the value returned is always a multiple of four).
  // On 8 MHz Arduino boards (e.g. the LilyPad), this function has a resolution of eight microseconds.
  timeGap = micros() - lastTimeGap;

  // Detect the missing step.
  if (timeGap * 10 >= lastTimeGap * 15) {
    if (secondCycle) {
      hallCounter = 0;                  // On the second cycle reset the hallCounter.
    } else {
      hallCounter = STEPS_PER_ROTATION; // Forcing the counter in case of drift over a rotation.
    }
    // Flip the secondCycle flag.
    secondCycle = !secondCycle;
  }

  // Store the last time difference so we can use it in the next interrupt.
  lastTimeGap = timeGap;

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
