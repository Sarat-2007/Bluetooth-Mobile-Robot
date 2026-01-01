# Bluetooth Mobile Robot (Module: Rover) 🤖

![Status](https://img.shields.io/badge/Status-Completed-success)
![Hardware](https://img.shields.io/badge/Hardware-Arduino_Uno-blue)
![Control](https://img.shields.io/badge/Control-Bluetooth_UART-orange)

## 🏎️ Overview
This project implements an **Arduino-based mobile robot** capable of precise wireless control via Bluetooth. [cite_start]Unlike standard hobbyist robots that use direct PWM mapping, this system features a custom **Non-Blocking Smooth Acceleration Algorithm**[cite: 23].

[cite_start]It enables fluid velocity profiling (Soft-Start/Soft-Stop), mimicking the motion of industrial AGVs and reducing mechanical stress on the gearboxes[cite: 64].

## 🧠 Control Theory & Logic
### 1. The "Non-Blocking" Kernel
Standard robots use `delay()` to manage speed, which freezes the processor. This firmware uses a time-based asynchronous loop (`millis()`) to:
* Generate Motor PWM signals.
* Parse Bluetooth UART commands.
* [cite_start]Calculate velocity profiles simultaneously[cite: 118].

### 2. Velocity Profiling (Soft-Start)
To ensure stability, the robot does not jump from 0% to 100% speed instantly.
* **Algorithm:** `handleSpeedTransition()`
* [cite_start]**Logic:** The PWM duty cycle ramps up linearly (e.g., +5 PWM every 20ms) until the target speed is reached[cite: 119].
* [cite_start]**Result:** Enhanced stability and non-jerky movement[cite: 57].

## 🛠️ Tech Stack
* [cite_start]**Microcontroller:** Arduino Uno R3 [cite: 76]
* [cite_start]**Driver:** L298N Dual H-Bridge [cite: 80]
* [cite_start]**Communication:** HC-05 Bluetooth Module (UART) [cite: 78]
* [cite_start]**Power:** 18650 Li-ion Battery Pack [cite: 83]

## 🔌 Pin Configuration
| Component | Pin Name | Arduino Pin |
|-----------|----------|-------------|
| **L298N** | ENA (Left PWM) | 9 |
| | IN1 (Left A) | 8 |
| | IN2 (Left B) | 7 |
| | ENB (Right PWM)| 3 |
| | IN3 (Right A) | 5 |
| | IN4 (Right B) | 4 |
| **HC-05** | TX | 10 (SoftRX) |
| | RX | 11 (SoftTX) |

## 🎮 Controls
[cite_start]The robot accepts single-character commands via Bluetooth Terminal[cite: 113]:
| Key | Action |
|-----|--------|
| **F / B** | Forward / Backward |
| **L / R** | Pivot Turn Left / Right |
| **G / I** | Forward Left / Forward Right (Differential Drive) |
| **0 - 9** | Speed 0% to 90% |
| **q** | Max Speed (100%) |

---
[cite_start]*Developed as part of the Electrical Engineering Laboratory (23EEE105) at Amrita Vishwa Vidyapeetham.* [cite: 8]
