#include "LMotorController.h"

LMotorController::LMotorController(
    int ena, int in1, int in2, 
    int enb, int in3, int in4, 
    double motorSpeedFactorLeft, 
    double motorSpeedFactorRight
) {
    _ena = ena;
    _in1 = in1;
    _in2 = in2;
    _enb = enb;
    _in3 = in3;
    _in4 = in4;
    _currentSpeed = 0;
    _motorSpeedFactorLeft = motorSpeedFactorLeft;
    _motorSpeedFactorRight = motorSpeedFactorRight;

    pinMode(_ena, OUTPUT);
    pinMode(_in1, OUTPUT);
    pinMode(_in2, OUTPUT);
    pinMode(_enb, OUTPUT);
    pinMode(_in3, OUTPUT);
    pinMode(_in4, OUTPUT);
}

void LMotorController::move(int leftSpeed, int rightSpeed, int minAbsSpeed)
{
    // Right Motor Processing
    if (rightSpeed < 0)
    {
        rightSpeed = min(rightSpeed, -1 * minAbsSpeed);
        rightSpeed = max(rightSpeed, -255);
    }
    else if (rightSpeed > 0)
    {
        rightSpeed = max(rightSpeed, minAbsSpeed);
        rightSpeed = min(rightSpeed, 255);
    }
    
    int realRightSpeed = (int)(rightSpeed * _motorSpeedFactorRight);

    if (realRightSpeed > 0)
    {
        digitalWrite(_in3, HIGH);
        digitalWrite(_in4, LOW);
        analogWrite(_enb, realRightSpeed);
    }
    else if (realRightSpeed < 0)
    {
        digitalWrite(_in3, LOW);
        digitalWrite(_in4, HIGH);
        analogWrite(_enb, abs(realRightSpeed));
    }
    else
    {
        digitalWrite(_in3, LOW);
        digitalWrite(_in4, LOW);
        analogWrite(_enb, 0);
    }

    // Left Motor Processing
    if (leftSpeed < 0)
    {
        leftSpeed = min(leftSpeed, -1 * minAbsSpeed);
        leftSpeed = max(leftSpeed, -255);
    }
    else if (leftSpeed > 0)
    {
        leftSpeed = max(leftSpeed, minAbsSpeed);
        leftSpeed = min(leftSpeed, 255);
    }
    
    int realLeftSpeed = (int)(leftSpeed * _motorSpeedFactorLeft);

    if (realLeftSpeed > 0)
    {
        digitalWrite(_in1, HIGH);
        digitalWrite(_in2, LOW);
        analogWrite(_ena, realLeftSpeed);
    }
    else if (realLeftSpeed < 0)
    {
        digitalWrite(_in1, LOW);
        digitalWrite(_in2, HIGH);
        analogWrite(_ena, abs(realLeftSpeed));
    }
    else
    {
        digitalWrite(_in1, LOW);
        digitalWrite(_in2, LOW);
        analogWrite(_ena, 0);
    }
}

void LMotorController::move(int speed, int minAbsSpeed)
{
    int direction = 1;
    if (speed < 0)
    {
        direction = -1;
        speed = min(speed, -1 * minAbsSpeed);
        speed = max(speed, -255);
    }
    else
    {
        speed = max(speed, minAbsSpeed);
        speed = min(speed, 255);
    }
    
    if (speed == _currentSpeed) return;
    
    int realLeftSpeed = (int)(speed * _motorSpeedFactorLeft);
    int realRightSpeed = (int)(speed * _motorSpeedFactorRight);
    
    analogWrite(_ena, abs(realLeftSpeed));
    analogWrite(_enb, abs(realRightSpeed));
    
    if (direction > 0)
    {
        digitalWrite(_in1, HIGH);
        digitalWrite(_in2, LOW);
        digitalWrite(_in3, HIGH);
        digitalWrite(_in4, LOW);
    }
    else
    {
        digitalWrite(_in1, LOW);
        digitalWrite(_in2, HIGH);
        digitalWrite(_in3, LOW);
        digitalWrite(_in4, HIGH);
    }
    
    _currentSpeed = direction * (abs(realLeftSpeed) + abs(realRightSpeed)) / 2;
}

void LMotorController::move(int speed)
{
    if (speed == _currentSpeed) return;
    
    if (speed > 255) speed = 255;
    else if (speed < -255) speed = -255;
    
    int realLeftSpeed = (int)(speed * _motorSpeedFactorLeft);
    int realRightSpeed = (int)(speed * _motorSpeedFactorRight);
    
    analogWrite(_ena, abs(realLeftSpeed));
    analogWrite(_enb, abs(realRightSpeed));
    
    if (speed > 0)
    {
        digitalWrite(_in1, HIGH);
        digitalWrite(_in2, LOW);
        digitalWrite(_in3, HIGH);
        digitalWrite(_in4, LOW);
    }
    else
    {
        digitalWrite(_in1, LOW);
        digitalWrite(_in2, HIGH);
        digitalWrite(_in3, LOW);
        digitalWrite(_in4, HIGH);
    }
    
    _currentSpeed = speed;
}

void LMotorController::turnLeft(int speed, bool kick)
{
    digitalWrite(_in1, LOW);
    digitalWrite(_in2, HIGH);
    digitalWrite(_in3, HIGH);
    digitalWrite(_in4, LOW);
    analogWrite(_ena, abs((int)(speed * _motorSpeedFactorLeft)));
    analogWrite(_enb, abs((int)(speed * _motorSpeedFactorRight)));
}

void LMotorController::turnRight(int speed, bool kick)
{
    digitalWrite(_in1, HIGH);
    digitalWrite(_in2, LOW);
    digitalWrite(_in3, LOW);
    digitalWrite(_in4, HIGH);
    analogWrite(_ena, abs((int)(speed * _motorSpeedFactorLeft)));
    analogWrite(_enb, abs((int)(speed * _motorSpeedFactorRight)));
}

void LMotorController::stopMoving()
{
    digitalWrite(_in1, LOW);
    digitalWrite(_in2, LOW);
    digitalWrite(_in3, LOW);
    digitalWrite(_in4, LOW);
    analogWrite(_ena, 0);
    analogWrite(_enb, 0);
    _currentSpeed = 0;
}
