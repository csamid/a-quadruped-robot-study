//Samid C (4/21/22)
// code to start 4 leg walking hopefully :)

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
unsigned long  i = 0; // used as a increment for the gait (might make h, and i local vars and store them in a function)
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
void IK(float Px, float Py); // inverse kinematics
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
      IK(xn,yn);
      // Write to servo 
      Srv[0].writeMicroseconds(degtoMicros(psi_deg,0));
      Srv[1].writeMicroseconds(degtoMicros(90-eta_deg,1));

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

void IK(float Px, float Py) {
  //  leg inverse kinematics // 
  float S;
  float alpha;
  float b;
  float psi;
  float phi;
  float eta;

  //  solve for b : the resultant vector from the origin to the end effector //
  b = sqrt((91.92 - Py) * (91.92 - Py) + Px * Px);

  //  get the 2 angles that need to be sent to the servos (phi and psi) //

  // angle between b and the vertical d "y component of b" //
  S = acos((91.92 - Py) / b);
  if (Px < 0.0) {
    //  if e is negative make the angle S negative //
    S = -S;
  }

  alpha = acos(((b * b + 1024.0) - 1024.0) / (2.0 * b * 65));

  psi = (PI - alpha) - ((PI/2.0) - S);
  phi = ((PI - 2.0 * alpha) - psi);
  eta = (PI/2.0) - phi;

  //  convert (phi and psi into degrees) //
  psi_deg = (180.0/PI) * psi; 
  eta_deg = (180.0/PI) * eta;

}

float degtoMicros(float degree, int srv_num) {
  float micro = minPW[srv_num] + degree*(1000.0/90.0);
  return micro;
}
