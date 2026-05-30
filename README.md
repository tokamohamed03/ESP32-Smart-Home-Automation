# Smart Home Automation System using ESP32, Bluetooth, and MIT App Inventor

An embedded IoT home automation project that uses an **ESP32 development board** and a **mobile application built with MIT App Inventor** to monitor and control household devices through Bluetooth communication.

## Final Prototype

![Final Prototype](hardware/prototype_photos/prototype.jpg)

## Mobile Application Interface

| Main Screen | Door Logs Screen |
|------------|------------|
| ![](app/screenshots/main_screen.png) | ![](app/screenshots/logs_screen.png) |

## Overview

This project combines embedded systems, mobile application development, PCB design, and hardware interfacing into a complete smart home automation solution.

The ESP32 serves as the central controller and communicates with an Android application via Bluetooth. The application sends control commands and receives sensor data, while the ESP32 reads inputs from the temperature sensor, magnetic door sensor, and manual light switch, and controls outputs such as relays, buzzer, and indicator LEDs.

---

## Features

* Bluetooth communication between ESP32 and Android app
* Remote light control through the mobile application
* Local manual light control using a physical switch
* Automatic synchronization between switch status and system state
* Outlet control with manual and timed operation
* Live room temperature monitoring using a DHT11 sensor
* Door open/close detection using a magnetic sensor
* Storage and display of the last 10 door event logs
* Real-time door status monitoring
* Buzzer and LED feedback
* Custom PCB designed in EasyEDA
* Wiring diagram created in Fritzing
* Mobile application developed using MIT App Inventor

---

## Hardware Components

| Component              | Function                |
| ---------------------- | ----------------------- |
| ESP32 DevKit V1        | Main controller         |
| DHT11 Sensor           | Temperature monitoring  |
| Magnetic Door Sensor   | Door state detection    |
| 2-Channel Relay Module | Bulb and outlet control |
| Manual Switch          | Local light control     |
| Buzzer                 | Audible feedback        |
| LED                    | Status indication       |
| AC Bulb                | Lighting load           |
| AC Outlet              | Appliance control       |

---

## System Functionality

### Smart Lighting Control

The lighting system supports two control methods:

* Remote control through the Bluetooth mobile application.
* Manual control using a physical switch connected to the system.

This allows the light to be operated even when the mobile application is not being used, providing a realistic home automation experience.

### Smart Outlet Control

The outlet can be switched ON or OFF directly from the mobile application or scheduled to operate after a user-selected delay.

### Temperature Monitoring

The DHT11 sensor continuously measures room temperature and sends the readings to the mobile application.

### Door Monitoring

A magnetic door sensor detects whether the door is open or closed and updates the application in real time.

### Door Event Logging

The system records the timestamps of recent door events and displays the latest 10 opening and closing logs through the mobile application.

### Bluetooth Communication

The Android application exchanges commands and system data with the ESP32 through Bluetooth communication.

---

## System Architecture

```text
Android Application
        │
    Bluetooth
        │
      ESP32
        │
 ┌──────┼──────┐
 │      │      │
DHT11 Door  Relays
Sensor Sensor
              │
        ┌─────┴─────┐
        │           │
      Bulb       Outlet
```

---

## Repository Structure

```text
Smart-Home-Automation/
├── README.md
├── firmware/
│   └── HomeAutomation.ino
├── app/
│   ├── HomeAutomation.aia
│   └── screenshots/
├── pcb/
│   ├── pcb_images/
│   └── gerber_files/
├── wiring/
│   └── fritzing_schematic.png
├── hardware/
│   └── prototype_photos/
├── diagrams/
│   ├── block_diagram.png
│   ├── flowchart.png
│   └── state_machine.png
└── docs/
    └── project_report.pdf
```

---

## Mobile Application

The Android application was developed using MIT App Inventor and provides:

* Bluetooth connection management
* Light control
* Outlet control
* Timer-based outlet scheduling
* Temperature display
* Door status monitoring
* Door event log display

### Application Screenshots

```md
![Home Screen](app/screenshots/home_screen.png)

![Control Screen](app/screenshots/control_screen.png)

![Logs Screen](app/screenshots/logs_screen.png)
```

---

## PCB Design

A custom PCB was designed using EasyEDA to improve wiring organization and system reliability.

```md
![PCB Top View](pcb/pcb_images/pcb_top.png)

![PCB Bottom View](pcb/pcb_images/pcb_bottom.png)
```

---

## Wiring Diagram

The complete system wiring and hardware connections were designed using Fritzing.

```md
![Fritzing Schematic](wiring/fritzing_schematic.png)
```

---

## Hardware Demonstration

Additional images of the assembled system and testing stages can be found in:

```text
hardware/prototype_photos/
```

---

## Software and Design Tools

* Arduino IDE
* MIT App Inventor
* EasyEDA
* Fritzing

---

## Installation

1. Open the firmware in Arduino IDE.
2. Install the required ESP32 board package.
3. Install all required libraries.
4. Select the correct ESP32 board and COM port.
5. Upload the firmware.
6. Power the system.
7. Pair the mobile phone with the ESP32.
8. Open the application and connect.

---

## Future Improvements

* Wi-Fi and cloud connectivity
* Mobile notifications
* Voice assistant integration
* Historical database storage
* Energy consumption monitoring
* Additional smart home sensors

---

## Author

**Toka Mohamed**

Embedded Systems & IoT Project

---

## License

This project is released for educational and demonstration purposes.
