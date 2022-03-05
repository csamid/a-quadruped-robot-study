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
float Gxyz[3];

#define COMPARE_MATCH 0x30D4 // 12500, 5Hz = 200ms
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

}

void loop() {
   if (flag) {
    getGyro_Data();
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
    Gxyz[0] = (double) gx * 250 / 32768;
    Gxyz[1] = (double) gy * 250 / 32768;
    Gxyz[2] = (double) gz * 250 / 32768;
}

void writeSerialPort() {
  /*
  char gx_tx[8];
  char gy_tx[8];
  char gz_tx[8];
  //char pitch_tx[10];
  //char roll_tx[10];
  char text[200];
  dtostrf(Gxyz[0], 6, 2, gx_tx);
  dtostrf(Gxyz[1], 6, 2, gy_tx);
  dtostrf(Gxyz[2], 6, 2, gz_tx);
  //dtostrf(pitch, 8, 2, pitch_tx);
  //dtostrf(roll, 8, 2, roll_tx);
  
  snprintf(text,200,"%s,%s,%s", gx_tx, gy_tx, gz_tx);
  Serial.println(text);
  */
  Serial.println(Gxyz[0]);

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
