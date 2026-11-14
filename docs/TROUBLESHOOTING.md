# Troubleshooting & FAQ

Here are the most common issues encountered when building this two-wheeled self-balancing robot and how to resolve them.

---

### 1. The robot runs away at top speed and falls flat
* **Root Cause:** Inverted motor polarity or reverse angle calculation.
* **Fix:** When the robot tilts forward, the wheels must accelerate **forward** to move underneath the center of mass. If your wheels turn backward instead:
  * Swap the two motor wire pairs on the L298N terminals (`OUT1`/`OUT2` and `OUT3`/`OUT4`), OR
  * In the sketch, swap the pin assignments (e.g., swap `IN1` with `IN2`, and `IN3` with `IN4`).

---

### 2. "DMP Initialization failed. Code: 1" (or Code: 2) in Serial Monitor
* **Root Cause:** I2C communication issue between Arduino Nano and MPU-6050.
* **Fix:**
  * Check that SDA is firmly connected to **A4** and SCL is firmly connected to **A5**.
  * Make sure MPU-6050 `VCC` is supplied with 5V or 3.3V and common `GND` is connected.
  * Check that `INT` pin on the MPU-6050 is connected to **D2** on the Nano.

---

### 3. FIFO Buffer Overflow errors or stuttering motion
* **Root Cause:** The `loop()` is taking too long to read packets, or Serial output is blocking the processor.
* **Fix:**
  * Keep Serial printing interval at ≥ 100 ms (already implemented in this code).
  * Ensure the Serial baud rate is set to `115200` baud in `Serial.begin()` and in your Serial Monitor.
  * The code includes automatic FIFO clearing:
    ```cpp
    if ((mpuIntStatus & 0x10) || fifoCount >= 1024) {
        mpu.resetFIFO();
        return;
    }
    ```

---

### 4. Robot violently twitches or shakes
* **Root Cause:** **Kp** or **Kd** is set too high, or the MPU-6050 is loosely mounted.
* **Fix:**
  * Securely fasten the sensor to the chassis using double-sided foam tape or screws.
  * Lower **Kp** slightly (e.g., reduce from 51 to 40) or lower **Kd**.

---

### 5. Robot balances for 10-20 seconds and then resets/falls
* **Root Cause:** Battery voltage sag caused by motor current spikes.
* **Fix:**
  * Ensure your two 18650 Li-ion cells are fully charged (around 8.2V - 8.4V total).
  * Ensure common ground between Arduino and L298N is solid.
  * Optionally, solder a 100µF – 470µF electrolytic capacitor across the L298N power input terminals (12V and GND) to absorb voltage dips.
