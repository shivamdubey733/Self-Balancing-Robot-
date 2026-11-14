# Complete Build Guide: From Requirements to Self-Balancing

This guide provides an end-to-end walkthrough for building the two-wheeled self-balancing robot from scratch, covering hardware and software prerequisites, mechanical fabrication, electrical wiring, calibration, and initial testing.

---

## 📋 Table of Contents
1. [Prerequisites & Requirements](#1-prerequisites--requirements)
   - [Hardware Requirements](#a-hardware-requirements)
   - [Software Requirements](#b-software-requirements)
   - [Tools & Consumables](#c-tools--consumables)
2. [Step 1: Mechanical Chassis Fabrication](#step-1-mechanical-chassis-fabrication)
3. [Step 2: Circuit Wiring & Power Connections](#step-2-circuit-wiring--power-connections)
4. [Step 3: Software Environment & Library Installation](#step-3-software-environment--library-installation)
5. [Step 4: MPU-6050 Calibration & Offset Calculation](#step-4-mpu-6050-calibration--offset-calculation)
6. [Step 5: Firmware Upload & Motor Direction Verification](#step-5-firmware-upload--motor-direction-verification)
7. [Step 6: PID Fine-Tuning & First Run](#step-6-pid-fine-tuning--first-run)

---

## 1. Prerequisites & Requirements

### A. Hardware Requirements
| Item | Details | Recommended Specs |
|---|---|---|
| **Microcontroller** | Arduino Nano | ATmega328P, 5V, 16MHz (Mini-USB or Type-C) |
| **Motion Sensor** | MPU-6050 Module | 3-axis accelerometer + 3-axis gyroscope with DMP |
| **Motor Driver** | L298N Dual H-Bridge | Handles 7V–12V DC, dual 2A outputs |
| **Motors** | 2x Dual-Shaft BO Motors | Gear ratio ~1:48, 3V–6V DC rated |
| **Wheels** | 2x Rubber Wheels | 65mm outer diameter, center bore matching BO motor shaft |
| **Batteries** | 2x 18650 Li-ion Cells | 3.7V nominal each (total 7.4V–8.4V, >=2000mAh) |
| **Battery Holder** | 2-Slot 18650 Holder | Series connection with red/black lead wires |
| **Breadboard** | Mini Breadboard | 170 tie-points (for seating the Arduino Nano) |
| **Jumper Wires** | Dupont Cables | Male-to-Male, Male-to-Female, Female-to-Female (~20 pcs) |
| **Chassis Plates** | 3 Flat Sheets | Acrylic, MDF, sunboard, or sturdy foam board (~15cm x 10cm) |
| **Standoffs** | 8x Spacers / Dowels | Wooden dowels or M3 nylon/brass standoffs (~40mm to 50mm) |

### B. Software Requirements
1. **Operating System:** Windows 10/11, macOS, or Linux.
2. **Arduino IDE:** Version 1.8.19 (Legacy) or Version 2.x ([Download Arduino IDE](https://www.arduino.cc/en/software)).
3. **USB Drivers:**
   * **CH340 Driver:** Required for most clone Arduino Nano boards ([Download CH340 Driver](https://sparks.gogo.co.nz/ch340.html)).
   * **FTDI Driver:** If using official or FT232-based Arduino Nano boards.
4. **Arduino Libraries (Pre-bundled in this repository):**
   * `PID_v1`: Controls the feedback loop calculation.
   * `I2Cdev`: Low-level I2C hardware abstraction.
   * `MPU6050`: MotionApps 2.0 DMP driver for on-chip sensor fusion.
   * `LMotorController`: Dedicated H-bridge motor driver interface.

### C. Tools & Consumables
* Glue gun or M3 machine screws with nuts.
* Wire stripper and small Phillips screwdriver.
* Soldering iron (for soldering leads to BO motor terminals, if not pre-soldered).
* Double-sided foam tape (for mounting the MPU-6050 and battery holder securely).

---

## Step 1: Mechanical Chassis Fabrication

A 3-tier vertical tower design is optimal for two-wheeled self-balancing robots because it provides an elevated center of mass (like a broom balancing on a hand) while keeping the heavy battery near the base for smooth inertia.

```
       [ Tier 3: Sensor Deck ]        <-- MPU-6050 mounted flat & centered
             |         |
         [ Standoffs / Dowels ]
             |         |
    [ Tier 2: Electronics Bay ]       <-- Arduino Nano & L298N Motor Driver
             |         |
         [ Standoffs / Dowels ]
             |         |
     [ Tier 1: Base Platform ]        <-- 2x BO Motors, Wheels & 18650 Battery Pack
```

1. **Cut 3 identical plates** (approx. 15 cm wide × 10 cm deep) from MDF, acrylic, or sturdy foam board.
2. **Assemble Tier 1 (Base):**
   * Mount the two BO motors underneath the plate using motor brackets or hot glue/zip ties.
   * Ensure both motor axles align along the exact same transverse axis.
   * Press-fit the two 65mm rubber wheels onto the motor shafts.
   * Place the 2-slot 18650 battery holder in the center of the base plate between the two wheels.
3. **Mount Standoffs:**
   * Attach 4 standoffs/dowels (approx. 40mm long) at the 4 corners of Tier 1.
4. **Assemble Tier 2 (Electronics Bay):**
   * Fix the second plate onto the Tier 1 standoffs.
   * Mount the L298N motor driver on one side.
   * Stick the mini breadboard on the other side and seat the Arduino Nano firmly onto it.
5. **Assemble Tier 3 (Sensor Deck):**
   * Fix the top plate onto 4 additional standoffs above Tier 2.
   * Secure the **MPU-6050 module** flat in the exact center using double-sided foam tape.
   * **Orientation rule:** Align the X/Y axes parallel to the robot frame so that pitching forward/backward maps cleanly to the IMU pitch angle.

---

## Step 2: Circuit Wiring & Power Connections

> ⚠️ **Warning:** Always disconnect the battery while making or changing connections to avoid accidental short circuits.

### 1. Remove the L298N Jumper Caps
On the L298N motor driver board, locate the jumpers labeled **ENA** and **ENB**. **Remove both jumper caps.**
* Removing these jumpers allows the Arduino Nano to send PWM (analog pulse-width modulation) signals to pins D5 and D10 to dynamically regulate wheel speed.

### 2. Connect the Motor Driver to Arduino Nano
* **ENA** $\rightarrow$ **D5** (PWM)
* **IN1** $\rightarrow$ **D6** (Digital)
* **IN2** $\rightarrow$ **D7** (Digital)
* **IN3** $\rightarrow$ **D8** (Digital)
* **IN4** $\rightarrow$ **D9** (Digital)
* **ENB** $\rightarrow$ **D10** (PWM)

### 3. Connect Motors to L298N
* Left BO Motor $\rightarrow$ **OUT1** and **OUT2** screw terminals.
* Right BO Motor $\rightarrow$ **OUT3** and **OUT4** screw terminals.

### 4. Connect the MPU-6050 to Arduino Nano
* **VCC** $\rightarrow$ **5V** (or 3.3V) on Arduino Nano
* **GND** $\rightarrow$ **GND** on Arduino Nano
* **SCL** $\rightarrow$ **A5** (I2C Clock)
* **SDA** $\rightarrow$ **A4** (I2C Data)
* **INT** $\rightarrow$ **D2** (Hardware Interrupt 0)

### 5. Power Wiring (2x 18650 Li-ion Cells = 7.4V - 8.4V)
* **Battery Red (+)** $\rightarrow$ **12V terminal** of L298N.
* **Battery Black (-)** $\rightarrow$ **GND terminal** of L298N.
* **Common Ground:** Connect a wire from the L298N **GND terminal** to an **Arduino Nano GND** pin.
* **Powering the Nano:** With the L298N onboard 5V regulator jumper attached, connect a wire from the L298N **5V terminal** to the **VIN pin** (or 5V pin) of the Arduino Nano.

---

## Step 3: Software Environment & Library Installation

### 1. Install Arduino IDE
Download and install [Arduino IDE 1.8.x or 2.x](https://www.arduino.cc/en/software).

### 2. Install Drivers (If Nano is not recognized)
If your computer does not assign a COM port when the Nano is plugged in, install the **CH340 driver**.

### 3. Install Repository Libraries
All four required libraries are included in this repository under `/libraries`:
1. Copy the 4 folders from `libraries/` inside this repository:
   * `I2Cdev`
   * `LMotorController`
   * `MPU6050`
   * `PID_v1`
2. Paste them into your Arduino libraries folder:
   * **Windows:** `Documents\Arduino\libraries\`
   * **macOS:** `~/Documents/Arduino/libraries/`
   * **Linux:** `~/Arduino/libraries/`
3. Restart the Arduino IDE.

---

## Step 4: MPU-6050 Calibration & Offset Calculation

Every individual MPU-6050 chip has slight factory silicon variations. For optimal balance:

1. Place the robot completely upright and supported on a flat table so it cannot move.
2. The included code already has calibrated offsets pre-configured:
   ```cpp
   mpu.setXGyroOffset(220);
   mpu.setYGyroOffset(76);
   mpu.setZGyroOffset(-85);
   mpu.setZAccelOffset(1788);
   ```
3. To determine your robot's exact mechanical vertical equilibrium point:
   * Power the Nano via USB and open the Serial Monitor at **115200 baud**.
   * Hold the robot at the angle where it balances naturally by hand.
   * Note the `Angle` displayed (e.g., `175.50`).
   * Set `originalSetpoint = 175.5;` in `SelfBalancingRobot.ino`.

---

## Step 5: Firmware Upload & Motor Direction Verification

1. Open `src/SelfBalancingRobot/SelfBalancingRobot.ino` in Arduino IDE.
2. Select your board:
   * **Tools > Board > Arduino AVR Boards > Arduino Nano**
   * **Tools > Processor > ATmega328P** (If upload fails or hangs, switch to **ATmega328P (Old Bootloader)**)
   * **Tools > Port > Select your COM Port**
3. Click **Upload** (Ctrl + U).
4. **Motor Direction Verification ("The Tumble Test"):**
   * With the robot held safely in the air, turn on the battery power switch.
   * Tilt the robot **forward**: Both wheels must spin **forward** to move underneath the tilt.
   * Tilt the robot **backward**: Both wheels must spin **backward**.
   * *If the wheels spin in reverse:* Swap the motor leads on the L298N screw terminals or invert `IN1`/`IN2` in the code.

---

## Step 6: PID Fine-Tuning & First Run

1. Place the robot upright on a smooth, hard floor (tile or wood; avoid thick carpets initially).
2. Turn on the battery power switch while holding the robot upright for **3 to 5 seconds** without moving it. This allows the MPU-6050 DMP to complete its initialization.
3. Gently release your hand.
4. **Observing Robot Behavior:**
   * **If it balances cleanly:** You're done!
   * **If it twitches rapidly or oscillates violently:** Reduce $K_p$ (e.g., from 51 down to 40) or decrease $K_d$.
   * **If it feels sluggish and falls over slowly:** Increase $K_p$ (e.g., from 51 to 58) and slightly increase $K_d$.
   * **If it drifts slowly in one direction:** Check that `originalSetpoint` matches the physical center of mass, or increase $K_i$ slightly.

For in-depth mathematical tuning rules, consult [docs/PID_TUNING_GUIDE.md](PID_TUNING_GUIDE.md).
