// test code for servo calibration

// adjust max_pw and min_pw to make each servo 
// have a range from 0 to 180 degrees

#include <Servo.h>   // arduino library

//create servo object:
Servo myServoB;
Servo myServoY;

int sw = 1; // switch
#define servoPinB 6 
#define servoPinY 12
int angle;
void rot180(Servo servo_number);

//pulse width determines the position of the output shaft:
int min_pwB = 970; //default is 544
int max_pwB = 1945; // default is 2400  
int min_pwY = 1075; //default is 544
int max_pwY = 2075; // default is 2400 

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
  for (angle = max_pwB; angle >= min_pwB; angle -= 1) { 
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
