/*
  L298N.cpp - Libreria per gestire i movimenti di un motore DC con il modulo L298N
  Autore:   Andrea Lombardo
  Sito web: http://www.lombardoandrea.com  
*/

#include "L298N.h"

typedef void (*CallBackFunction)();

L298N::L298N(uint8_t pinEnable, uint8_t pinIN1, uint8_t pinIN2)
{
  _pinEnable = pinEnable;
  _pinIN1 = pinIN1;
  _pinIN2 = pinIN2;
  _pwmVal = 100;
  _isMoving = false;
  _canMove = true;
  _lastMs = 0;
  _direction = STOP;

  this->pinMode(_pinEnable, OUTPUT);
  this->pinMode(_pinIN1, OUTPUT);
  this->pinMode(_pinIN2, OUTPUT);
}

L298N::L298N(uint8_t pinIN1, uint8_t pinIN2)
{
  _pinEnable = -1;
  _pinIN1 = pinIN1;
  _pinIN2 = pinIN2;
  _pwmVal = 255; // It's always at the max speed due to jumper on module
  _isMoving = false;
  _canMove = true;
  _lastMs = 0;
  _direction = STOP;

  this->pinMode(_pinIN1, OUTPUT);
  this->pinMode(_pinIN2, OUTPUT);
}

void L298N::setSpeed(unsigned short pwmVal)
{
  _pwmVal = pwmVal;
}

unsigned short L298N::getSpeed()
{
  return _pwmVal;
}

void L298N::forward()
{
  this->digitalWrite(_pinIN1, HIGH);
  this->digitalWrite(_pinIN2, LOW);

  this->analogWrite(_pinEnable, _pwmVal);

  _direction = FORWARD;
  _isMoving = true;
}

void L298N::backward()
{
  this->digitalWrite(_pinIN1, LOW);
  this->digitalWrite(_pinIN2, HIGH);

  this->analogWrite(_pinEnable, _pwmVal);

  _direction = BACKWARD;
  _isMoving = true;
}

void L298N::run(L298N::Direction direction)
{
  switch (direction)
  {
  case BACKWARD:
    this->backward();
    break;
  case FORWARD:
    this->forward();
    break;
  case STOP:
    this->stop();
    break;
  }
}

//Timing and callback

void L298N::runFor(unsigned long delay, L298N::Direction direction, CallBackFunction callback)
{

  if ((_lastMs == 0) && _canMove)
  {
    _lastMs = millis();

    switch (direction)
    {
    case FORWARD:
      this->forward();
      break;
    case BACKWARD:
      this->backward();
      break;
    case STOP:
    default:
      this->stop();
      break;
    }
  }

  if (((millis() - _lastMs) > delay) && _canMove)
  {
    this->stop();
    _lastMs = 0;
    _canMove = false;

    callback();
  }
}

void L298N::runFor(unsigned long delay, L298N::Direction direction)
{
  this->runFor(delay, direction, fakeCallback);
}

void L298N::forwardFor(unsigned long delay, CallBackFunction callback)
{
  this->runFor(delay, FORWARD, callback);
}

void L298N::forwardFor(unsigned long delay)
{
  this->runFor(delay, FORWARD);
}

void L298N::backwardFor(unsigned long delay, CallBackFunction callback)
{
  this->runFor(delay, BACKWARD, callback);
}

void L298N::backwardFor(unsigned long delay)
{
  this->runFor(delay, BACKWARD);
}

void L298N::stop()
{
  this->digitalWrite(_pinIN1, LOW);
  this->digitalWrite(_pinIN2, LOW);

  this->analogWrite(_pinEnable, 255);

  _direction = STOP;
  _isMoving = false;
}

void L298N::reset()
{
  _canMove = true;
}

boolean L298N::isMoving()
{
  return _isMoving;
}

L298N::Direction L298N::getDirection()
{
  return _direction;
}

void L298N::pinMode(uint8_t pin, uint8_t mode, bool digitalPin)
{
  (void) digitalPin;
  pinMode(pin, mode);
}

void L298N::analogWrite(uint8_t pin, int value)
{
  analogWrite(pin, value);
}

void L298N::digitalWrite(uint8_t pin, uint8_t value)
{
  digitalWrite(pin, value);
}

void L298N::fakeCallback()
{
}