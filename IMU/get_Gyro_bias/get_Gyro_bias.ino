#include "Wire.h"

// I2Cdev and MPU9250 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "I2Cdev.h"
#include "MPU9250.h"
#include "math.h"

// class default I2C address is 0x68
MPU9250 imu;
I2Cdev   I2C_M;

uint8_t buffer_m[6];


int16_t ax, ay, az;
int16_t gx, gy, gz;
int16_t mx, my, mz;
float Axyz[3];
float p, q, r; // gyro body angular rates 
float N = 100;
double bias[5]; // 0 through 2 is for Gyro, 3 through 4 is for Accel
double phi_hat_acc; // roll accel estimate
double theta_hat_acc; // pitch accel estimate
double phi_hat = 0.0; // roll estimate 
double theta_hat = 0.0; // pitch estimate
static double alpha = 0.2; // Filter coefficient
double phi_dot; // gyro fixed frame rate
double theta_dot; // gyro fixed frame rate

#define COMPARE_MATCH 0x30D4 // 12500, 5Hz = 200ms
static double dt = 0.20; // 0.2s = 200ms
volatile uint8_t flag = 0;


void setup() {
  // join I2C bus (I2Cdev library doesn't do this automatically)
    Wire.begin();

    // initialize serial communication
    Serial.begin(38400);

    // initialize device
    Serial.println("Initializing I2C devices...");
    imu.initialize();
    
    // verify connection
    Serial.println("Testing device connections...");
    Serial.println(imu.testConnection() ? "MPU9250 connection successful" : "MPU9250 connection failed");

    // Timer1 setup
    timerInterruptSetup();

    // Calculate accel and gyro bias 
    getSensor_bias();

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

    // Update complementary filter
    phi_hat = (1 - alpha) * (phi_hat + dt * phi_dot) + alpha * phi_hat_acc;
    theta_hat = (1 - alpha) * (theta_hat + dt * theta_dot) + alpha * theta_hat_acc;

    // Write to serial port
    writeSerialPort();

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
    Axyz[0] = (double) ax / 16384;  // in g 
    Axyz[1] = (double) ay / 16384;
    Axyz[2] = (double) az / 16384;
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
  
  Serial.print(phi_hat); 
  Serial.print(","); 
  Serial.println(theta_hat); 

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
