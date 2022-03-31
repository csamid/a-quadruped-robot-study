#include <Servo.h>   // arduino library
//#include <Ramp.h> // Ramp lib for interpolation (smooth motion between the two servos)

//create servo object:
Servo sBlue; // thigh  angle: psi
Servo sYellow; // shank  angle: eta

#define sPinB 9 
#define sPinY 10

// setting servos init: this is when P(x,y) = P(0,0)
float sB_init = 1031; // KEY: set zero to 0, servo moves CCW 
float sY_init = 1031; // KEY: if we go with eta then 90 deg becomes the new 0 deg for Yellow, i.e. sY_angle = 90 - eta

//pulse width microseconds to set the range the servos from 0 to 180
int min_pw = 550; //default is 544
int max_pw = 2475; // default is 2400

//servo offsets in degree
float sB_offset = -3.0;
float sY_offset = 5.0;

// switch:
int sw = 0;

// leg vars:
float xn; // value to move in the x direction
float yn; // value to move in the y direction 
float psi_deg;
float eta_deg;

int endp = 100; // end point

// output to servos
float shankMs;
float thighMs;

// inverse kinematics
void foxyIK(float Px, float Py);

void setup() {
  Serial.begin(9600);
  
    // intializing servos 
  sBlue.writeMicroseconds(sB_init - 32.083); // 
  sYellow.writeMicroseconds(sY_init + 53.472); 
   
  sBlue.attach(sPinB,min_pw,max_pw); // "thigh"
  sYellow.attach(sPinY,min_pw,max_pw); // "shank"
  delay(2000);
   
  
}

void loop() {

  //delay(2000);

  if (sw <= 10) {
      Swing();
      //delay(1000);
      Stance();
      sw = sw + 1;
    }
  }



 


void foxyIK(float Px, float Py) {
  float S;
  float alpha;
  float b;
  float psi;
  float phi;
  float eta;
  //  foxy leg kinematics /
  //  constant values of leg links length. Links are: c1, c2, a1, a2  /
  //  [mm] this is the link connected to servo 1 and c2 /
  //  connected to c1 and a2 /
  //  parallelogram, a1 connected to servo 2 and a2 /
  //  link that touches the ground //
  //  solve for b : the resultant vector from the origin to the end effector //
  b = sqrt((91.92 - Py) * (91.92 - Py) + Px * Px);
  //  get the 2 angles that need to be sent to the servos (phi and psi) //
  S = acos((91.92 - Py) / b);
  // angle between b and the vertical d "y component of b" //
  if (Px < 0.0) {
    S = -S;
    //  if e is negative make the angle S negative //
  }
  alpha = acos(((b * b + 1024.0) - 1024.0) / (2.0 * b * 65));
  //  angle from right horizontal to b //
  psi = (PI - alpha) - ((PI/2.0) - S);
  phi = ((PI - 2.0 * alpha) - psi);
  eta = (PI/2.0) - phi;
  //  180 deg minus the two angles //
  //  isoceles triangle: so beta the angle from a2 and a1 is easy to get from
   // alpha //
  // angle between left horizontal to c1 //
  //  convert (phi and psi into degrees) //
  psi_deg = (180.0/PI) * psi;
  eta_deg = (180.0/PI) * eta;
  //return psi_deg, eta_deg;
}

float degtoMicroS(float degree) {
  float MicroS = 550.0 + degree*(1925.0/180.0);
  return MicroS;
}

void Dir(int dir) {
  float ystep;
  float xstep;
  if (dir) {
    ystep = 0.0;
    xstep = 0.5;
  }
  else {
    ystep = 0.5;
    xstep = 0.0;
  }
  for (int i = 0; i <= endp; i++) {  // for loop is equiv to linspace
      xn = i * xstep; 
      yn = i * ystep;
      foxyIK(xn,yn);
      sBlue.writeMicroseconds(degtoMicroS(psi_deg+sB_offset));
      sYellow.writeMicroseconds(degtoMicroS(90-eta_deg+sY_offset));
      delay(15);
      }
      for (int i = endp-1; i >= 0; i--) {  // for loop is equiv to linspace
      xn = i * xstep; 
      yn = i * ystep;
      foxyIK(xn,yn);
      sBlue.writeMicroseconds(degtoMicroS(psi_deg+sB_offset));
      sYellow.writeMicroseconds(degtoMicroS(90-eta_deg+sY_offset));
      delay(15);
      }
      for (int i = 0; i >= -endp; i--) {  // for loop is equiv to linspace
      xn = i * xstep; 
      yn = i * ystep;
      foxyIK(xn,yn);
      sBlue.writeMicroseconds(degtoMicroS(psi_deg+sB_offset));
      sYellow.writeMicroseconds(degtoMicroS(90-eta_deg+sY_offset));
      delay(15);
      }
      for (int i = -endp; i <= 0; i++) {  // for loop is equiv to linspace
      xn = i * xstep; 
      yn = i * ystep;
      foxyIK(xn,yn);
      sBlue.writeMicroseconds(degtoMicroS(psi_deg+sB_offset));
      sYellow.writeMicroseconds(degtoMicroS(90-eta_deg+sY_offset));
      delay(15);
      }
}

void Swing() {
  for (int i = 0; i <= endp; i++) {  // for loop is equiv to linspace
      xn = i * 0.25; 
      yn = sqrt(156.25 - pow((xn - 12.5),2));
      foxyIK(xn,yn);
      sBlue.writeMicroseconds(degtoMicroS(psi_deg+sB_offset));
      sYellow.writeMicroseconds(degtoMicroS(90-eta_deg+sY_offset));
      delay(2);
      }
}

void Stance() {
  for (int i = endp; i >= 0; i--) {
    xn = xn - 0.25;
    yn = yn;
    foxyIK(xn,yn);
    sBlue.writeMicroseconds(degtoMicroS(psi_deg+sB_offset));
    sYellow.writeMicroseconds(degtoMicroS(90-eta_deg+sY_offset));
    delay(2);
  }
}
