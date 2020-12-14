//This code is full of test strings and is generally a conglomerate of code from stackexchange and
//bad form. If it looks stupid, I probably wrote it.
//Wesley Kagan, 2020

const int hall = 2;
int hallcounter = 0;
int hallstate = 0;
int lasthallstate = 0;
unsigned long triggerTime;
unsigned long last_triggerTime;
unsigned long timeGap;
unsigned long last_timeGap;
unsigned int degree;
unsigned int state;
int newstate;

void setup() {
  pinMode(hall, INPUT);
  Serial.begin(115200);
  pinMode(13, OUTPUT);    // sets the digital pin 13 as output
  pinMode(12, OUTPUT);    // sets the digital pin 12 as output
  attachInterrupt(0, magnet_detect, RISING);//Initialize the intterrupt pin digital pin 2
  degree = 0;
}

void loop() {
  //The compiler says this is needed otherwise the govt. takes my cat
}

void magnet_detect() {
  hallcounter ++;
  triggerTime = millis();
  timeGap = triggerTime - last_triggerTime;
  last_triggerTime = triggerTime;
     
  if (timeGap >= last_timeGap + last_timeGap / 2) {
    //Serial.println(state);
    Serial.println("missing tooth");
    hallcounter = 1; 
    state++; //This right here is garbage and you know it.
  }
    
  last_timeGap = timeGap;
  //Serial.println(hallcounter);
    
  degree = hallcounter * 6;

  if ((state % 2) == 0) { //Bool wasn't working so this dumpster fire got started.
    Serial.print("STATE 1 ");
    Serial.println(degree);
    if (6 <= degree && degree <= 180) { //EDIT HERE INTAKE
      digitalWrite(13, HIGH); // sets the digital pin 13 on
    } else {
      digitalWrite(13, LOW);  // sets the digital pin 13 off
    }
  }

  if ((state % 2) == 1) {
    Serial.print("STATE 2 ");
    Serial.println(degree);
    if (180 <= degree && degree <= 354) { //EDIT HERE EXHAUST
      digitalWrite(12, HIGH); // sets the digital pin 13 on
    } else {
      digitalWrite(12, LOW);  // sets the digital pin 13 off
    }
  }

  lasthallstate = hallstate;
}
