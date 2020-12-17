#include <FeedBackServo.h>

#include "FeedBackServo.h"

#define FEEDBACK_PIN 2 // define feedback signal pin

int val_old = 0;
int rotate = 0;

FeedBackServo Servo = FeedBackServo(FEEDBACK_PIN);  // set feedback signal pin number

void setup() {
    Serial.begin(115200);  // serial communication start with 115200 bps
}

void loop() {              
    int IntValue = analogRead(A0); 
    int ExhValue = analogRead(A1); 
    Serial.print("          Intake:");
    Serial.print(IntValue);
    Serial.print("          Exhaust:");
    Serial.println(ExhValue);
    delay(1000);
}
