# PID Tuning Guide for Two-Wheel Self-Balancing Robot

A self-balancing robot operates as an **inverted pendulum**. It requires continuous feedback from the MPU-6050 sensor to calculate how fast and in which direction to spin the wheels so that the robot stays upright.

---

## 1. Understanding the Variables

In this project, the control loop uses the standard Arduino PID library (`PID_v1`):

$$ \text{Output} = K_p \cdot e(t) + K_i \int e(t)\,dt + K_d \cdot \frac{de(t)}{dt} $$

Where:
* **Error $e(t)$** = $\text{Setpoint} - \text{Current Angle}$
* **$K_p$ (Proportional Gain):** Determines the primary restorative force. Higher values make the robot react stronger to tilting.
* **$K_i$ (Integral Gain):** Corrects steady-state offset and accumulated error (e.g., if the robot drifts slowly in one direction).
* **$K_d$ (Derivative Gain):** Provides damping against overshoot. It acts like a shock absorber, counteracting sudden fast velocity changes.

### Proven Working Values in this Repository:
* **Setpoint:** `175.5` degrees (Physical mechanical balance point)
* **$K_p$:** `51.0`
* **$K_i$:** `200.0`
* **$K_d$:** `2.8`
* **Sample Time:** `10 ms` (100 Hz refresh loop matching DMP interrupt)

---

## 2. Step-by-Step Tuning Procedure

Because every DIY chassis has slight differences in weight distribution, wheel diameter, and battery position, follow these steps if you need to fine-tune:

### Step 1: Find the Equilibrium Setpoint
1. Power the Arduino while holding the robot completely upright at the point where it neither falls forward nor backward.
2. Read the `Angle` value from the Serial Monitor (`115200` baud).
3. Set `originalSetpoint` in the code to this exact value (e.g., `175.5`).

### Step 2: Set $K_i = 0$ and $K_d = 0$
1. Start with only $K_p$. Set $K_p \approx 20$ to $30$.
2. Gradually increase $K_p$ until the robot starts oscillating back and forth (hunting for balance) without instantly collapsing.

### Step 3: Add Derivative Gain ($K_d$)
1. Once the robot oscillates rapidly, slowly increase $K_d$ (starting from $1.0$ up to $3.5$).
2. $K_d$ will dampen the oscillations and make the robot feel stiffer and more stable when you push it gently.

### Step 4: Add Integral Gain ($K_i$)
1. If the robot stays upright but slowly drifts in one direction or cannot stand completely still on carpet vs tile, increase $K_i$ (from $50$ to $250$).
2. $K_i$ removes the persistent tilt angle.

---

## 3. Practical Tips for BO Motors
* BO motors have a mechanical **deadzone** (they do not move below a certain minimum PWM value). The `MIN_ABS_SPEED = 15` in the code ensures that the motor receives enough starting torque.
* If one motor turns slightly faster than the other, use `motorSpeedFactorLeft` and `motorSpeedFactorRight` (default: `0.6`) to balance straight-line stability.
