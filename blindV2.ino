/* Sonic Cane
  Just some dumb screwing around from mcmasterp. free to use if your broke like me! If you're not, have a heart. paypal is modmanpaul@gmail.com
 */
#include <NewPing.h>
#define TRIGGER_PIN  5
#define ECHO_PIN     7
#define MAX_DISTANCE 200
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
const int yPin = A7;     // the number of the y axis data pin
int yState;
boolean rightVibe = true;
boolean leftVibe = true;
int rightVibeData, leftVibeData;
const int rightVibePin = 11;
const int leftVibePin = 10;
const int rangeButtonPin = 9;     // the number of the pushbutton pin
const int sonarPwrPin = 8;
int rangeButtonState = 0;
int rangeVal, pulsePWM, pulsePWMVal, pings, finishedData;

void setup() {
  // initialize serial communication:
  Serial.begin(115200);
  pinMode(sonarPwrPin, OUTPUT);
  digitalWrite(sonarPwrPin, HIGH);
  pinMode(rangeButtonPin, INPUT);
  pinMode(yPin, INPUT);
  pinMode(rightVibePin, OUTPUT);
  pinMode(leftVibePin, OUTPUT);
}

void loop()
{
  
  pings = sonar.ping_median(3);
  pulsePWM = microsecondsToPWM(pings);
  pulsePWMVal = 255 - pulsePWM;
  finishedData = constrain(pulsePWMVal, 1, 255); //keep pulse pwm to within range

rangeButtonState = digitalRead(rangeButtonPin);
  if (rangeButtonState == HIGH) {
    rangeVal = 10;
  } else {
    rangeVal = 19.6;
  }


yState = analogRead(yPin) / 2;
if (yState < 166) {
  rightVibe = false; 
} else {
  rightVibe = true;
}

if (yState > 202) {
  leftVibe = false; 
} else {
  leftVibe = true;
}

    //Serial.print(rangeVal);
    //Serial.print(" modifier  ");
    //Serial.print(yState);
    //Serial.print(" Y  ");
    //Serial.print(pings);
    //Serial.print(" pings  ");
    //Serial.print(pulsePWM);
    //Serial.print(" PWM  ");
    //Serial.print(pulsePWMVal);
    //Serial.print(" PWM  ");

    
 if (rightVibe > 0) {
    rightVibeData = finishedData;
} 
 else {
    rightVibeData = finishedData / 4;
}
 if (leftVibe > 0) {
    leftVibeData = finishedData;
} 
 else {
    leftVibeData = finishedData / 4;
}

    analogWrite(leftVibePin, leftVibeData);
    analogWrite(rightVibePin, rightVibeData);
    //Serial.print("R:");
    //Serial.print(rightVibeData);
    //Serial.print(" L:");
    //Serial.println(leftVibeData);
    delay(1);


}


long microsecondsToPWM(long microseconds)
{
  // To convert the distance into pulses for vibration
  return microseconds / 2 / rangeVal;
}

