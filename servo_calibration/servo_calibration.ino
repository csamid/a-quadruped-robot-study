#include <Servo.h>   // arduino library

//create servo object:
Servo myServoB;
Servo myServoY;
int sw = 1; // switch
#define servoPinB 6 // connected to digital pin 9 in arduino board
#define servoPinY 12
int angle;
void rot180(Servo servo_number);
//pulse width determines the position of the output shaft:
int min_pwB = 970; //default is 544
int max_pwB = 1945; // default is 2400   this pwm range is about 0 to 180 we 
int min_pwY = 1075; //default is 544
int max_pwY = 2075; // default is 2400   this pwm range is about 0 to 180 we 
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
  for (angle = max_pwB; angle >= 1458; angle -= 1) { //offset is about 3 deg for servo1, so 90 deg is around 1480 us
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

// Notes on current servos:
// Servo with blue mark:
  //offset is about -3 deg for servo1, so 90 deg is around 1480 us, 45 deg is around 999
// Servo with no mark:
  //offset is about 5 deg "servo2", so 90 deg is around 1566 us
  // if we go with eta then 90 deg becomes the new 0 deg for servo2


  // notes 4/21, mg90s servo calibration: 
  //servoBlue1: min: 900 max: 1900
  // servoYellow1: min: 910 max: 1910
  // servoBlue2: min: 1025 max: 2050
  // servoYellow2: min: 935 max: 1935

  //notes 4/22, recal mg92B servos
  // B1: min: 970   max: 1945 
  // Y1: min: 925   max: 1875
