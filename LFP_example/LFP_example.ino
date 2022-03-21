float xn1 = 0;
float yn1 = 0;

int k = 0;


void setup() {
  Serial.begin(115200);

}

void loop() {
  // Test signal
  float t = micros()/1.0e6;
  float xn = sin(2*PI*2*t) + 0.2*sin(2*PI*25*t);
  

  // Compute the filtered signal (1st low-pass filter)
  float yn =  0.969*yn1 + 0.0155*xn + 0.0155*xn1;

  delay(1); // 1 ms so that loop updates at 1 kHz
  //Store the values
  xn1 = xn;
  yn1 = yn;

  if (k % 3 == 0) { // if k is even the print to serial 
    // Output
    Serial.print(2*xn);
    Serial.print(" ");
    Serial.println(2*yn);
  }
  k = k + 1;
}
