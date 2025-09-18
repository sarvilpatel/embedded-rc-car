# 🚗 Embedded RC Car with Obstacle Avoidance

An Arduino Uno–based RC car project featuring:

- Dual H-Bridge Motor Driver (L298N)
- Bluetooth (HC-05) remote control
- Ultrasonic Sensor (HC-SR04) for obstacle detection
- Auto stop/resume logic (car stops when too close, resumes when clear)
- Speed control via PWM

---

## 🎥 Demo
[![Watch the demo](https://img.youtube.com/vi/VV_tw-a58Gs/0.jpg)](https://youtu.be/VV_tw-a58Gs)

---

## ⚡ Features
- Forward, backward, left, and right control over Bluetooth.
- Speed selection via numeric commands (1–10).
- Adaptive stop/resume based on distance:
  - Fast → stops at ~30 cm
  - Medium → stops at ~22 cm
  - Slow → stops at ~15 cm
- Obstacle detection + hysteresis (prevents jitter).
- Auto-resume once path is clear.
- Written in Arduino C++.

---

## 🛠️ Hardware
- Arduino Uno R3  
- L298N Dual H-Bridge Motor Driver  
- HC-05 Bluetooth Module  
- HC-SR04 Ultrasonic Sensor  
- 2 × DC Motors + Wheels  
- 1 × Chassis + Battery Pack  

---

## ⚡ Wiring

| Component           | Arduino Pin | Notes                        |
|---------------------|-------------|------------------------------|
| L298N IN1           | D4          | Motor A                      |
| L298N IN2           | D5          | Motor A                      |
| L298N IN3           | D6          | Motor B                      |
| L298N IN4           | D7          | Motor B                      |
| L298N ENA (PWM)     | D9          | Speed control A              |
| L298N ENB (PWM)     | D10         | Speed control B              |
| HC-SR04 TRIG        | D2          | Ultrasonic trigger           |
| HC-SR04 ECHO        | D3          | Ultrasonic echo              |
| HC-05 TX            | RX (D0)     | Arduino receives commands    |
| HC-05 RX            | TX (D1)     | Use voltage divider for 5V→3.3V |

---

## 📂 File Structure

---

## 🚀 Getting Started
1. Wire the components as shown above.
2. Upload `src/main.ino` to Arduino Uno.
3. Pair Bluetooth module with your phone (default PIN: `1234` or `0000`).
4. Use a Bluetooth RC controller app:
   - `F` → Forward  
   - `B` → Backward  
   - `L` → Left turn  
   - `R` → Right turn  
   - `S` → Stop  
   - `1–10` → Speed levels (mapped to PWM)

**Behavior:**  
- Hold **F** → car moves forward until obstacle detected → auto stop → auto resume when clear.  
- Release **F**, press **L/R** → it turns.  
- Press **F** again → goes forward again if no obstacle.  

---

## 📜 License
This project is licensed under the MIT License — see the [LICENSE](LICENSE) file for details.
