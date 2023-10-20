// single leg tests

#include <Servo.h>   // servo library

//create servo object:
Servo sBlue; // thigh  angle: psi
Servo sYellow; // shank  angle: eta

#define sPinB 9 
#define sPinY 10


// setting servos init: this is when P(x,y) = P(0,0)
float sB_init = 1450; // KEY: set zero to 0, servo moves CCW 
float sY_init = 1575; // KEY: if we go with eta then 90 deg becomes the new 0 deg for Yellow, i.e. sY_angle = 90 - eta

//pulse width microseconds to set the range the servos from 0 to 180
int min_pwB = 950; 
int max_pwB = 1950; 
int min_pwY = 1075; 
int max_pwY = 2075; 

// switch:
int sw = 0;
//delay
int d = 15;

// leg vars:
float xn; // value to move in the x direction
float yn; // value to move in the y direction 
float psi_deg;
float eta_deg;

int endp = 25; // end point

// inverse kinematics
void IK(float Px, float Py);

void setup() {
  Serial.begin(9600);
  
  // intializing servos 
  sBlue.writeMicroseconds(sB_init); // - 32.083
  sYellow.writeMicroseconds(sY_init); // + 53.472
   
  sBlue.attach(sPinB,min_pwB,max_pwB); // "thigh"
  sYellow.attach(sPinY,min_pwY,max_pwY); // "shank"
  delay(2000);
  
}

void loop() {

  //delay(2000);

  if (sw < 1) {
    
      Swing();
      Down();
      Stance();
      Up();
      Right();
      sw = sw + 1;
    }
  else {
    sBlue.detach(); // "thigh"
    sYellow.detach(); // "shank"
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

float degtoMicroS_B(float degree) {
    float MicroSB = min_pwB + degree*(1000.0/90.0);
    return MicroSB;
}

float degtoMicroS_Y(float degree) {
    float MicroSY = min_pwY + degree*(1000.0/90.0);
    return MicroSY;
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
  for (int i = 0; i <= endp; i++) {  
      xn = i * xstep; 
      yn = i * ystep;
      IK(xn,yn);
      sBlue.writeMicroseconds(degtoMicroS_B(psi_deg));
      sYellow.writeMicroseconds(degtoMicroS_Y(90-eta_deg));
      delay(15);
      }
      for (int i = endp-1; i >= 0; i--) {  
      xn = i * xstep; 
      yn = i * ystep;
      IK(xn,yn);
      sBlue.writeMicroseconds(degtoMicroS_B(psi_deg));
      sYellow.writeMicroseconds(degtoMicroS_Y(90-eta_deg));
      delay(15);
      }
      for (int i = 0; i >= -endp; i--) {  
      xn = i * xstep; 
      yn = i * ystep;
      IK(xn,yn);
      sBlue.writeMicroseconds(degtoMicroS_B(psi_deg));
      sYellow.writeMicroseconds(degtoMicroS_Y(90-eta_deg));
      delay(15);
      }
      for (int i = -endp; i <= 0; i++) { 
      xn = i * xstep; 
      yn = i * ystep;
      IK(xn,yn);
      sBlue.writeMicroseconds(degtoMicroS_B(psi_deg));
      sYellow.writeMicroseconds(degtoMicroS_Y(90-eta_deg));
      delay(15);
      }
}

void Swing() {
  for (int i = 0; i <= endp; i++) { 
      xn = i * 1; 
      yn = sqrt(156.25 - pow((xn - 12.5),2));
      IK(xn,yn);
      Serial.print(xn);
      Serial.print(',');
      Serial.println(yn);
      sBlue.writeMicroseconds(round(degtoMicroS_B(psi_deg)));
      sYellow.writeMicroseconds(round(degtoMicroS_Y(90-eta_deg)));
      delay(d);
      }
}

void Down() {
  for (int i = 0; i < 15; i++) {
    xn = xn;
    yn = yn - 1;
    IK(xn,yn);
    Serial.print(xn);
    Serial.print(',');
    Serial.println(yn);
    sBlue.writeMicroseconds(round(degtoMicroS_B(psi_deg)));
    sYellow.writeMicroseconds(round(degtoMicroS_Y(90-eta_deg)));
    delay(d);
  }
}

void Stance() {
  for (int i = endp + 15; i > 0; i--) {
    xn = xn - 1;
    yn = yn;
    IK(xn,yn);
    Serial.print(xn);
    Serial.print(',');
    Serial.println(yn);
    sBlue.writeMicroseconds(round(degtoMicroS_B(psi_deg)));
    sYellow.writeMicroseconds(round(degtoMicroS_Y(90-eta_deg)));
    delay(d);
  }
}

void Up() {
  for (int i = 0; i < 15; i++) {
    xn = xn;
    yn = yn + 1;
    IK(xn,yn);
    Serial.print(xn);
    Serial.print(',');
    Serial.println(yn);
    sBlue.writeMicroseconds(round(degtoMicroS_B(psi_deg)));
    sYellow.writeMicroseconds(round(degtoMicroS_Y(90-eta_deg)));
    delay(d);
  }
}

void Right() {
  for (int i = 0; i < 15; i++) {
    xn = xn + 1;
    yn = yn;
    IK(xn,yn);
    Serial.print(xn);
    Serial.print(',');
    Serial.println(yn);
    sBlue.writeMicroseconds(round(degtoMicroS_B(psi_deg)));
    sYellow.writeMicroseconds(round(degtoMicroS_Y(90-eta_deg)));
    delay(d);
  }
}
