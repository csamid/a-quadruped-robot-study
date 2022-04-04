#include <Servo.h>   // arduino library

//create servo object:
Servo myServo1;
Servo myServo2;
int sw = 1; // switch
#define servoPin1 9 // connected to digital pin 9 in arduino board
#define servoPin2 11
int angle = 1000;
void rot180(Servo servo_number);
//pulse width determines the position of the output shaft:
int min_pw = 550; //default is 544
int max_pw = 2475; // default is 2400   this pwm range is about 0 to 180 we 
// can use this to map the angle we get to this pulse range

void setup() {
  Serial.begin(9600);
  myServo1.write(min_pw);
  myServo2.write(min_pw);
  delay(500);
  // attach servo var to connected pin
  myServo1.attach(servoPin1,min_pw,max_pw); 
  myServo2.attach(servoPin2,min_pw,max_pw);
  //myServo1.attach(servoPin1); 
  //myServo2.attach(servoPin2);

}

void loop() {
 
  // Sweep from 0 to 180 degrees:
  delay(500);
  if (sw) {
  rot180(myServo1);
  //rot180(myServo2);
  

  // And back from 180 to 0 degrees:
  for (angle = 1566; angle >= 1085; angle -= 1) { //offset is about 3 deg for servo1, so 90 deg is around 1480 us
    myServo1.writeMicroseconds(angle);
    delay(5);
  }

  delay(2500);
  sw = 0;
  }
}


float degtoMicroS(float degree) {
  float MicroS = 550.0 + degree*(1925.0/180.0);
  return MicroS;
}

void rot180(Servo servo_number) {
  for (int angle = 0; angle <= 180; angle += 1) {
    //degtoMicroS((float)angle)
    servo_number.writeMicroseconds(degtoMicroS((float)angle));
    delay(5);
  }
}

// Notes on current servos:
// Servo with blue mark:
  //offset is about -3 deg for servo1, so 90 deg is around 1480 us, 45 deg is around 999
// Servo with no mark:
  //offset is about 5 deg "servo2", so 90 deg is around 1566 us
  // if we go with eta then 90 deg becomes the new 0 deg for servo2
