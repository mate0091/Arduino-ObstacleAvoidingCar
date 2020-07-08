#include "Motor.h"
#include <Wire.h>
#include <MPU6050.h>
#include <Servo.h>
#include <TimerOne.h>

bool msg_in_end = false;

int accY, accX;
int zAxis = 0;

enum State {GOING, STOPPED, SCANNING, LEFT, RIGHT, FULL};

String inputString = "";
State currentState, prevState;

Motor m1, m2;
MPU6050 gyro;

int moveL, moveR;
float error;
unsigned long timer = 0;
float moveAmount = 0.0f;
float targetAngle = 0.0f;

float yaw = 0;
float tempYaw = 0;
float deltaYaw = 0;
const int adj_increment = 15;
float timeStep = 0.01f;

//ultrasonic
const int trig = A0;
const int echo = A1;

float targetZ = 0.0f;
int servoAngle = 85;

volatile bool is_avoiding = false;
int ledState = LOW;

Servo serv;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  m1.attach(6, 5);
  m2.attach(3, 11);

  currentState = STOPPED;

  while(!gyro.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G)){}

  while(!Serial){}

  gyro.calibrateGyro();
  gyro.setThreshold(3);

  inputString.reserve(200);

  moveL = 255;
  moveR = 255;
  error = 0;

  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  serv.attach(9);
  serv.write(85);

  delay(500);

  serv.detach();
  
  timer = millis();
}

void loop() 
{
  zAxis = readGyro();

  if((millis() - timer) >= timeStep * 1000)
  {
    yaw += zAxis * timeStep;
    timer = millis();
  }
    
  processInputs();
  processState();
}

void serialEvent()
{
  while(Serial.available() > 0)
  {
    char inChar = (char)Serial.read();

    inputString += inChar;

    if(inChar == ';') msg_in_end = true;
  }
}

void processInputs()
{
  if(msg_in_end)
  {
    int ind = inputString.lastIndexOf(',');
    int semicolon = inputString.lastIndexOf(';');
    accX = inputString.substring(0, ind).toInt();
    accY = inputString.substring(ind + 1, semicolon).toInt();
    inputString = "";
    msg_in_end = false;      
  }
}

void blinkLed()
{
  if(is_avoiding == true)
  {
    if(ledState == LOW)
    {
      ledState = HIGH;
    }
  
    else
    {
      ledState = LOW;
    }
  
    digitalWrite(LED_BUILTIN, ledState);
  }
}

void processState()
{  
  if(currentState == STOPPED)
  {
      if(!is_avoiding) digitalWrite(LED_BUILTIN, HIGH);
      stopMotors();
      //start conditions trigger move forward
      if(metObstacle(15.0f)) currentState = SCANNING;
      
      else if(accY < -40)
      {
        currentState = GOING;
      }
  }

  else if(currentState == GOING)
  {
      if(!is_avoiding) digitalWrite(LED_BUILTIN, LOW);
      if(accX <= 40 && accX >= -40) accX = 0;
      if(accY >= -40) accY = 0;

      //targetAngle = map(accX, -100, 100, -70, 70);
      targetAngle = accX;

      adjust_motors(zAxis, targetAngle);

      moveMotors(moveL, moveR);
      
      if(accY == 0 || metObstacle(15.0f))
      {
        currentState = STOPPED;
      }   
  }

  else if(currentState == SCANNING)
  {
      is_avoiding = true;
      //look both ways with the servo and check for distances
      serv.attach(9);
      serv.write(0);

      delay(1000);

      float distRight = calculateDistance();

      serv.write(180);
      delay(1000);

      float distLeft = calculateDistance();

      serv.write(85);
      delay(1000);

      serv.detach();

      if(distLeft <= 15.0f && distRight <= 15.0f) 
      {
        currentState = FULL;
      }

      else if(distRight == -1) 
      {
        currentState = RIGHT;
      }
      else if(distLeft == -1) 
      {
        currentState = LEFT;
      }

      else
      {
        if(distLeft < distRight)
        {
          currentState = LEFT;
        }

        else
        {
          currentState = RIGHT;
        }
      }

      tempYaw = yaw;

      Serial.end(); 
  }

  else if(currentState == LEFT)
  {
      moveMotors(0, 255);

      if(abs(tempYaw - yaw) >= 40.0f)
      {
        tempYaw = 0;
        //resume
        currentState = GOING;
        is_avoiding = false;

        Serial.begin(9600);
      }   
  }

  else if(currentState == RIGHT)
  {
      moveMotors(255, 0);
      
      if(abs(tempYaw - yaw) >= 40.0f)
      {
        tempYaw = 0;
        currentState = GOING;

        is_avoiding = false;
        Serial.begin(9600);
      }
  }

  else if(currentState == FULL)
  {
      moveMotors(0, 255);

      if(abs(tempYaw - yaw) >= 80.0f)
      {
        tempYaw = 0;
        currentState = GOING;

        is_avoiding = false;
        Serial.begin(9600);
      }
  }
}

void adjust_motors(float gyro, float target)
{
  //right error = +, left = -
  error = gyro - target;

  if(error > 5)
  {
    if(moveR < 255 - adj_increment) moveR += adj_increment * abs(error);
    
    else if(moveL > adj_increment + 110) moveL -= adj_increment * abs(error);
  }

  else if(error < -5)
  {
    if(moveL < 255 - adj_increment) moveL += adj_increment * abs(error);
    
    else if(moveR > adj_increment + 110) moveR -= adj_increment * abs(error);
  }

  moveL = constrain(moveL, 110, 255);
  moveR = constrain(moveR, 110, 255);
}

int readGyro()
{
  return gyro.readNormalizeGyro().ZAxis;
}

int calculateDistance()
{
  float duration, distance;
  
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  duration = pulseIn(echo, HIGH);
  distance = (duration / 2) * 0.0343f;

  if(distance >= 400 || distance <= 2)
  {
    distance = -1;
  }

  return distance;
}

bool metObstacle(float range)
{
  if (calculateDistance == -1) return false;
  return (calculateDistance() < range);
}

void stopMotors()
{
  m1.stop();
  m2.stop();
}

void moveMotors(int val1, int val2)
{
  m1.move(val1);
  m2.move(val2);
}
