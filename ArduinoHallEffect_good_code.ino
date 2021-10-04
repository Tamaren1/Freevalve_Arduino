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
const int intDesiredOpen = 6;
const int intDesiredClose = 180;
const int exhDesiredOpen = 180;
const int exhDesiredClose = 354;
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
    
     int degree = (int) (hallcounter * degreeModifier); //more accurate degree calculation
     int crankSectiondegree = degree % cylinderDegrees; //equates to degress in the section of the crankshaft for comparisons on timing event math. 
     int crankSection = degree/cylinderDegrees; // crank sectioned by number of cylinders.
     int int1Pin = intFirstPin + crankSection; //output pin for phase1 intake valve - based on crank section and next cylinder in firing order
     int int2Pin = intFirstPin + crankSection + cylindersPerRotation;  //output pin for phase2 intake valve
     int exh1Pin = exhFirstPin + crankSection; //output pin for phase1 exhaust valve
     int exh2Pin = exhFirstPin + crankSection + cylindersPerRotation; //output pin for phase2 exhaust valve
     if ((phase % 2) == 0) { //crankshaft degrees in first phase.    
       //Here we need to focus on valve state change.  less efficient to do a digitalwrite each tooth we check.  so we need to look at current valve state and desired state
        //desired open - this will not be able to be exact, as it's to the nearest hall effect node for degree.  We determine if the degree is near the desired event, and activate the event if so.
        //Much of the math here is irrelevant to single cylinder engines.  However, this will still work with single cylinder.
       
       //intakeOpen  
       if (crankSectiondegree <= intDesiredOpen + degreeModifier/2 and crankSectiondegree >= intDesiredOpen - degreeModifier/2)
       {
         OpenValve(int1Pin);
       }
       
       //intakeClose
       if (crankSectiondegree <= intDesiredClose + degreeModifier/2 and crankSectiondegree >= intDesiredClose - degreeModifier/2)
       {
         CloseValve(int1Pin);
       }
       
       //In multicylinder engines, half the cylinders will have exhaust events in phase 1
       if(cylinders > 1) {
         //exhaustOpen  
         if (crankSectiondegree <= exhDesiredOpen + degreeModifier/2 and crankSectiondegree >= exhDesiredOpen - degreeModifier/2)
         {
           OpenValve(exh2Pin);
         }

         //exhaustClose
         if (crankSectiondegree <= exhDesiredClose + degreeModifier/2 and crankSectiondegree >= exhDesiredClose - degreeModifier/2)
         {
           CloseValve(exh2Pin);
         }
       }
     }
     if ((phase % 2) == 1) { //crankshaft degrees in 2nd phase
       //exhaustOpen  
       if (crankSectiondegree <= exhDesiredOpen + degreeModifier/2 and crankSectiondegree >= exhDesiredOpen - degreeModifier/2)
       {
         OpenValve(exh1Pin);
       }
       
       //exhaustClose
       if (crankSectiondegree <= ehxDesiredClose + degreeModifier/2 and crankSectiondegree >= exhDesiredClose - degreeModifier/2)
       {
         CloseValve(exh1Pin);
       }
       
       //In multicylinder engines, half the cylinders will have intake events in phase 2
       if(cylinders > 1) {
         //intakeOpen  
         if (crankSectiondegree <= intDesiredOpen + degreeModifier/2 and crankSectiondegree >= intDesiredOpen - degreeModifier/2)
         {
           OpenValve(int2Pin);
         }

         //intakeClose
         if (crankSectiondegree <= intDesiredClose + degreeModifier/2 and crankSectiondegree >= intDesiredClose - degreeModifier/2)
         {
           CloseValve(int2Pin);
         }
       }
     }
}
