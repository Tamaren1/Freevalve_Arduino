// This code is full of test strings and is generally a conglomerate of code from StackExchange and
// bad form. If it looks stupid, I probably wrote it.
// Wesley Kagan, 2020
// Ric Allinson, 2020

// Control variables.
const int MAGNET_PER_DEG = 6
const int FREEVALVE_ANGLE = 180;

// Pins assignment.
const int HALL_MAGNET = 2;
const int EXHAUST_V = 12;
const int INTAKE_V = 13;

int cad;                   // Crank Angle Degrees (CAD).
bool cycle;                // "true" for Intake, "false" for Exhaust.
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
    Serial.print(cad);
    printLog = false;
  }
}

void magnetDetect() {
  hallCounter++;
  timeGap = millis() - lastTimeGap;

  // Find missing tooth for dead top.
  if (timeGap >= lastTimeGap * 3 / 2) {
    hallCounter = 1;
    cycle = !cycle;
  }

  // Store the last time difference so we can use it in the next cycle.
  lastTimeGap = timeGap;

  
  cad = hallCounter * MAGNET_PER_DEG;

  // Every rotation of the crank alternates between Intake and Exhaust.
  if (cycle) {
    digitalWrite(INTAKE_V,  cad <= FREEVALVE_ANGLE); 
  } else {
    digitalWrite(EXHAUST_V, FREEVALVE_ANGLE <= cad);
  }
  printLog = true;
}
