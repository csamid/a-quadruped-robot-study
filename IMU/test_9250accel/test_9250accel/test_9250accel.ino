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
float Axyz[3];
float pitch;
float roll;


void setup() {
  // join I2C bus (I2Cdev library doesn't do this automatically)
    Wire.begin();

    // initialize serial communication
    // (38400 chosen because it works as well at 8MHz as it does at 16MHz, but
    // it's really up to you depending on your project)
    Serial.begin(38400);

    // initialize device
    Serial.println("Initializing I2C devices...");
    imu.initialize();
    

    // verify connection
    Serial.println("Testing device connections...");
    Serial.println(imu.testConnection() ? "MPU9250 connection successful" : "MPU9250 connection failed");

    delay(1000);
    Serial.println("     ");

  
}

void loop() {
  getAccel_Data();
  getRollPitch();
  writeSerialPort();

}

void getAccel_Data(void)
{
    imu.getAcceleration(&ax, &ay, &az);
    Axyz[0] = (double) ax / 16384;
    Axyz[1] = (double) ay / 16384;
    Axyz[2] = (double) az / 16384;
}

void writeSerialPort() {
  char ax_tx[8];
  char ay_tx[8];
  char az_tx[8];
  char pitch_tx[10];
  char roll_tx[10];
  char text[200];
  dtostrf(Axyz[0], 6, 2, ax_tx);
  dtostrf(Axyz[1], 6, 2, ay_tx);
  dtostrf(Axyz[2], 6, 2, az_tx);
  dtostrf(pitch, 8, 2, pitch_tx);
  dtostrf(roll, 8, 2, roll_tx);
  
  snprintf(text,200,"%s,%s,%s,%s,%s", ax_tx, ay_tx, az_tx, pitch_tx, roll_tx);
  Serial.println(text);

}

void getRollPitch() {
  pitch = atan2(Axyz[0],Axyz[2]) * 180/PI;
  roll = atan2(Axyz[1],Axyz[2]) * 180/PI;
}
