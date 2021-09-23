#include <Servo.h>
#include <DMXSerial.h>

int pot1 = 512;
int pot2 = 512;
int pot3 = 512;
int pot4 = 512;
int pot5 = 512;
int pot6 = 512;
int pot7 = 512;
int pot8 = 512;
int pot9 = 512;
int pot10 = 512;
int pot11 = 512;

float pot1Scaled;
float pot2Scaled;
float pot3Scaled;
float pot4Scaled;
float pot4aScaled;
float pot5Scaled;
float pot6Scaled;

float pot8Scaled;
float pot9Scaled;
float pot10Scaled;
float pot11Scaled;

float pot1Smoothed = 512;
float pot2Smoothed = 512;
float pot3Smoothed = 512;
float pot4Smoothed = 512;
float pot4aSmoothed = 512;
float pot5Smoothed = 512;
float pot6Smoothed = 512;

float pot8Smoothed = 512;
float pot9Smoothed = 512;
float pot10Smoothed = 512;
float pot11Smoothed = 512;

float pot1SmoothedPrev = 512;
float pot2SmoothedPrev = 512;
float pot3SmoothedPrev = 512;
float pot4SmoothedPrev = 512;
float pot4aSmoothedPrev = 512;
float pot5SmoothedPrev = 512;
float pot6SmoothedPrev = 512;

float pot8SmoothedPrev = 512;
float pot9SmoothedPrev = 512;
float pot10SmoothedPrev = 512;
float pot11SmoothedPrev = 512;

unsigned long currentMillis;
long previousMillis = 0;    // set up timers
long interval = 5;        // time constant for timer

int stepFlag = 0;
long previousStepMillis = 0;

// head
Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;
Servo servo5;
Servo servo6;
Servo servo7;

// arms
Servo servo8;
Servo servo9;
Servo servo10;
Servo servo11;

void setup() {

  DMXSerial.init(DMXReceiver);

  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(A4, INPUT);
  pinMode(A5, INPUT);

  Serial.begin(115200);

  // head
  servo1.attach(22);
  servo2.attach(24);
  servo3.attach(26);  
  servo4.attach(28);
  servo5.attach(30);
  servo6.attach(32);
  servo7.attach(34);

  // arms
  servo8.attach(36);    // right shoulder
  servo9.attach(38);    // right elbow
  servo10.attach(40);   // left shoulder
  servo11.attach(42);   // left elbow

  // head
  servo1.writeMicroseconds(1200);
  servo2.writeMicroseconds(1650);
  servo3.writeMicroseconds(1650);  
  servo4.writeMicroseconds(1500);
  servo5.writeMicroseconds(1550);
  servo6.writeMicroseconds(1500);
  servo7.writeMicroseconds(1450);

  // arms
  servo8.writeMicroseconds(1500);
  servo9.writeMicroseconds(1500);
  servo10.writeMicroseconds(1500);
  servo11.writeMicroseconds(1500);

}

void loop() {

  currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {  // start 5ms timed loop  
      previousMillis = currentMillis; 

      // receive DMX data
      // head
      pot1 = (DMXSerial.read(1))*4;
      pot2 = (DMXSerial.read(2))*4;
      pot3 = (DMXSerial.read(3))*4;
      pot4 = (DMXSerial.read(4))*4;
      pot5 = (DMXSerial.read(5))*4;
      pot6 = (DMXSerial.read(6))*4;
      // arms
      pot8 = (DMXSerial.read(8))*4;
      pot9 = (DMXSerial.read(9))*4;
      pot10 = (DMXSerial.read(10))*4;
      pot11 = (DMXSerial.read(11))*4;

  
      // scale all pots for the servo microseconds range

      //head
      pot1Scaled = ((pot1 - 512) * -1.6) + 1200;
      pot2Scaled = (pot2 - 512) + 1500;
      pot3Scaled = (pot3 - 512) + 1500;
      pot4Scaled = ((pot4 - 512) * 1.2) + 1500;
      pot4aScaled = ((pot4 - 512) * -1.2) + 1500;
      pot5Scaled = (pot5 - 512) * 1.5;
      pot6Scaled = (pot6 - 512) + 1650;    
      pot4Scaled = constrain(pot4Scaled,900,1500);
      pot4aScaled = constrain(pot4aScaled,1500,2100);

      //arms
      pot8Scaled = ((pot8 - 512) *1.3) + 1500;
      pot9Scaled = ((pot9 - 512) * 1.3) + 1500;
      pot10Scaled = ((pot10 - 512) * 1.3) + 1500;
      pot11Scaled = ((pot11 - 512) * 1.3) + 1500;      
    
      // smooth pots
      
      pot1Smoothed = (pot1Scaled * 0.01) + (pot1SmoothedPrev * 0.99);
      pot2Smoothed = (pot2Scaled * 0.01) + (pot2SmoothedPrev * 0.99);
      pot3Smoothed = (pot3Scaled * 0.01) + (pot3SmoothedPrev * 0.99);
      pot4Smoothed = (pot4Scaled * 0.05) + (pot4SmoothedPrev * 0.95);
      pot4aSmoothed = (pot4aScaled * 0.05) + (pot4aSmoothedPrev * 0.95);
      pot5Smoothed = (pot5Scaled * 0.01) + (pot5SmoothedPrev * 0.99);
      pot6Smoothed = (pot6Scaled * 0.01) + (pot6SmoothedPrev * 0.99);  

      pot8Smoothed = (pot8Scaled * 0.01) + (pot8SmoothedPrev * 0.99);
      pot9Smoothed = (pot9Scaled * 0.01) + (pot9SmoothedPrev * 0.99);
      pot10Smoothed = (pot10Scaled * 0.01) + (pot10SmoothedPrev * 0.99);
      pot11Smoothed = (pot11Scaled * 0.01) + (pot11SmoothedPrev * 0.99);
    
      // bookmark previous values
    
      pot1SmoothedPrev = pot1Smoothed;
      pot2SmoothedPrev = pot2Smoothed;
      pot3SmoothedPrev = pot3Smoothed;
      pot4SmoothedPrev = pot4Smoothed;
      pot4aSmoothedPrev = pot4aSmoothed;
      pot5SmoothedPrev = pot5Smoothed;
      pot6SmoothedPrev = pot6Smoothed;

      pot8SmoothedPrev = pot8Smoothed;
      pot9SmoothedPrev = pot9Smoothed;
      pot10SmoothedPrev = pot10Smoothed;
      pot11SmoothedPrev = pot11Smoothed;
      
      /*
  
      Serial.print(pot1Smoothed);
      Serial.print(" , ");
      Serial.print(pot2Smoothed);
      Serial.print(" , ");
      Serial.print(pot3Smoothed);
      Serial.print(" , ");
      Serial.print(pot4Smoothed);
      Serial.print(" , ");
      Serial.print(pot5Smoothed);
      Serial.print(" , ");
      Serial.print(pot6Smoothed);
      Serial.print(" , ");
      Serial.print(pot8Smoothed);
      Serial.print(" , ");
      Serial.print(pot9Smoothed);
      Serial.print(" , ");
      Serial.print(pot10Smoothed);
      Serial.print(" , ");
      Serial.println(pot11Smoothed);

      */

      // write servos

      servo1.writeMicroseconds(pot1Smoothed);                     // neck rotate
      servo2.writeMicroseconds(pot6Smoothed - pot5Smoothed);      // neck side-side
      servo3.writeMicroseconds(pot6Smoothed + pot5Smoothed);      // neck front-back
    
      servo5.writeMicroseconds(pot2Smoothed);                     // eyeballs side-side
      servo7.writeMicroseconds(pot2Smoothed);
    
      servo4.writeMicroseconds(pot4Smoothed);                     // eyelids
      servo6.writeMicroseconds(pot4aSmoothed); 

      servo8.writeMicroseconds(pot8Smoothed);                    //arms
      servo9.writeMicroseconds(pot9Smoothed); 
      servo10.writeMicroseconds(pot10Smoothed); 
      servo11.writeMicroseconds(pot11Smoothed); 

      

  } // end of timed loop


} // end if main loop
