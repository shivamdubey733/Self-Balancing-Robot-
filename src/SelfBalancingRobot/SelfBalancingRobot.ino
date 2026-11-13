#include <PID_v1.h>
#include <LMotorController.h>
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"

#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
#include "Wire.h"
#endif

#define MIN_ABS_SPEED 15

// =========================================================================
// MPU6050 Configuration
// =========================================================================
MPU6050 mpu;

bool dmpReady = false;
uint8_t mpuIntStatus;
uint8_t devStatus;
uint16_t packetSize;
uint16_t fifoCount;
uint8_t fifoBuffer[64];

Quaternion q;
VectorFloat gravity;
float ypr[3];

volatile bool mpuInterrupt = false;

// =========================================================================
// PID Controller Settings
// =========================================================================
// Setpoint: The angle (in degrees) at which the robot is in vertical equilibrium
double originalSetpoint = 175.5;
double setpoint = originalSetpoint;

double input = 0;
double output = 0;

// PID Tuning Constants (adjust according to your robot's weight and center of gravity)
double Kp = 51.0;
double Ki = 200.0;
double Kd = 2.8;

PID pid(&input, &output, &setpoint, Kp, Ki, Kd, DIRECT);

// =========================================================================
// Motor Driver Settings (L298N)
// =========================================================================
// Balance motors speed factor (compensate for minor motor speed differences)
double motorSpeedFactorLeft = 0.6;
double motorSpeedFactorRight = 0.6;

int ENA = 5;   // PWM pin for Left Motor speed control
int IN1 = 6;   // Left Motor direction control 1
int IN2 = 7;   // Left Motor direction control 2
int IN3 = 8;   // Right Motor direction control 1
int IN4 = 9;   // Right Motor direction control 2
int ENB = 10;  // PWM pin for Right Motor speed control

LMotorController motorController(
  ENA, IN1, IN2, ENB, IN3, IN4,
  motorSpeedFactorLeft, motorSpeedFactorRight
);

// Serial printing timer
unsigned long lastPrintTime = 0;

void dmpDataReady()
{
  mpuInterrupt = true;
}

void setup()
{
  Serial.begin(115200);

  #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    Wire.begin();
    TWBR = 24; // 400kHz I2C clock (fast mode)
  #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
    Fastwire::setup(400, true);
  #endif

  Serial.println(F("Initializing MPU6050 DMP..."));

  mpu.initialize();

  devStatus = mpu.dmpInitialize();

  // Sensor Calibration Offsets (customize for your specific MPU6050 module)
  mpu.setXGyroOffset(220);
  mpu.setYGyroOffset(76);
  mpu.setZGyroOffset(-85);
  mpu.setZAccelOffset(1788);

  if (devStatus == 0)
  {
    mpu.setDMPEnabled(true);

    // Attach interrupt to Arduino Nano Pin D2 (INT0)
    attachInterrupt(
      digitalPinToInterrupt(2),
      dmpDataReady,
      RISING
    );

    mpuIntStatus = mpu.getIntStatus();

    dmpReady = true;
    packetSize = mpu.dmpGetFIFOPacketSize();

    // Configure PID parameters
    pid.SetMode(AUTOMATIC);
    pid.SetSampleTime(10);           // 10ms loop time
    pid.SetOutputLimits(-255, 255);  // PWM output range for motor driver

    Serial.println(F("MPU6050 DMP Initialized successfully!"));
    Serial.println(F("Angle | Setpoint | PID Output"));
  }
  else
  {
    Serial.print(F("DMP Initialization failed. Code: "));
    Serial.println(devStatus);
  }
}

void loop()
{
  if (!dmpReady)
    return;

  if (!mpuInterrupt)
    return;

  // Clear interrupt flag safely
  noInterrupts();
  mpuInterrupt = false;
  interrupts();

  mpuIntStatus = mpu.getIntStatus();
  fifoCount = mpu.getFIFOCount();

  // Handle FIFO buffer overflow
  if ((mpuIntStatus & 0x10) || fifoCount >= 1024)
  {
    mpu.resetFIFO();
    return;
  }

  if (mpuIntStatus & 0x02)
  {
    // Wait until at least one complete packet is available
    while (fifoCount < packetSize)
    {
      fifoCount = mpu.getFIFOCount();
    }

    // Read the latest packet from FIFO
    while (fifoCount >= packetSize)
    {
      mpu.getFIFOBytes(fifoBuffer, packetSize);
      fifoCount -= packetSize;
    }

    // Compute pitch/roll angles from Quaternion
    mpu.dmpGetQuaternion(&q, fifoBuffer);
    mpu.dmpGetGravity(&gravity, &q);
    mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);

    // Convert pitch to degrees and shift range to positive values
    input = ypr[1] * 180 / M_PI + 180;

    // Compute PID response
    pid.Compute();

    // Drive motors to restore balance
    motorController.move(output, MIN_ABS_SPEED);

    // Debug print every 100 milliseconds
    if (millis() - lastPrintTime >= 100)
    {
      lastPrintTime = millis();

      Serial.print("Angle: ");
      Serial.print(input, 2);

      Serial.print(" | Setpoint: ");
      Serial.print(setpoint, 2);

      Serial.print(" | Output: ");
      Serial.println(output, 2);
    }
  }
}
