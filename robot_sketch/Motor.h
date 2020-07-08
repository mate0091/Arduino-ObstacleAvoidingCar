#ifndef MOTOR_H
#define MOTOR_H

#include <Arduino.h>

class Motor
{
  private:
    int pin1, pin2;

  public:
    void move(int);
    void attach(int, int);
    void detach();
    void stop();
};

#endif MOTOR_H
