/************* Program variables ************/
int state = 1;
float duration, inches;       //ultrasonic reading
boolean is_boulder_green;
float boulder_length, boulder_height, boulder_area; 

/**************** Constants *****************/
const int markerNumber = 113; //update this when receive new marker
const float pi      = 3.14159;
const int repeatNo  = 5;      //for multiple ultrasonic read
const int interval  = 200;    //ms wait
const long upper    = 1200;   //filter out unwanted data in ultrasonic reading
const long lower    = 150;    //only apply when averaging multiple trials

/************** Pin Variables ***************/
int a = 1;  int b = 2; //Kaitlin, Maria, please verify the digital pin asssignments
int c = 3;  int d = 4;
int e = 5;  int f = 6;
int g = 7;  int h = 8;
int i = 9;  int j = 10;
int k = 11;
int photoresist_pin     = 0;
int ultrasound_top_pin  = 1;
int ultrasound_left_pin = 2;
int ultrasound_right_pin= 3;
int ultrasound_side_pin = 4;

/********* Fakebot testing pin var **********/
int ena = 5;  int enb = 3; 
int in1 = 6;  int in3 = 2;
int in2 = 7;  int in4 = 1;
int trig = 3;       //attach pin 3 to Trig
int echo = 4;       //attach pin 4 to Echo
int trig2 = 5;      //attach pin 5 to Trig on second sensor
int echo2 = 6;      //attach pin 6 to Echo on second sensor 
int green = 10;
int red = 11;

/********* Initiate RF GPS System ***********/
#include <SoftwareSerial.h>
#include "enes100.h"
SoftwareSerial mySerial(8, 9); //RX is pin 8, TX is pin 9
enes100::RfClient<SoftwareSerial> rf(&mySerial); 
enes100::Marker marker;

void setup() {
  RFSetup();
  motorSetup();
  singleSensorSetup();
  Serial.begin(9600);
  pinMode(green, OUTPUT);
  pinMode(red, OUTPUT);
}

void loop() {
  Serial.println("woohoo");
  RFLoop();
  switch(state) {
    case 0:
      break;
    case 1: //OSV is in landing zone
      Serial.println("yay");
      turnRight(0);
      driveForwardXDirection(1);
      state = 2;
      break;
    case 2: //OSV is at edge of landing zone
      boolean obstacle;
      obstacle = senseObstacle();
      //turnLeft(pi/2);
      if(obstacle == true){
        if(marker.y > 1){
          turnRight(-pi/2);
          driveForwardYDirection(1);
          turnLeft(0);
          break;
        } else {
          turnLeft(pi/2);
          driveForwardYDirection(1);
          turnRight(0);
          break;
        }
      } else {
        //turnRight(0);
        state = 3;
        break;
      }
    case 3://OSV can now move forward because no obstacles are in the way
      if (marker.y != 1) {
        if (marker.y > 1) {
          turnRight(-pi/2);
          driveForwardYDirection(1);
          turnLeft(0);
            
        } else {
          turnLeft(pi/2);
          driveForwardYDirection(1);
          turnRight(0);
        }
      }
      driveForwardXDirection(3.5);
      state = 4;
      break;
    case 4://OSV is now at the x value of the Terrain Site
      turnLeft(pi/2);
      driveForwardYDirection(1.5);
      turnRight(0);
      state = 0;
      break;  
  }

}

boolean senseObstacle() {
  boolean obstacle;
  if (inches != 0) {
    obstacle = true;
  } else {
    obstacle = false;
  }
}

void driveForwardXDirection(float xValue) {
  while (marker.x < xValue) {
    Serial.println("happy");
    motorStraight();
    RFLoop();
    digitalWrite(green, HIGH);
    digitalWrite(red, HIGH);
  }
}

void driveForwardYDirection(float yValue) {
  float tolerance = .05;
  while (marker.y - yValue < -(tolerance) || marker.y - yValue > tolerance) {
    motorStraight();
    RFLoop();
    digitalWrite(green, HIGH);
    digitalWrite(red, HIGH);
  }
}

void turnLeft(float orientation) {
  float tolerance = (pi/12);
  while (marker.theta - orientation < -(tolerance)) {
    motorTurnLeft();
    RFLoop();
    digitalWrite(green, LOW);
    digitalWrite(red, HIGH);
  }
}

void turnRight(float orientation) {
  float tolerance = (pi/12);
  while (marker.theta - orientation > tolerance) {
    Serial.println("sad");
    motorTurnRight();
    RFLoop();
    digitalWrite(green, HIGH);
    digitalWrite(red, LOW);
  }
}

void RFSetup() {
   mySerial.begin(9600); //this establishes serial communication with
                        //something other than serial monitor, in this
                        //case RF communication with mission control
  Serial.begin(9600); //this establishes regular serial communication
                      //through USB to student's serial monitor

  pinMode(8, INPUT); //since pin 8 is RX, it receives as an input
  pinMode(9, OUTPUT); //since pin 9 is TX, it transmits as an output
  
  rf.resetServer();
  rf.sendMessage("\nTeam PandaXpress is Connected\n"); //sent to mission control
  Serial.println("Team PandaXpress is Connected"); //sent to student's serial monitor
  delay(1000);
}

void RFLoop() {
    if(rf.receiveMarker(&marker, markerNumber)) //see if marker 104 is received
  {
    rf.sendMessage("\nMarker is successfully being read\n");
    Serial.print("x value of marker is ");
    Serial.println(marker.x);
    Serial.print("y value of marker is ");
    Serial.println(marker.y);
    Serial.print("the orientation is ");
    Serial.println(marker.theta);
    delay(500);
    rf.sendMessage("Panda Xpress Rocks");
  }
  else
  {
    rf.sendMessage("\nMarker is not registering\n");
    Serial.println("Marker is not registering");
    delay(500);
  }
}

void motorSetup() {
  pinMode(ena,OUTPUT);
  pinMode(in1,OUTPUT);
  pinMode(in2,OUTPUT);
  pinMode(enb,OUTPUT);
  pinMode(in3,OUTPUT);
  pinMode(in4,OUTPUT);
}

void motorStraight() {
  digitalWrite(in1,LOW);
  digitalWrite(in2,HIGH);
  analogWrite(ena,255);
  digitalWrite(in3,LOW);
  digitalWrite(in4,HIGH);
  analogWrite(enb,255);
}

void motorTurnRight() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  analogWrite(ena,255);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  analogWrite(enb,255);
}

void motorTurnLeft() {
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  analogWrite(ena,255);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  analogWrite(enb,255);
}

void singleSensorSetup() {
  pinMode(trig, OUTPUT);
  pinMode(A1,INPUT);
  pinMode(trig2, OUTPUT);
  pinMode(echo2, INPUT);
  Serial.begin(9600);
}

void singleSensorLoop() {
  duration = multiplePing(repeatNo,interval,lower,upper); //average multiple ping
  // convert the time into a distance
  inches = microsecondsToInches(duration);
  Serial.print(inches);
  Serial.print(" ");
  Serial.print("in");
  Serial.println();
}
float multiplePing(int trialNo, int delayTime, long lowerLimit, long upperLimit) {
  float duration = 0;
  int trialNum = 0;
  for(int i=0; i<trialNo; i++){
    long d = ultrasoundPing();
    if(d > lowerLimit && d < upperLimit){
        duration = duration + d;
        trialNum++;
    }
    delay(delayTime);
  }
  if(trialNum == 0){ return 0;
  }else{ return duration /1.0 / trialNum;
  }
}

long ultrasoundPing() {
  // Ping triggered by a HIGH pulse of 2 or more microseconds.
  // Short LOW pulse beforehand ensures a clean HIGH pulse:
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(5);
  digitalWrite(trig, LOW);
  
  // The same pin is used to read the signal from the PING))): a HIGH
  // pulse whose duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  return pulseIn(A1, HIGH);
}

float microsecondsToInches(long microseconds) {
  // According to Parallax's datasheet for the PING))), there are
  // 73.746 microseconds per inch (i.e. sound travels at 1130 feet per
  // second). This gives the distance travelled by the ping, outbound
  // and return, so we divide by 2 to get the distance of the obstacle.
  // See: http://www.parallax.com/dl/docs/prod/acc/28015-PI...
  return microseconds / 73.746 / 2.0;
}


