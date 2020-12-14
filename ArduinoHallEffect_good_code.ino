//This code is full of test strings and is generally a conglomerate of code from stackexchange and
//bad form. If it looks stupid, I probably wrote it.
//Wesley Kagan, 2020

//this might be of interest https://forum.arduino.cc/index.php?topic=4324.0 fast pin switching at 2MHz on Uno

const int hall = 2;
const int INTAKE_V = 13;
const int EXHAUST_V = 12;
int hallcounter = 0;
unsigned long timeGap;
unsigned long last_timeGap;
int degree;
int cycle = 1; //1 is intake cycle, -1 is exhaust, change initial cycle here

void setup() {
   pinMode(hall, INPUT);
   Serial.begin(115200);
   pinMode(INTAKE_V, OUTPUT);                 // sets the digital pin 13 as output
   pinMode(EXHAUST_V, OUTPUT);                // sets the digital pin 12 as output
   attachInterrupt(0, magnet_detect, RISING); //Initialize the intterrupt pin digital pin 2
   degree = 0;
}

void loop() {
   //The compiler says this is needed otherwise the govt. takes my cat
}

void magnet_detect() {
   hallcounter++;
   timeGap = millis() - timeGap;

   if (timeGap >= (int)(1.5 * last_timeGap)) {
      //Serial.println(state);
      Serial.println("missing tooth");
      hallcounter = 1;
      cycle = -1 * cycle; 
   }
   last_timeGap = timeGap;

   //Serial.println(hallcounter);
   degree = hallcounter * 6;
   if (cycle > 0) { //Bool wasn't working so this dumpster fire got started.
      Serial.print("STATE 1 ");
      if (degree <= 180) {                                //EDIT HERE INTAKE
         digitalWrite(INTAKE_V, HIGH); // sets the digital pin 13 on
      }
      else {
         digitalWrite(INTAKE_V, LOW); // sets the digital pin 13 off
      }
   }
   else {
      Serial.print("STATE 2 ");
      if (180 <= degree) {                                 //EDIT HERE EXHAUST
         digitalWrite(EXHAUST_V, HIGH); // sets the digital pin 12 on
      }
      else {
         digitalWrite(EXHAUST_V, LOW); // sets the digital pin 12 off
      }
   }
   Serial.println(degree);
}