//Arduio hall effect.  Valve timing based on a reluctor wheel.  Adjustable cylinders and reluctor wheel
//This is build currently for waste spark systems.  you would need to be able to calculate ignition position to match #1 firing with valve events.
//some of this code may be too expensive math wise and slow, will have to evaluate
//
//Currently still only enabling single valve.  working on math for multivalve with 
//Started by Wesley Kagan, 2020
//Updated by Jason Brandt 20211003

const int hallPin = 2;
const int teeth = 58; //reluctor wheel teeth
const float degreeModifier = 360/teeth; //Used to calculate degree location of each tooth for more accurate timing
const int cylinders = 8;
const int cylindersPerRotation = cylinders/2; //basically, number of cylinders in stroke 1/2 this state.  typically half, even split between cylinders/states; distributor based engines firing is typically evenly spaced over 2 crankshaft rotations
const int cylinderDegrees = 360/cylindersPerRotation; //Used for valve timing math on multiple cylinder engines.  
const int IVO = 31;  //Intake Valve Opening BTDC
const int IVC = 70; //Intake Valve Closing ABDC
const int EVO = 82; //Exhaust Valve Opening BBDC
const int EVC = 29; //Exhaust Valve Closing ATDC
const int intFirstPin = 12;
const int exhFirstPin = 20;
//const int[] firingOrder = {1,8,7,2,6,5,4,3}  //This is not actually needed.  simply wire the valve pins by firing order as described below
int hallcounter = 0;
unsigned long triggerTime;
unsigned long last_triggerTime;
unsigned long timeGap;
unsigned long last_timeGap;
unsigned int phase; //Phase of the engine is either stroke 1/2 or 3/4 on #1.  Everything else is relative to that.  


void setup() {
  ///!!! Pins Follow your Firing order!  So if you start at 12 int with the above firing order, int 1 -> 12, 8 -> 13, 7 -> 14, etc.  exh 1-> 20, 8 -> 21, etc.  
  for(int i=0;i<cylinders;i++) { 
    //Set Intake pins to output and zero
    int Pin=intFirstPin+i; 
    pinMode((Pin), OUTPUT); 
    digitalWrite(Pin,LOW); 
    
    //Set Exhaust pins to output and zero
    int Pin=exhFirstPin+i; 
    pinMode((Pin), OUTPUT); 
    digitalWrite(Pin,LOW); 
  } 
 
  pinMode(hallPin, INPUT);
  attachInterrupt(0, magnet_detect, RISING);//Initialize the intterrupt pin digital pin 2 for the hall sensor
}

 void loop()
 {
  //The compiler says this is needed otherwise the govt. takes my cat
   }

void open_valve(int Pin) {
  digitalWrite(Pin, HIGH);
}

void close_valve(int Pin) {
  digitalWrite(Pin, LOW);
}

void magnet_detect() {
      hallcounter ++;
      triggerTime = millis();
      timeGap = triggerTime - last_triggerTime;
      last_triggerTime = triggerTime;
     
      if (timeGap >= last_timeGap + last_timeGap / 2) // detect blank (open) tooth
      {
        hallcounter = 0; //This needs to be 0 to get proper timing.  if another tooth is actually 0, then you need to rotate the order at 58 back to 0 elsewhere and mark this tooth at proper location (ie hallcounter=4, etc).
        phase = (phase+1) % 2; //Alternate between phases (stroke 1/2, 3/4) each revolution.
      }
    
      last_timeGap = timeGap;
    
    //determine crankshaft position based on hall counter
     int degree = (int) (hallcounter * degreeModifier); //more accurate degree calculation

  //check valve events for each pair of sync'd cylinders (or 
  for (int i=0;i<cylindersPerRotation || i=0;i++) {
    //Calculate Degrees for timing checks
    int sectionDegree = degree - (i * cylinderDegrees); } //base degree for current section - current degree + firing cylinder number * degrees to zero each cylinder (2 cyl =360, 4=180, 6=120, 8=90)
    if (sectionDegree < 0) { sectionDegree = 360 + sectionDegree; } //if calculated degree is - (before 0), reset degree to correct position in the circle
    int ATDC = sectionDegree; //Calcaute current position relative to TDC.  ATDC counts up from TDC.  As such, this will equal our degree.
    if (sectionDegree > 180) { ATDC = sectionDegree-360; } //Allow for Negative ATDC values (before TDC) - if degrees > 180, display as negative
    int BTDC = -(ATDC); //Calculate current position relative to TDC.  BTDC counts down to TDC. BTDC is opposite of ATDC
    int BBDC = 180-sectionDegree; //Calculate current position relative to BDC.  BBDC counts down to BDC. This value will naturally go negative after passing 180 degrees
    int ABDC = -(BBDC); //Calculate current position relative to BDC.  ABDC counts up from BDC.  ABDC is opposite of BBDC
    
    //Output Pin Definitions
    int int1Pin = intFirstPin + i; //output pin for phase1 intake valve - based on crank section and next cylinder in firing order
    int int2Pin = intFirstPin + i + cylindersPerRotation;  //output pin for phase2 intake valve
    int exh1Pin = exhFirstPin + i; //output pin for phase1 exhaust valve
    int exh2Pin = exhFirstPin + i + cylindersPerRotation; //output pin for phase2 exhaust valve
  
     if ((phase % 2) == 0) { //crankshaft in phase 1 - #1 just fired - Events in phase 1 are EVO, IVO for current "first" group, EVC, IVC for "second" group (multicylinder)    
       //Here we need to focus on valve state change.  less efficient to do a digitalwrite each tooth we check.  so we need to look at current valve state and desired state
        //desired open - this will not be able to be exact, as it's to the nearest hall effect node for degree.  We determine if the degree is near the desired event, and activate the event if so.
        //Much of the math here is irrelevant to single cylinder engines.  However, this will still work with single cylinder.
       
       //exhaust valve open BBDC  
       if (BBDC <= EVO + degreeModifier/2 and BBDC >= EVO - degreeModifier/2)
       {
         OpenValve(exh1Pin);
       }
       
       //intake valve open BTDC  
       if (BTDC <= IVO + degreeModifier/2 and BTDC >= IVO - degreeModifier/2)
       {
         OpenValve(int1Pin);
       }

       //In multicylinder engines, half the cylinders will have intake events in phase 2
       if(cylinders > 1) {
         //exhaust valve close ATDC
         if (ATDC <= EVC + degreeModifier/2 and ATDC >= EVC - degreeModifier/2)
         {
           CloseValve(exh2Pin);
         }
         
         //intake valve close ABDC
         if (ABDC <= IVC + degreeModifier/2 and ABDC >= IVC - degreeModifier/2)
         {
           CloseValve(int2Pin);
         }
       }
     }
     if ((phase % 2) == 1) { //crankshaft degrees in 2nd phase
       //exhaust valve close ATDC
       if (ATDC <= EVC + degreeModifier/2 and ATDC >= EVC - degreeModifier/2)
       {
         CloseValve(exh1Pin);
       }
       
       //intake valve close ABDC
       if (ABDC <= IVC + degreeModifier/2 and ABDC >= IVC - degreeModifier/2)
       {
         CloseValve(int1Pin);
       }
       
       //In multicylinder engines, half the cylinders will have exhaust events in phase 1
       if(cylinders > 1) {
         //exhaust valve open BBDC
         if (BBDC <= EVO + degreeModifier/2 and BBDC >= EVO - degreeModifier/2)
         {
           OpenValve(exh2Pin);
         }
         
         //intake valve open BTDC
         if (BTDC <= IVO + degreeModifier/2 and BTDC >= IVO - degreeModifier/2)
         {
           OpenValve(int2Pin);
         }
       }
     }
  }
}
