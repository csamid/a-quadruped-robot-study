#include <Servo.h>   // arduino library

//create servo object:
Servo myServoB;
Servo myServoY;
int sw = 1; // switch
#define servoPinB 9 // connected to digital pin 9 in arduino board
#define servoPinY 12
int angle;
void rot180(Servo servo_number);
//pulse width determines the position of the output shaft:
int min_pwB = 930; //default is 544
int max_pwB = 1850; // default is 2400   this pwm range is about 0 to 180 we 
//int min_pwY = 1075; //default is 544
//int max_pwY = 2075; // default is 2400   this pwm range is about 0 to 180 we 
// can use this to map the angle we get to this pulse range

void setup() {
  Serial.begin(9600);
  myServoB.write(min_pwB);
  //myServoY.write(min_pwY);
  delay(500);
  // attach servo var to connected pin
  myServoB.attach(servoPinB,min_pwB,max_pwB); 
  //myServoY.attach(servoPinY,min_pwY,max_pwY);

}

void loop() {
 
  // Sweep from 0 to 180 degrees:
  delay(500);
  if (sw) {
  //rot180(myServoY);
  rot180(myServoB);
  
  delay(5000);
  // And back from 180 to 0 degrees:
  for (angle = max_pwB; angle >= min_pwB; angle -= 1) { //offset is about 3 deg for servo1, so 90 deg is around 1480 us
    myServoB.writeMicroseconds(angle);
    delay(3);
  }

  delay(2500);

  sw = 0;
  }
}


float degtoMicroS(float degree, int servo_number) {
  if (servo_number = 1) {
    float MicroS = min_pwB + degree*(1000.0/90.0);
    return MicroS;
  }
}

void rot180(Servo servo_number) {
  for (int i = min_pwB; i <= max_pwB; i++) {
    //degtoMicroS((float)angle)
    servo_number.writeMicroseconds(i);
    delay(3);
  }
}

// Update 4/30 Servos calibration

// srv Black1: min: 1060 max: 2000 mid: 1530   pin 5
// srv Blue2: min: 1080 max: 2030 mid: 1555    pin 3
// srv Yellow3: min: 930 max: 1890 mid: 1410 pin 14
// srv Red4: min: 965 max: 1955 mid: 1460 pin 6
// srv White5: min: 925 max: 1885 mid: 1405 pin 7
// srv Green6: min: 925 max: 1845 mid: 1385 pin 15
// srv BlackBlue7: min: 930 max: 1850 mid: 1390 pin 10
// srv BlueBlack8: min: 920  max: 1890 mid: 1405 pin 9
 
