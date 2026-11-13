#ifndef LMotorController_h
#define LMotorController_h

#include "Arduino.h"

/**
 * @brief L298N Dual H-Bridge Motor Controller Helper Library
 * Designed specifically for two-wheel self-balancing robots.
 */
class LMotorController
{
protected:
    int _ena, _in1, _in2, _enb, _in3, _in4;
    int _currentSpeed;
    double _motorSpeedFactorLeft, _motorSpeedFactorRight;

public:
    LMotorController(
        int ena, int in1, int in2, 
        int enb, int in3, int in4, 
        double motorSpeedFactorLeft, 
        double motorSpeedFactorRight
    );

    void move(int leftSpeed, int rightSpeed, int minAbsSpeed);
    void move(int speed, int minAbsSpeed);
    void move(int speed);
    void turnLeft(int speed, bool kick);
    void turnRight(int speed, bool kick);
    void stopMoving();
};

#endif
