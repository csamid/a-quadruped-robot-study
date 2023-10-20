// Samid C

// code to compare roll and pitch estimates to HEDS 5540 Encoder angle output
// and show the effects of alpha (filter coefficient of the complementary filter)

// data sent via serial can be saved by readBasic.py or SerialReadSave.py

#include "Wire.h"
#include "I2Cdev.h"
#include "MPU9250.h"
#include "MPU6050.h"
#include "math.h"

// class default I2C address is 0x68
MPU9250 imu;
//MPU6050 imu;
I2Cdev   I2C_M;

uint8_t buffer_m[6];


int16_t ax, ay, az;
int16_t gx, gy, gz;
int16_t mx, my, mz;
float Axyz[3];
float p, q, r; // gyro body angular rates 
float N = 1000;
double bias[5]; // 0 through 2 is for Gyro, 3 through 4 is for Accel
double phi_hat_acc; // roll accel estimate
double theta_hat_acc; // pitch accel estimate
double phi_hat_acc1 = 0; // roll accel estimate
double theta_hat_acc1 = 0; // pitch accel estimate
double phi_hat = 0.0; // roll estimate 
double theta_hat = 0.0; // pitch estimate
double phi_hat2 = 0.0; // roll estimate 
double theta_hat2 = 0.0; // pitch estimate
double phi_hat3 = 0.0; // roll estimate 
double theta_hat3 = 0.0; // pitch estimate
static double alpha = 0.05; // Filter coefficient
static double alpha2 = 0.2; // Filter coefficient
static double alpha3 = 0.5; // Filter coefficient
double phi_dot; // gyro fixed frame rate
double theta_dot; // gyro fixed frame rate
float t;
float phiLPF; // roll accel estimate (filtered)
float phiLPF1;
float thetaLPF; // pitch accel estimate (filtered)
float thetaLPF1; // pitch accel estimate (filtered)
double phi_hat_gyro = 0.0; // pitch estimate gyro  
double theta_hat_gyro = 0.0; // roll estimate gyro
double phi_dot_gyro; // gyro fixed frame rate
double theta_dot_gyro; // gyro fixed frame rate

// Encoder variables and pins
#define encPin1 2
#define encPin2 3
volatile long encCount = 0;
volatile int pin1state;
volatile int pin2state;
volatile char REGVALS;
void pin1Change();
void pin2Change();
long abs_encCount;
float true_phi;

#define COMPARE_MATCH 0x271 // 3125, 100Hz = 10ms
static double dt = 0.01; // 0.01s = 10ms
volatile uint8_t flag = 0;



void setup() {
  // join I2C bus (I2Cdev library doesn't do this automatically)
    Wire.begin();

    // initialize serial communication
    Serial.begin(115200);

    // initialize device
    imu.initialize();
    
    // verify connection
    //Serial.println("Testing device connections...");
    //Serial.println(imu.testConnection() ? "MPU9250 connection successful" : "MPU9250 connection failed");

    //encoder setup
    pinMode(encPin1, INPUT);
    pinMode(encPin2, INPUT);
    attachInterrupt(0, pin1Change, CHANGE);
    attachInterrupt(1, pin2Change, CHANGE);
    
    // Timer1 setup
    timerInterruptSetup();

    // Calculate accel and gyro bias 
    getSensor_bias();

    // Write header
    Serial.print("Time(s), ");
    Serial.print("Roll(deg), ");
    Serial.print("Pitch(deg)");
    Serial.println();
    
}

void loop() {
   if (flag) {

    // Get estimated raw gyro data and subtract biases
    getGyro_Data();
    p -= bias[0];
    q -= bias[1];
    r -= bias[2];

    // Get estimated angles from raw accel and subtract biases  
    getAccel_angles();
    phi_hat_acc -= bias[3];
    theta_hat_acc -= bias[4];


    // Calculate Euler angle derivatives (body to fixed frame)
    phi_dot = p + sin(phi_hat) * tan(theta_hat) * q + cos(phi_hat) * tan(theta_hat) * r;
    theta_dot = cos(phi_hat) * q - sin(phi_hat) * r;

    // Calculate Euler angle derivatives (body to fixed frame) (just Gyro)
    phi_dot_gyro = p + sin(phi_hat_gyro) * tan(theta_hat_gyro) * q + cos(phi_hat_gyro) * tan(theta_hat_gyro) * r;
    theta_dot_gyro = cos(phi_hat_gyro) * q - sin(phi_hat_gyro) * r;

    // get angles from gyro 
    phi_hat_gyro = phi_hat_gyro + dt * phi_dot_gyro;
    theta_hat_gyro = theta_hat_gyro + dt * theta_dot_gyro;

    // Update complementary filter
    phi_hat = (1 - alpha) * (phi_hat + dt * phi_dot) + alpha * phi_hat_acc;
    theta_hat = (1 - alpha) * (theta_hat + dt * theta_dot) + alpha * theta_hat_acc;

    // Update complementary filter
    phi_hat2 = (1 - alpha2) * (phi_hat + dt * phi_dot) + alpha2 * phi_hat_acc;
    theta_hat2 = (1 - alpha2) * (theta_hat + dt * theta_dot) + alpha2 * theta_hat_acc;

    // Update complementary filter
    phi_hat3 = (1 - alpha3) * (phi_hat + dt * phi_dot) + alpha3 * phi_hat_acc;
    theta_hat3 = (1 - alpha3) * (theta_hat + dt * theta_dot) + alpha3 * theta_hat_acc;

     // from encoder count get angle:
     abs_encCount = abs(encCount);
     true_phi = ((abs_encCount * 360.0) / 2000.0);


    // get current time
    t = ((float) millis())/1000.0;
    //
 
    // Write to serial port
    writeSerialPort();


    // Update flag
    flag = 0;
  }
}

ISR(TIMER1_COMPA_vect) {
  flag = 1;
}

void getGyro_Data(void)
{
    imu.getRotation(&gx, &gy, &gz);
    p = (double) gx * 250 / 32768;  // p units:[deg/s]
    q = (double) gy * 250 / 32768;  // q
    r = (double) gz * 250 / 32768;  // r 
}

void getAccel_Data(void)
{
    imu.getAcceleration(&ax, &ay, &az);
    Axyz[0] = (double) ax ;  // in g 
    Axyz[1] = (double) ay ;
    Axyz[2] = (double) az ;
}

void getAccel_angles()
{
    getAccel_Data();
    phi_hat_acc = atan2(Axyz[1], sqrt(pow(Axyz[0],2) + pow(Axyz[2],2))) * 180/PI; // [deg]
    theta_hat_acc = atan2(-Axyz[0], sqrt(pow(Axyz[1],2) + pow(Axyz[2],2))) * 180/PI;
}

void getSensor_bias(void) {
  for (int i = 0; i <= N; i++ ) { // Collect 100 samples
    // For Gyro
    getGyro_Data();
    bias[0] += p;
    bias[1] += q;
    bias[2] += r;
    // For Accel
    getAccel_angles();
    bias[3] += phi_hat_acc;
    bias[4] += theta_hat_acc;
  }  
  bias[0] = bias[0] / N;
  bias[1] = bias[1] / N;
  bias[2] = bias[2] / N;
  bias[3] = bias[3] / N;
  bias[4] = bias[4] / N;
}

void writeSerialPort() {
  // comp filter data and time
  Serial.print(t);
  Serial.print(" ");
  Serial.print(true_phi); 
  Serial.print(" "); 
  Serial.print(phi_hat);
  Serial.print(" ");
  Serial.print(phi_hat2);
  Serial.print(" ");
  Serial.print(phi_hat3);
  Serial.print(" ");
  Serial.println();
  // accel roll and pitch estimates
  //Serial.print(phi_hat_acc); 
  //Serial.print(","); 
  //Serial.print(theta_hat_acc);
  //Serial.println(",");
   //  gyro roll and pitch estimates
  //Serial.print(phi_hat_gyro); 
  //Serial.print(","); 
  //Serial.println(theta_hat_gyro);
  
  
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

void pin1Change() 
{
  REGVALS = PIND;
  pin1state = REGVALS & B00000100;
  pin2state = REGVALS & B00001000;

  if (pin1state == B00000100) {
    if (pin2state == B00000000) {
      encCount++;
    } 
    else {
      encCount--;
    }
  }
  else {
    if (pin2state == B00001000) {
      encCount++;
    } 
    else {
      encCount--;
    }    
  }
}

void pin2Change() {
  REGVALS = PIND;
  pin1state = REGVALS & B00000100;
  pin2state = REGVALS & B00001000;

  if (pin1state == B00000100) {
    if (pin2state != B00000000) {
      encCount++;
    } 
    else {
      encCount--;
    }
  }
  else {
    if (pin2state != B00001000) {
      encCount++;
    } 
    else {
      encCount--;
    }    
  }
}
