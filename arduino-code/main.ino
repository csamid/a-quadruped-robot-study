// Samid C (4/24/22) 

#include <Servo.h>   // arduino library

//create servo object:
Servo sBlue; // thigh  angle: psi
Servo sYellow; // shank  angle: eta

Servo sBlue1; 
Servo sYellow1; 

Servo sBlue2;
Servo sYellow2; 

Servo sBlue3; 
Servo sYellow3;

#define sPinB 9 
#define sPinY 10
#define sPinB1 3 
#define sPinY1 5
#define sPinB2 6
#define sPinY2 14
#define sPinB3 15
#define sPinY3 7


// setting servos init: this is when P(x,y) = P(0,0)
float sB_init  = 1405; // KEY: set zero to 0, servo moves CCW 
float sY_init  = 1390; // KEY: if we go with eta then 90 deg becomes the new 0 deg for Yellow, i.e. sY_angle = 90 - eta
float sB_init1 = 1555; 
float sY_init1 = 1530;
float sB_init2 = 1460; 
float sY_init2 = 1410;
float sB_init3 = 1385; 
float sY_init3 = 1405;

//pulse width microseconds to set the range the servos from 0 to 90
int min_pwB = 920; 
int max_pwB = 1890; 
int min_pwY = 930; 
int max_pwY = 1850; 
int min_pwB1 = 1080; 
int max_pwB1 = 2030; 
int min_pwY1 = 1060; 
int max_pwY1 = 2000; 
int min_pwB2 = 965; 
int max_pwB2 = 1955; 
int min_pwY2 = 930; 
int max_pwY2 = 1890;
int min_pwB3 = 925; 
int max_pwB3 = 1845; 
int min_pwY3 = 925; 
int max_pwY3 = 1885;


float xn,xn2;
float yn,yn2;
float psi_deg;
float eta_deg;




int i = 0;
int k = 0;
int r = 0;
int sw = 0;
int sw2 = 1;
const int endp = 60; // number of points  
float t;
unsigned long current; // us time 
unsigned long previous1;  // us time (timed event 1)
unsigned long previous2; //us time (timed event 2)
const unsigned long period = 12000; // us  <-- the time each servo write is commanded 
unsigned long h = 0;
unsigned long j = 0;
int stateNEW = 2;
int stateOLD;
char inByte;
// inverse kinematics
void IK(float Px, float Py);

void setup() {
  Serial.begin(115200);

  // Timer1 setup
  //timerInterruptSetup();


  // Write header
  Serial.print("Time(s), ");
  Serial.print("Px(mm), ");
  Serial.print("Py(mm)");
  Serial.println(); 

  // intializing servos 
  sBlue.writeMicroseconds(sB_init); // - 32.083
  sYellow.writeMicroseconds(sY_init); // + 53.472
  sBlue1.writeMicroseconds(sB_init1); 
  sYellow1.writeMicroseconds(sY_init1);
  sBlue2.writeMicroseconds(sB_init2); 
  sYellow2.writeMicroseconds(sY_init2);
  sBlue3.writeMicroseconds(sB_init3); 
  sYellow3.writeMicroseconds(sY_init3);

  
  sBlue.attach(sPinB,min_pwB,max_pwB); // "thigh"
  sYellow.attach(sPinY,min_pwY,max_pwY); // "shank" 
  sBlue1.attach(sPinB1,min_pwB1,max_pwB1); // "thigh"
  sYellow1.attach(sPinY1,min_pwY1,max_pwY1); // "shank"
  sBlue2.attach(sPinB2,min_pwB2,max_pwB2); // "thigh"
  sYellow2.attach(sPinY2,min_pwY2,max_pwY2); // "shank"
  sBlue3.attach(sPinB3,min_pwB3,max_pwB3); // "thigh"
  sYellow3.attach(sPinY3,min_pwY3,max_pwY3); // "shank"
  delay(1000);
  
  previous1 = micros();
  previous2 = micros();
}

void loop() {
  //get the current time in us
  current = micros();
  //Serial.print(stateOLD); Serial.print(","); Serial.println(stateNEW);

  // READ FROM SERIAL TO GET STATE 
  if (Serial.available() > 0) {
    inByte = Serial.read();
  }
  if (inByte == '1') {
    stateOLD = stateNEW;
    stateNEW = 1; //TROT
  }
  else if (inByte == '0') {
    stateOLD = stateNEW;
    stateNEW = 0; //WALK
  }
  else if(inByte == '3') {
    stateOLD = stateNEW;
    stateNEW = 3; //STOP
  }
  else if(inByte == '4') {
    stateOLD = stateNEW;
    stateNEW = 4; //HINGE
  }
  //--------------------------


  // Timed Event 1 //
  if(current - previous1 >= period) {
    // TROT IF STATE IS 1
    if (stateNEW == 1) {
      //TRANSITION FROM INIT TO TROT
      if (stateOLD == 2 || stateOLD == 3) {
        stateOLD = stateNEW;
      }
      //TRANSITION FROM WALK TO TROT
      else if (stateOLD == 0) {
        if (i <= 10) {
        //FORWARD
        xn = i*1;
        yn = 0;
        i += 1;
      }
      else if (i > 10 && i < 20) {
        //DOWN
        xn = xn;
        yn = yn - 1;
        i += 1;
      }
      else if (i >= 20 && i < 40) {
        //BACK
        xn = xn - 1;
        yn = yn;
        i += 1;
      }
      else if (i >= 40 && i < 50) {
        //UP
        xn = xn;
        yn = yn + 1;
        i += 1;
      }
      else {
        if (i == endp) {
          i = 0;
          stateOLD = stateNEW; //END OF TRANSITION
        }
        //FORWARD
        xn = xn + 1;
        yn = yn;
        i += 1;
      }

        // Get servo angles from end effector position
        IK(xn,yn);
        // Write to servo in us
        sBlue.writeMicroseconds(round(degtoMicroS_B(90-eta_deg)));
        sYellow.writeMicroseconds(round(degtoMicroS_Y(psi_deg)));
        sBlue3.writeMicroseconds(round(degtoMicroS_B3(90-eta_deg)));
        sYellow3.writeMicroseconds(round(degtoMicroS_Y3(psi_deg)));  
      }
      //TRANSITION FROM HINGE TO TROT
      else if (stateOLD == 4) {
        if (i <= 20) {
        //UP
        xn = 0;
        yn = i*1;
        i += 1;
      }
      else {
        if (i == 40) {
          i = 0;
          stateOLD = stateNEW; //END OF TRANSITION
        }
        //DOWN
        xn = 0;
        yn = yn - 1;
        i += 1;
      }

        // Get servo angles from end effector position
        IK(xn,yn);
        // Write to servo in us
        sBlue1.writeMicroseconds(round(degtoMicroS_B1(90-eta_deg)));
        sYellow1.writeMicroseconds(round(degtoMicroS_Y1(psi_deg)));
        sBlue3.writeMicroseconds(round(degtoMicroS_B3(90-eta_deg)));
        sYellow3.writeMicroseconds(round(degtoMicroS_Y3(psi_deg))); 
                // Get servo angles from end effector position
        IK(-xn,-yn);
        // Write to servo in us
        sBlue2.writeMicroseconds(round(degtoMicroS_B2(90-eta_deg)));
        sYellow2.writeMicroseconds(round(degtoMicroS_Y2(psi_deg)));
        sBlue.writeMicroseconds(round(degtoMicroS_B(90-eta_deg)));
        sYellow.writeMicroseconds(round(degtoMicroS_Y(psi_deg))); 
        
      }
   //TROT STARTS
    else if (stateOLD == stateNEW && r == 0) {
      if (i <= 20) {
        //UP
        xn = 0;
        yn = i*1;    
        i += 1;
      }
      else {
        if (i == 40) {
          i = 0;
          sw = !sw;    
        }
        //DOWN
        xn =  0;
        yn = yn - 1;
        i += 1;
      }
      if (sw) {
        IK(xn,yn);
        sBlue.writeMicroseconds(round(degtoMicroS_B(90-eta_deg)));
        sYellow.writeMicroseconds(round(degtoMicroS_Y(psi_deg)));
        sBlue3.writeMicroseconds(round(degtoMicroS_B3(90-eta_deg)));
        sYellow3.writeMicroseconds(round(degtoMicroS_Y3(psi_deg)));
      }
      else {
        IK(xn,yn);
        sBlue1.writeMicroseconds(round(degtoMicroS_B1(90-eta_deg)));
        sYellow1.writeMicroseconds(round(degtoMicroS_Y1(psi_deg)));
        sBlue2.writeMicroseconds(round(degtoMicroS_B2(90-eta_deg)));
        sYellow2.writeMicroseconds(round(degtoMicroS_Y2(psi_deg)));
      }
    }
   }
    
    // WALK IF STATE IS 0
    else if (stateNEW == 0) {
      //TRANSITION FROM INIT TO TROT
      if (stateOLD == 2 || stateOLD == 3) {
        stateOLD = stateNEW;
      }
      else if (stateOLD == 1) {
      //TRANSITION FROM TROT TO WALK
        if (i <= 20) {
          //UP
          xn = 0;
          yn = i*1;
        
          i += 1;
        }
        else {
          if (i == 40) {
             i = 0;
             sw = !sw; 
             stateOLD = stateNEW;   
          }
          //DOWN
          xn =  0;
          yn = yn - 1;
          i += 1;
        }
     if (sw) {
        IK(xn,yn);
        sBlue.writeMicroseconds(round(degtoMicroS_B(90-eta_deg)));
        sYellow.writeMicroseconds(round(degtoMicroS_Y(psi_deg)));
        sBlue3.writeMicroseconds(round(degtoMicroS_B3(90-eta_deg)));
        sYellow3.writeMicroseconds(round(degtoMicroS_Y3(psi_deg)));
      }
      else {
        IK(xn,yn);
        sBlue1.writeMicroseconds(round(degtoMicroS_B1(90-eta_deg)));
        sYellow1.writeMicroseconds(round(degtoMicroS_Y1(psi_deg)));
        sBlue2.writeMicroseconds(round(degtoMicroS_B2(90-eta_deg)));
        sYellow2.writeMicroseconds(round(degtoMicroS_Y2(psi_deg)));
      }
      }
      //TRANSITION FROM HINGE TO WALK
      else if (stateOLD == 4) {
        if (i <= 20) {
        //UP
        xn = 0;
        yn = i*1;
        i += 1;
      }
      else {
        if (i == 40) {
          i = 0;
          stateOLD = stateNEW; //END OF TRANSITION
        }
        //DOWN
        xn = 0;
        yn = yn - 1;
        i += 1;
      }

        // Get servo angles from end effector position
        IK(xn,yn);
        // Write to servo in us
        sBlue1.writeMicroseconds(round(degtoMicroS_B1(90-eta_deg)));
        sYellow1.writeMicroseconds(round(degtoMicroS_Y1(psi_deg)));
        sBlue3.writeMicroseconds(round(degtoMicroS_B3(90-eta_deg)));
        sYellow3.writeMicroseconds(round(degtoMicroS_Y3(psi_deg))); 
                // Get servo angles from end effector position
        IK(-xn,-yn);
        // Write to servo in us
        sBlue2.writeMicroseconds(round(degtoMicroS_B2(90-eta_deg)));
        sYellow2.writeMicroseconds(round(degtoMicroS_Y2(psi_deg)));
        sBlue.writeMicroseconds(round(degtoMicroS_B(90-eta_deg)));
        sYellow.writeMicroseconds(round(degtoMicroS_Y(psi_deg))); 
        
      }
      //---------------------------
      // START WALK (DIAG PAIRS 1) 
      else if (stateOLD == stateNEW) {
        if (i <= 10) {
        //FOWARD
        xn = i*1;
        yn = 0;     
        i += 1;
      }
      else if (i > 10 && i < 20) {
        //DOWN
        xn = xn;
        yn = yn - 1;
        i += 1;
      }
      else if (i >= 20 && i < 40) {
        if (i == endp/2) {
          r = 1;
        }
        //BACK
        xn = xn - 1;
        yn = yn;
        i += 1;
      }
      else if (i >= 40 && i < 50) {
        //UP
        xn = xn;
        yn = yn + 1;
        i += 1;
      }
      else {
        if (i == endp) {
          i = 0;
        }
        //FORWARD
        xn = xn + 1;
        yn = yn;
        i += 1;
      }
        // Get servo angles from end effector position
        IK(xn,yn);
        // Write to servo in us
        sBlue.writeMicroseconds(round(degtoMicroS_B(90-eta_deg)));
        sYellow.writeMicroseconds(round(degtoMicroS_Y(psi_deg)));
        sBlue3.writeMicroseconds(round(degtoMicroS_B3(90-eta_deg)));
        sYellow3.writeMicroseconds(round(degtoMicroS_Y3(psi_deg)));  
      }    
    }
    //STOP THE DOGO
    else if (stateNEW == 3) {
      //TRANSITION FROM INIT TO STOP
      if (stateOLD == 2) {
        stateOLD = stateNEW;
      }
      //TRANSITION FROM TROT TO STOP
      else if (stateOLD == 1) {
        if (i <= 20) {
          //UP
          xn = 0;
          yn = i*1;
        
          i += 1;
        }
        else {
          if (i == 40) {
             i = 0;
             sw = !sw; 
             stateOLD = stateNEW;   
          }
          //DOWN
          xn =  0;
          yn = yn - 1;
          i += 1;
        }
     if (sw) {
        IK(xn,yn);
        sBlue.writeMicroseconds(round(degtoMicroS_B(90-eta_deg)));
        sYellow.writeMicroseconds(round(degtoMicroS_Y(psi_deg)));
        sBlue3.writeMicroseconds(round(degtoMicroS_B3(90-eta_deg)));
        sYellow3.writeMicroseconds(round(degtoMicroS_Y3(psi_deg)));
      }
      else {
        IK(xn,yn);
        sBlue1.writeMicroseconds(round(degtoMicroS_B1(90-eta_deg)));
        sYellow1.writeMicroseconds(round(degtoMicroS_Y1(psi_deg)));
        sBlue2.writeMicroseconds(round(degtoMicroS_B2(90-eta_deg)));
        sYellow2.writeMicroseconds(round(degtoMicroS_Y2(psi_deg)));
      }  
      }
      //TRANSITION FROM WALK TO STOP
      else if (stateOLD == 0) {
        if (i <= 10) {
          //FORWARD
          xn = i*1;
          yn = 0;
          i += 1;
        }
        else if (i > 10 && i < 20) {
          //DOWN
          xn = xn;
          yn = yn - 1;
          i += 1;
        }
        else if (i >= 20 && i < 40) {
          //BACK
          xn = xn - 1;
          yn = yn;
          i += 1;
        }
        else if (i >= 40 && i < 50) {
          //UP
          xn = xn;
          yn = yn + 1;
          i += 1;
        }
        else {
          if (i == endp) {
            i = 0;
            stateOLD = stateNEW; //END OF TRANSITION
          }
          //FORWARD
          xn = xn + 1;
          yn = yn;
          i += 1;
        }

        // Get servo angles from end effector position
        IK(xn,yn);
        // Write to servo in us
        sBlue.writeMicroseconds(round(degtoMicroS_B(90-eta_deg)));
        sYellow.writeMicroseconds(round(degtoMicroS_Y(psi_deg)));
        sBlue3.writeMicroseconds(round(degtoMicroS_B3(90-eta_deg)));
        sYellow3.writeMicroseconds(round(degtoMicroS_Y3(psi_deg)));        
      }
      //TRANSITION FROM HINGE TO STOP
      else if (stateOLD == 4) {
        if (i <= 20) {
        //UP
        xn = 0;
        yn = i*1;
        i += 1;
      }
      else {
        if (i == 40) {
          i = 0;
          stateOLD = stateNEW; //END OF TRANSITION
        }
        //DOWN
        xn = 0;
        yn = yn - 1;
        i += 1;
      }

        // Get servo angles from end effector position
        IK(xn,yn);
        // Write to servo in us
        sBlue1.writeMicroseconds(round(degtoMicroS_B1(90-eta_deg)));
        sYellow1.writeMicroseconds(round(degtoMicroS_Y1(psi_deg)));
        sBlue3.writeMicroseconds(round(degtoMicroS_B3(90-eta_deg)));
        sYellow3.writeMicroseconds(round(degtoMicroS_Y3(psi_deg))); 
                // Get servo angles from end effector position
        IK(-xn,-yn);
        // Write to servo in us
        sBlue2.writeMicroseconds(round(degtoMicroS_B2(90-eta_deg)));
        sYellow2.writeMicroseconds(round(degtoMicroS_Y2(psi_deg)));
        sBlue.writeMicroseconds(round(degtoMicroS_B(90-eta_deg)));
        sYellow.writeMicroseconds(round(degtoMicroS_Y(psi_deg))); 
        
      }
      //START STOP
      else if (stateOLD == 3 && r == 0) {
        // do nothing
      }
    }
      else if (stateNEW == 4) {
        //TRANSITION FROM INIT TO HINGE
      if (stateOLD == 2 || stateOLD == 3) {
        stateOLD = stateNEW;
      }
      else if (stateOLD == 1) {
      //TRANSITION FROM TROT TO HINGE
        if (i <= 20) {
          //UP
          xn = 0;
          yn = i*1;
        
          i += 1;
        }
        else {
          if (i == 40) {
             i = 0;
             sw = !sw; 
             stateOLD = stateNEW;   
          }
          //DOWN
          xn =  0;
          yn = yn - 1;
          i += 1;
        }
     if (sw) {
        IK(xn,yn);
        sBlue.writeMicroseconds(round(degtoMicroS_B(90-eta_deg)));
        sYellow.writeMicroseconds(round(degtoMicroS_Y(psi_deg)));
        sBlue3.writeMicroseconds(round(degtoMicroS_B3(90-eta_deg)));
        sYellow3.writeMicroseconds(round(degtoMicroS_Y3(psi_deg)));
      }
      else {
        IK(xn,yn);
        sBlue1.writeMicroseconds(round(degtoMicroS_B1(90-eta_deg)));
        sYellow1.writeMicroseconds(round(degtoMicroS_Y1(psi_deg)));
        sBlue2.writeMicroseconds(round(degtoMicroS_B2(90-eta_deg)));
        sYellow2.writeMicroseconds(round(degtoMicroS_Y2(psi_deg)));
      }
      }
      //TRANSITION FROM WALK TO HINGE
      else if (stateOLD == 0) {
        if (i <= 10) {
          //FORWARD
          xn = i*1;
          yn = 0;
          i += 1;
        }
        else if (i > 10 && i < 20) {
          //DOWN
          xn = xn;
          yn = yn - 1;
          i += 1;
        }
        else if (i >= 20 && i < 40) {
          //BACK
          xn = xn - 1;
          yn = yn;
          i += 1;
        }
        else if (i >= 40 && i < 50) {
          //UP
          xn = xn;
          yn = yn + 1;
          i += 1;
        }
        else {
          if (i == endp) {
            i = 0;
            stateOLD = stateNEW; //END OF TRANSITION
          }
          //FORWARD
          xn = xn + 1;
          yn = yn;
          i += 1;
        }

        // Get servo angles from end effector position
        IK(xn,yn);
        // Write to servo in us
        sBlue.writeMicroseconds(round(degtoMicroS_B(90-eta_deg)));
        sYellow.writeMicroseconds(round(degtoMicroS_Y(psi_deg)));
        sBlue3.writeMicroseconds(round(degtoMicroS_B3(90-eta_deg)));
        sYellow3.writeMicroseconds(round(degtoMicroS_Y3(psi_deg)));        
      }
      //START HINGE
      else if (stateOLD == 4 && r == 0) {
        if (i <= 20) {
        //UP
        xn = 0;
        yn = i*1;
        i += 1;
      }
      else {
        if (i == 40) {
          i = 0;
          stateOLD = stateNEW; //END OF TRANSITION
        }
        //DOWN
        xn = 0;
        yn = yn - 1;
        i += 1;
      }

        // Get servo angles from end effector position
        IK(xn,yn);
        // Write to servo in us
        sBlue1.writeMicroseconds(round(degtoMicroS_B1(90-eta_deg)));
        sYellow1.writeMicroseconds(round(degtoMicroS_Y1(psi_deg)));
        sBlue3.writeMicroseconds(round(degtoMicroS_B3(90-eta_deg)));
        sYellow3.writeMicroseconds(round(degtoMicroS_Y3(psi_deg))); 
                // Get servo angles from end effector position
        IK(-xn,-yn);
        // Write to servo in us
        sBlue2.writeMicroseconds(round(degtoMicroS_B2(90-eta_deg)));
        sYellow2.writeMicroseconds(round(degtoMicroS_Y2(psi_deg)));
        sBlue.writeMicroseconds(round(degtoMicroS_B(90-eta_deg)));
        sYellow.writeMicroseconds(round(degtoMicroS_Y(psi_deg)));
      }
      }
    
    previous1 = current;
  }



  // Timed Event 2 //
    if (current - previous2 >= period) {
      //Wait till first pairs are halfway in their cycle and if state is 0 "WALK"
      if (r == 1 && stateNEW == 0) {  
        if (k <= 10) {  
        //FORWARD
        xn2 = k*1;
        yn2 = 0;
        k += 1;
      }
      else if (k > 10 && k < 20) {
        // DOWN
        xn2 = xn2;
        yn2 = yn2 - 1;
        k += 1;
      }
      else if (k >= 20 && k < 40) {
        //BACK
        xn2 = xn2 - 1;
        yn2 = yn2;
        k += 1;
      }
      else if (k >= 40 && k < 50) {
        // UP
        xn2 = xn2;
        yn2 = yn2 + 1;
        k += 1;
      }
      else {
        if (k == endp) {
          k = 0;
        }
        //FORWARD
        xn2 = xn2 + 1;
        yn2 = yn2;
        k += 1;
      }
    // Write to servos
    IK(xn2,yn2);
    sBlue1.writeMicroseconds(round(degtoMicroS_B1(90-eta_deg)));
    sYellow1.writeMicroseconds(round(degtoMicroS_Y1(psi_deg)));
    sBlue2.writeMicroseconds(round(degtoMicroS_B2(90-eta_deg)));
    sYellow2.writeMicroseconds(round(degtoMicroS_Y2(psi_deg)));
    }
    //TRANSITION FROM WALK TO TROT OR STOP OR HINGE
    else if ((r == 1 && stateNEW == 1) || (r == 1 && stateNEW == 3) || (r == 1 && stateNEW == 4)) {
      if (k <= 10) {
        //FORWARD
        xn2 = k*1;
        yn2 = 0;
        k += 1;
      }
      else if (k > 10 && k < 20) {
        // DOWN
        xn2 = xn2;
        yn2 = yn2 - 1;
        k += 1;
      }
      else if (k >= 20 && k < 40) {
        //BACK
        xn2 = xn2 - 1;
        yn2 = yn2;
        k += 1;
      }
      else if (k >= 40 && k < 50 ) {
        // UP
        xn2 = xn2;
        yn2 = yn2 + 1;
        k += 1;
      }
      else {
        if (k == endp) {
          k = 0;
          r = 0; //RETURN r to 0 TO START TROT OR STOP  
        }
        //FORWARD
        xn2 = xn2 + 1;
        yn2 = yn2;
        k += 1;
      }
    // Write to servos
    IK(xn2,yn2);
    sBlue1.writeMicroseconds(round(degtoMicroS_B1(90-eta_deg)));
    sYellow1.writeMicroseconds(round(degtoMicroS_Y1(psi_deg)));
    sBlue2.writeMicroseconds(round(degtoMicroS_B2(90-eta_deg)));
    sYellow2.writeMicroseconds(round(degtoMicroS_Y2(psi_deg))); 
    }
    //make current time the previous time
    previous2 = current;
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
    float MicroSB = min_pwB + degree*(970.0/90.0);
    return MicroSB;
}

float degtoMicroS_Y(float degree) {
    float MicroSY = min_pwY + degree*(920.0/90.0);
    return MicroSY;
}

float degtoMicroS_B1(float degree) {
    float MicroSB1 = min_pwB1 + degree*(960.0/90.0);
    return MicroSB1;
}

float degtoMicroS_Y1(float degree) {
    float MicroSY1 = min_pwY1 + degree*(930.0/90.0);
    return MicroSY1;
}

float degtoMicroS_B2(float degree) {
    float MicroSB2 = min_pwB2 + degree*(995.0/90.0);
    return MicroSB2;
}

float degtoMicroS_Y2(float degree) {
    float MicroSY2 = min_pwY2 + degree*(960.0/90.0);
    return MicroSY2;
}

float degtoMicroS_B3(float degree) {
    float MicroSB3 = min_pwB3 + degree*(920.0/90.0);
    return MicroSB3;
}

float degtoMicroS_Y3(float degree) {
    float MicroSY3 = min_pwY3 + degree*(960.0/90.0);
    return MicroSY3;
}
