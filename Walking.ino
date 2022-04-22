//Samid Ceballos (4/21/22)
// code to start 4 leg walking hopefully

// LIBRARIES  
#include <Servo.h>
#include <math.h>

// CONSTANTS  

//servo stuff
#define num 6 // number of servos
const int SrvPin[num] = {3,5,6,9,10,11};  //servo pins
const int minPW[num] =  {900,910,1025,935,0,0}; // servo's respective min pulse width
const int maxPW[num] = {1900,1910,2025,1935,0,0}; // servo's max pulse width
const int SrvInit[num] = {1400,1410,1525,1435,0,0}; // servo's 45 degree position to start 

// gait stuff
const int endp = 50; // number of points of the gait cycle
const int period = 12500; // in us, time each servowrite is commanded, "speed" of the gait
const int gait_cycles = 1;

// GLOBAL VARIABLES
unsigned long current; // us current time
unsigned long previous; // us previous time
unsigned long  i = 0; // used as a increment for the gait 
unsigned long h = 0; // used as a increment for the gait 
float t; // time in loop (seconds)

float xn; // x dir, mm
float yn; // y dir, mm

/* 
convention: blue servos use the psi angle "thigh", yellow servos use the eta angle "shank"
the yellow servos angle is: sY_angle = 90 - eta_deg                                         */
float psi_deg;
float eta_deg;

// GLOBAL FUNCTIONS
void foxyIK(float Px, float Py); // inverse kinematics
//----------------------------------------------------

// creating servo objects
Servo Srv[num]; // check diagram for servo numbering (Github)


void setup() {
  Serial.begin(115200);

  // Write header
  Serial.print(F("Time(s), "));
  Serial.print(F("Px(mm), "));
  Serial.print(F("Py(mm)"));
  Serial.println(); 

  // intializing servos
  for (int j = 0; j < num; j++) {
    Srv[j].writeMicroseconds(SrvInit[j]);
    Srv[j].attach(SrvPin[j],minPW[j],maxPW[j]);
  }

   previous = micros();
}

void loop() {
  //get current time 
  current = micros();

  // timed event //
  if (current - previous >= period) {
    if (h <= endp*gait_cycles) {
      if (i <= endp/2) {
        //SWING
        xn = i*1;
        yn = sqrt(156.25 - pow((xn - 12.5),2));
        i = i + 1; //increment i to act like a for loop
      }
      else if (i <= endp) {
        if (i == endp) {
          //rest the gait so that it starts again
          i = 0;
        }
        //STANCE
        xn = xn - 1;
        yn = yn;
        i = i + 1; //increment i to act like a for loop
      }

      // Get servo angles from end effector position (IK)
      foxyIK(xn,yn);
      // Write to servo 


      //

      // get time(s)
      t = ((float) millis())/1000.0;

      //print monitor
      Serial.print(t,4);
      Serial.print(F(" "));
      Serial.print(xn);
      Serial.print(F(" "));
      Serial.print(yn);
      Serial.print(F(" "));
      Serial.println();

      previous = current; //make previous the current time
      h += 1; //increment h
    }
    else {
      //servo detach

      //
    }

    
  }
  

}

void setPwmFrequency(int pin, int divisor) {
  byte mode;
  if(pin == 5 || pin == 6 || pin == 9 || pin == 10) {
    switch(divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 64: mode = 0x03; break;
      case 256: mode = 0x04; break;
      case 1024: mode = 0x05; break;
      default: return;
    }
    if(pin == 5 || pin == 6) {
      TCCR0B = TCCR0B & 0b11111000 | mode;
    } else {
      TCCR1B = TCCR1B & 0b11111000 | mode;
    }
  } else if(pin == 3 || pin == 11) {
    switch(divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 32: mode = 0x03; break;
      case 64: mode = 0x04; break;
      case 128: mode = 0x05; break;
      case 256: mode = 0x06; break;
      case 1024: mode = 0x7; break;
      default: return;
    }
    TCCR2B = TCCR2B & 0b11111000 | mode;
  }
}



  // notes 4/21, mg90s servo calibration: 
  //servoBlue1: min: 900 max: 1900
  // servoYellow1: min: 910 max: 1910
  // servoBlue2: min: 1025 max: 2050
  // servoYellow2: min: 935 max: 1935
