// practicing c++/arduino basics
 #include "math.h"
 #include "Servo.h"

int sw = 1;
int luckyNums[] = {7,11,1,4,25,6,12};

//2d identity matrix
int eye[2][2] = {{1,0},{0,1}};

// 3x2 matrix (Array)
//int matrix[3][2] = {{1,2},{3,4},{5,6}};

//pointers
int my_age = 22;
int *pAge;

// Created a class called Book
class Book {
  public: 
    Book(String title,String author,int pages) {
    }
};

//create an instance of a book (from class Book) using the constructor
//Book book1;
Book book1("Vinland Saga","Makoto Yukimura",470);

class Movie {
  public:
  String title;
  String director;
  String rating;
};

Movie avengers;

//Servo test
Servo myServo1;
#define servoPin1 9
float rot180(Servo servo_number);
int min_pw = 550;
int max_pw = 2475;
int angle = 0;

void setup() {
  Serial.begin(9600);

  //String a = test();
  Serial.print(test());
  Serial.println(F("Enter your age: "));

  avengers.title = "Not a Movie";
  avengers.director = "Joe Brandon";
  avengers.rating = "PG";

  //Servo setup
  myServo1.write(min_pw);
  myServo1.attach(servoPin1,min_pw,max_pw); 
}

void loop() {
  while (sw) {
    //Function asking for user input twice
    UserInputTwice();


    ///Replace a element?
    luckyNums[0] = 8;
    //Add an element?
    luckyNums[7] = 7;
    Serial.println(luckyNums[7]);
    Serial.println(power(2,3));

    //Iterate through array
    iterateArray();

    //pointer example
    pAge = &my_age;
    Serial.print(F("Address of my_age = 0x"));
    Serial.println((int)&my_age, HEX);
    Serial.print(F("Address of pointer = 0x"));
    Serial.println((int)pAge, HEX);

    // print from object: avengers
    Serial.println(avengers.director);

    // deg to Microseconds for servos with min_pw = 550, max_pw = 2475
    Serial.println(round(degtoMicroS(45)));

    //Servo
      for (angle = 0; angle <= 180; angle += 1) {
        float a = degtoMicroS((float)angle);
        myServo1.writeMicroseconds(a);
        Serial.println(a);
      }
      delay(2000);
      for(int i = 180; i >= 0; i--) {
        float b = degtoMicroS((float)i);
        myServo1.writeMicroseconds(b);
        Serial.println(b);
      }
    sw = 0;
  }
}

//function 
String test() {
  String s = "Hello World!\n";
  return s;
}

float mathFuns() {
  float f1 = pow(2,5);
  return f1; 
}

int UserInputTwice() {
     while (Serial.available() == 0) {
  }

  int age = Serial.parseInt();

    Serial.read();
    Serial.print(F("The user age is: "));
    Serial.println(age);
    Serial.println(F("Enter the current year: "));
    Serial.flush();

       while (Serial.available() == 0) {
  }
  int curr_year = Serial.parseInt();

  int year_born = curr_year - age;

  Serial.print(F("You were born in: "));
  Serial.print(year_born); Serial.print(F(" or ")); Serial.println(year_born-1);

}

int power(int baseNum, int powNum) {
  int result = 1;
  if(powNum == 0) {
    result = result;
  }
  else {
    for(int i = 0; i<powNum; i++) {
      result *= baseNum;
    }
  }
  return result;
}

int iterateArray() {
  int matrix[3][2] = {{1,2},{3,4},{5,6}};
  for(int i = 0; i < 3; i++) {
    for(int j = 0; j < 2; j++) {
      Serial.println(matrix[i][j]);
    }
  }
}


float degtoMicroS(float degree) {
  float MicroS = 550.0 + degree*(1925.0/180.0);
  return MicroS;
}

float rot180(Servo servo_number) {
  for (angle = 0; angle <= 180; angle += 1) {
    
    //servo_number.writeMicroseconds(degtoMicroS((float)angle));
    
    delay(5);
    return degtoMicroS((float)angle);
  }
}
