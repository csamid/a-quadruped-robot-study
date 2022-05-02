// Samid Ceballos (4/24/22) 
// practicing different times of ways to generate a gait, previously used for loops to generate 
// it but for loops stop everything else from the code from running until it finishes
// the easiest way would be to just have a variable of the array (gait) we want.
// but for now this code uses if statements to generate the gait 


#include <Servo.h>   // arduino library

//create servo object:
Servo sBlue; // thigh  angle: psi
Servo sYellow; // shank  angle: eta

Servo sBlue1; // thigh  angle: psi
Servo sYellow1; // shank  angle: eta

Servo sBlue2; // thigh  angle: psi
Servo sYellow2; // shank  angle: eta

Servo sBlue3; // thigh  angle: psi
Servo sYellow3; // shank  angle: eta

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
// Index ----------> 0, 1, 2, 3, 4,  5
//int ServoPins[6] = { 3, 5, 6, 9, 10, 11};

/*
#define COMPARE_MATCH 0x271 // 3125, 100Hz = 10ms
static double dt = 0.01; // 0.01s = 10ms
volatile uint8_t flag = 0;
float t;
int sw = 1;
*/

int i = 0;
int k = 0;
const int endp = 60; // number of points 
float t;
unsigned long current; // us time 
unsigned long previous1;  // us time (timed event 1)
unsigned long previous2; //us time (timed event 2)
const unsigned long period = 10000; // us  <-- the time each servo write is commanded 
unsigned long h = 0;
unsigned long j = 0;

// inverse kinematics
void foxyIK(float Px, float Py);

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

  const int gait_cycles = 10;

  
  // Timed Event 1 //
  if(current - previous1 >= period) {
    if (h <= endp*gait_cycles) {
      if (i <= endp/2) {
      
        //UP LEG 2
        xn = xn;
        yn = yn + 1;
        
        i += 1;
      }
      else if (i > endp/2) {
        if (i == endp) {
          i = 0;
        }
        // DOWN LEG 2
        xn = xn;
        yn = yn - 1;

        i += 1;
      }

    // Get servo angles from end effector position
    foxyIK(xn,yn);
    // Write to servo in us
    sBlue.writeMicroseconds(round(degtoMicroS_B(psi_deg)));
    sYellow.writeMicroseconds(round(degtoMicroS_Y(90-eta_deg)));
    sBlue2.writeMicroseconds(round(degtoMicroS_B2(psi_deg)));
    sYellow2.writeMicroseconds(round(degtoMicroS_Y2(90-eta_deg)));
    sBlue1.writeMicroseconds(round(degtoMicroS_B1(psi_deg)));
    sYellow1.writeMicroseconds(round(degtoMicroS_Y1(90-eta_deg)));
    sBlue3.writeMicroseconds(round(degtoMicroS_B3(psi_deg)));
    sYellow3.writeMicroseconds(round(degtoMicroS_Y3(90-eta_deg)));
    /*
    foxyIK(xn2,yn2);
    sBlue1.writeMicroseconds(round(degtoMicroS_B1(psi_deg)));
    sYellow1.writeMicroseconds(round(degtoMicroS_Y1(90-eta_deg)));
        sBlue3.writeMicroseconds(round(degtoMicroS_B3(psi_deg)));
    sYellow3.writeMicroseconds(round(degtoMicroS_Y3(90-eta_deg)));
    */
    
    t = ((float) millis())/1000.0; // calc time in seconds

    //print to monitor
    Serial.print(t,4); Serial.print(" "); Serial.print(xn); Serial.print(" "); Serial.print(yn); Serial.print(" "); Serial.println(); 

    //make current time the previous time
    previous1 = current;
    h += 1;
  }
  else {
    sBlue.detach(); sYellow.detach(); sBlue2.detach(); sYellow2.detach(); sBlue1.detach(); sYellow1.detach(); sBlue3.detach(); sYellow3.detach(); 
  }
  }
/*
  // Timed Event 2 //
  if (current/1000 > (period/1000)*endp*2.4) {
    if (current - previous2 >= period) {
      if (j <= endp*gait_cycles) {
        if (k <= endp/2 - 5) {
          //SWING LEG 1
          xn2 = k*1;
          yn2 = sqrt(156.25 - pow((xn2 - 12.5),2));

          k += 1;
        }
        else if (k <= endp/2 & k > endp/2 - 5) {
          // DOWN LEG 1
          xn2 = xn2;
          yn2 = yn2 - 1;

          k += 1;
        }
        else if (k <= endp - 5 & k > endp/2) {
          // STANCE LEG 1
          xn2 = xn2 - 1;
          yn2 = yn2;
          k += 1;
        }
        else {
          if (k == endp) {
            k = 0;
          }
        //UP LEG 1
        xn2 = xn2;
        yn2 = yn2 + 1;

        k += 1;
    }
    // Write to servos
    foxyIK(xn2,yn2);
    sBlue1.writeMicroseconds(round(degtoMicroS_B1(psi_deg)));
    sYellow1.writeMicroseconds(round(degtoMicroS_Y1(90-eta_deg)));
    sBlue3.writeMicroseconds(round(degtoMicroS_B3(psi_deg)));
    sYellow3.writeMicroseconds(round(degtoMicroS_Y3(90-eta_deg)));

    //make current time the previous time
    previous2 = current;
    j += 1;
  }
  else {
    sBlue1.detach(); sYellow1.detach(); sBlue3.detach(); sYellow3.detach();
  }
  }
  } 
  */
  
}


/*
ISR(TIMER1_COMPA_vect) {
  flag = 1;
}


void timerInterruptSetup() {
  // Timer/Counter 1
  // Reset Timer1 Control Reg A
  TCCR1A = 0x00;
  
  // Set prescalar to 256
  TCCR1B |= (1 << CS12);
  TCCR1B &= ~(1 << CS11);
  TCCR1B &= ~(1 << CS10);
  
  // Set CTC mode
  TCCR1B &= ~(1 << WGM13);
  TCCR1B |= (1 << WGM12);
  
  //Initialize Timer1 and Set compare match
  TCNT1 = 0x00;
  OCR1A = COMPARE_MATCH;

  // Enable Timer1 compare interrupt
  TIMSK1 = (1 << OCIE1A);

  // enable global interrupts
  sei();
}
*/

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
///
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
