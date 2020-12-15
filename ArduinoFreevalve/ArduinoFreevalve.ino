// An Arduino program for controlling valve timing on a 6.5ph Predator engine from Harbor Freight.
//
// Wesley Kagan, 2020
// Ric Allinson, 2020

// Control variables.
const int DEG_PER_MAGNET = 6;       // Number of degrees for each magnet.
const int FREEVALVE_OFFSET_TOP = 0; // The amount of off set from TDC (needs to be multiples of DEG_PER_MAGNET).
const int FREEVALVE_OFFSET_BOTTOM = 180 + FREEVALVE_OFFSET_TOP; 

// Pins assignment.
const int HALL_MAGNET = 2;
const int EXHAUST_V = 12;
const int INTAKE_V = 13;

int cad;                   // Crank Angle Degrees (CAD).
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
    Serial.print(cycle);
    Serial.print(cad);
    printLog = false;
  }
}

// This function will be called about every 15.79ms at the maximum RPM of 3800.
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
  cad = hallCounter * DEG_PER_MAGNET;

  // Every rotation of the crank alternates between Intake and Exhaust.
  if (cycle) {
    // If the crank angle degree is in the intake range open it, otherwise close it.
    digitalWrite(INTAKE_V, cad > FREEVALVE_OFFSET_TOP && cad <= FREEVALVE_OFFSET_BOTTOM);
  } else {
    // If the crank angle degree is in the exhaust range open it, otherwise close it.
    digitalWrite(EXHAUST_V, cad >= FREEVALVE_OFFSET_BOTTOM || cad < FREEVALVE_OFFSET_TOP);
  }
  printLog = true;
}
