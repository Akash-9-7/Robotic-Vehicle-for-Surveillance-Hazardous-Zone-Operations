# 🤖 IoT-Based Military & Railway Surveillance Robot

An IoT-enabled multi-sensor surveillance robot designed for hazardous environments, military tracks, and railway inspection. Built using **ESP32**, **Arduino IoT Cloud**, **HC-SR04 Ultrasonic Sensor**, **Flame Sensor**, and a **16x2 LCD Display**.

---

## 🌟 Key Features

* **🌐 IoT Cloud Remote Control:** Real-time directional navigation (Forward, Backward, Left, Right, Stop) integrated with Arduino IoT Cloud.
* **🛡️ Collision Avoidance:** Automatic emergency braking triggers when obstacles are detected within **15 cm**.
* **🔥 Fire & Hazard Detection:** Real-time flame detection for early warning and tracking.
* **📺 Live Onboard Display:** Status updates, active commands, and distance displayed on a 16x2 LCD.

---

## 🛠️ Hardware Setup & Pin Mapping

| Component | Connection Pin | Description |
| :--- | :--- | :--- |
| **ESP32 Dev Board** | Main Controller | Wi-Fi & Dual-Core MCU |
| **L298N Motor Driver** | GPIO 32, 33, 21, 22 | Controls 2x DC Motors |
| **Ultrasonic Sensor** | Trig: GPIO 13, Echo: GPIO 12 | Distance & Obstacle Detection |
| **Flame Sensor (DO)** | GPIO 34 | Digital Fire Detection |
| **16x2 LCD Display** | RS:2, EN:15, D4:19, D5:18, D6:5, D7:4 | Onboard Debug Display |

---

## 🚀 Getting Started

1. **Clone the repository:**
   ```bash
   git clone https://github.com/Akash-9-7/Robotic-Vehicle-for-Surveillance-Hazardous-Zone-Operations.git
