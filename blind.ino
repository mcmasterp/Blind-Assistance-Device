/* HC-SR04 Sensor
   https://www.dealextreme.com/p/hc-sr04-ultrasonic-sensor-distance-measuring-module-133696
  
   This sketch reads a HC-SR04 ultrasonic rangefinder and returns the
   distance to the closest object in range. To do this, it sends a pulse
   to the sensor to initiate a reading, then listens for a pulse 
   to return.  The length of the returning pulse is proportional to 
   the distance of the object from the sensor.
     
   The circuit:
  * VCC connection of the sensor attached to +5V
  * GND connection of the sensor attached to ground
  * TRIG connection of the sensor attached to digital pin 2
  * ECHO connection of the sensor attached to digital pin 4
   Original code for Ping))) example was created by David A. Mellis
   Adapted for HC-SR04 by Tautvidas Sipavicius
   This example code is in the public domain.
 */


const int trigPin = 5;
const int echoPin = 7;
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
int rangeVal;
#define filterSamples   3              // filterSamples should  be an odd number, no smaller than 3
int sensSmoothArray1 [filterSamples];   // array for holding raw sensor values for sensor
int smoothData1;  // variables for smoothing
int finishedData;

void setup() {
  // initialize serial communication:
  Serial.begin(115200);
  pinMode(echoPin, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(sonarPwrPin, OUTPUT);
  digitalWrite(sonarPwrPin, HIGH);
  pinMode(rangeButtonPin, INPUT);
  pinMode(yPin, INPUT);
  pinMode(rightVibePin, OUTPUT);
  pinMode(leftVibePin, OUTPUT);
}

void loop()
{
  // establish variables for duration of the ping, 
  // and the distance result in inches and centimeters:
  long duration, inches, cm, pulsePWM, pulsePWMVal;  

  // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read the signal from the sensor: a HIGH pulse whose
  // duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  
  duration = pulseIn(echoPin, HIGH);

  // convert the time into a distance
  inches = microsecondsToInches(duration);
  cm = microsecondsToCentimeters(duration);
  pulsePWM = microsecondsToPWM(duration);


  pulsePWMVal = constrain(pulsePWM, 1, 255); //keep pulse pwm to within range
  //smoothData1 = digitalSmooth(pulsePWMVal, sensSmoothArray1); 
  finishedData = 255 - pulsePWMVal;
  //finishedData = 255 - smoothData1;


rangeButtonState = digitalRead(rangeButtonPin);
  if (rangeButtonState == HIGH) {
    rangeVal = 10;
  } else {
    rangeVal = 20;
  }


yState = analogRead(yPin) / 2;
//smoothData1 = digitalSmooth(yState, sensSmoothArray1);
if (yState < 173) {
  rightVibe = false; 
} else {
  rightVibe = true;
}

if (yState > 191) {
  leftVibe = false; 
} else {
  leftVibe = true;
}



   // Serial.print(inches);
   // Serial.print("in, ");
   // Serial.print(smoothData1);
   // Serial.print("Y Axis  ");
   // Serial.print(rangeVal);
   // Serial.print(" modifier  ");

    
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
    delay(5);


}

long microsecondsToInches(long microseconds)
{
  // According to Parallax's datasheet for the PING))), there are
  // 73.746 microseconds per inch (i.e. sound travels at 1130 feet per
  // second).  This gives the distance travelled by the ping, outbound
  // and return, so we divide by 2 to get the distance of the obstacle.
  // See: http://www.parallax.com/dl/docs/prod/acc/28015-PING-v1.3.pdf
  return microseconds / 74 / 2;
}

long microsecondsToPWM(long microseconds)
{
  // To convert the distance into pulses for vibration
  return microseconds / 2 / rangeVal;
}

long microsecondsToCentimeters(long microseconds)
{
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled.
  return microseconds / 29 / 2;
}

int digitalSmooth(int rawIn, int *sensSmoothArray){     // "int *sensSmoothArray" passes an array to the function - the asterisk indicates the array name is a pointer
  int j, k, temp, top, bottom;
  long total;
  static int i;
 // static int raw[filterSamples];
  static int sorted[filterSamples];
  boolean done;

  i = (i + 1) % filterSamples;    // increment counter and roll over if necc. -  % (modulo operator) rolls over variable
  sensSmoothArray[i] = rawIn;                 // input new data into the oldest slot

  // Serial.print("raw = ");

  for (j=0; j<filterSamples; j++){     // transfer data array into anther array for sorting and averaging
    sorted[j] = sensSmoothArray[j];
  }

  done = 0;                // flag to know when we're done sorting              
  while(done != 1){        // simple swap sort, sorts numbers from lowest to highest
    done = 1;
    for (j = 0; j < (filterSamples - 1); j++){
      if (sorted[j] > sorted[j + 1]){     // numbers are out of order - swap
        temp = sorted[j + 1];
        sorted [j+1] =  sorted[j] ;
        sorted [j] = temp;
        done = 0;
      }
    }
  }

/*
  for (j = 0; j < (filterSamples); j++){    // print the array to debug
    Serial.print(sorted[j]); 
    Serial.print("   "); 
  }
  Serial.println();
*/

  // throw out top and bottom 15% of samples - limit to throw out at least one from top and bottom
  bottom = max(((filterSamples * 15)  / 100), 1); 
  top = min((((filterSamples * 85) / 100) + 1  ), (filterSamples - 1));   // the + 1 is to make up for asymmetry caused by integer rounding
  k = 0;
  total = 0;
  for ( j = bottom; j< top; j++){
    total += sorted[j];  // total remaining indices
    k++; 
    // Serial.print(sorted[j]); 
    // Serial.print("   "); 
  }

//  Serial.println();
//  Serial.print("average = ");
//  Serial.println(total/k);
  return total / k;    // divide by number of samples
}
