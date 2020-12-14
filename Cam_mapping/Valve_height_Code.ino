//For use with the valve position tool, copy and paste Serial out to Excel for graph purposes.
//Wesley Kagan, 2020
//www.youtube.com/c/wesleykagan for the video

#include <FeedBackServo.h>

#include "FeedBackServo.h"

#define FEEDBACK_PIN 2 // define feedback signal pin

int val_old = 0;
int rotate = 0;
int intake_height = 0;
int exhaust_height = 0;


//Calibration: Values: adjust these. Use Calibration program.
int Intake_lower = 995;
int Intake_higher = 885;
int Exhaust_lower = 279;
int Exhaust_higher = 384;
//End calibration.


FeedBackServo Servo = FeedBackServo(FEEDBACK_PIN);  // set feedback signal pin number

void setup() {
    Serial.begin(115200);  // serial communication start with 115200 bps
    Serial.print("Angle   Intake    Exhaust");
}

void loop() {
  rotate = Servo.Angle();             
  if (rotate > val_old) {
    int IntValue = analogRead(A0); // read the input on analog pin 0:
    int ExhValue = analogRead(A1); // read the input on analog pin 1:
    float intake_height = map(IntValue, Intake_lower, Intake_higher, 0, 1270);
    float exhaust_height = map(ExhValue, Exhaust_lower, Exhaust_higher, 0, 1270);
    abs(rotate);
    Serial.print(abs(rotate)); //Angle
    Serial.print("          ");
    Serial.print(intake_height/100); //Intake 
    Serial.print("          ");
    Serial.println(exhaust_height/100); //Exhaust
    delay(10);
    
  }
  val_old = rotate;
}
