#include "Motor.h"

void Motor::attach(int p1, int p2)
{
  pin1 = p1;
  pin2 = p2;

  pinMode(pin1, OUTPUT);
  pinMode(pin2, OUTPUT);

  this->stop();
}

void Motor::move(int speed)
{
  if(speed != 0)
  {
    if(speed > 0)
    {
      analogWrite(pin1, speed);
      digitalWrite(pin2, LOW);
    }

    else
    {
      analogWrite(pin2, speed);
      digitalWrite(pin1, LOW);
    }
  }

  else stop();
}

void Motor::stop()
{
  digitalWrite(pin1, LOW);
  digitalWrite(pin2, LOW);
}

void Motor::detach()
{
  free(&pin1);
  free(&pin2);
}
