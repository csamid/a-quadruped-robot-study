// Samid Ceballos (4/16/22) 
// practicing different times of ways to generate a gait, previously used for loops to generate 
// it but for loops stop everything else from the code from running until it finishes
// the easiest way would be to just have a variable of the array (gait) we want.
// but for now this code uses if statements to generate the gait 


#include <Servo.h>   // arduino library

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

float xn;
float yn;
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
const int endp = 25; // number of points 
float t;
unsigned long current; // ms time 
unsigned long previous;  // ms time 
const unsigned long period = 12500; // dt = 25ms  <-- the time each servo write is commanded 
unsigned long h = 0;

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
   
  sBlue.attach(sPinB,min_pwB,max_pwB); // "thigh"
  sYellow.attach(sPinY,min_pwY,max_pwY); // "shank"

  previous = micros();
}

void loop() {

  //get the current time in ms
  current = micros();

  const int gait_cycles = 1;

  
  // timed event //
  if(current - previous >= period) {
  if (h <= endp*2*gait_cycles) {
    if (i <= endp) {
     
        xn = i*1;
        yn = sqrt(156.25 - pow((xn - 12.5),2));
        i = i + 1; 
        
    }
    else if (i <= endp*2) {
      if (i == endp*2) {
        i = 0;
      }
      xn = xn - 1;
      yn = yn;
      i = i + 1;
    }

    // Get servo angles from end effector position
    foxyIK(xn,yn);
    // Write to servo in us
    sBlue.writeMicroseconds(round(degtoMicroS_B(psi_deg)));
    sYellow.writeMicroseconds(round(degtoMicroS_Y(90-eta_deg)));
    t = ((float) millis())/1000.0; // calc time in seconds

    //print to monitor
    Serial.print(t,4);
    Serial.print(" ");
    Serial.print(xn);
    Serial.print(" ");
    Serial.print(yn);
    Serial.print(" ");
    Serial.println(); 

    //make current time the previous time
    previous = current;
    h = h + 1;
  }
  else {
    sBlue.detach(); // "thigh"
    sYellow.detach(); // "shank"
  }
  }
 
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
  //return psi_deg, eta_deg;
}

float degtoMicroS_B(float degree) {
    float MicroSB = min_pwB + degree*(1000.0/90.0);
    return MicroSB;
}

float degtoMicroS_Y(float degree) {
    float MicroSY = min_pwY + degree*(1000.0/90.0);
    return MicroSY;
}
