<a id="readme-top"></a>

<!-- PROJECT SHIELDS -->
<div align="center">

[![Platform][Arduino-shield]][Arduino-url]
[![Language][Cpp-shield]][Cpp-url]
[![Simulator][Wokwi-shield]][Wokwi-url]
[![Display][OLED-shield]][OLED-url]
[![License][License-shield]][License-url]

</div>

<!-- PROJECT HEADER -->
<br />
<div align="center">
  <h1 align="center">🍅 Pomodoro Focus Station & Physical DND Flag</h1>

  <p align="center">
    An Arduino Uno desk productivity timer featuring a 128×64 OLED dashboard, single-joystick navigation, traffic-light status LEDs, and a servo-actuated physical <b>"Do Not Disturb"</b> flag.
    <br />
    <br />
    <a href="https://wokwi.com"><strong>Run Wokwi Simulation »</strong></a>
    <br />
    <br />
    <a href="#usage--joystick-controls">View Controls</a>
    ·
    <a href="#hardware-architecture--pinout">Wiring Pinout</a>
    ·
    <a href="#roadmap">Roadmap</a>
  </p>
</div>

<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#about-the-project">About The Project</a>
      <ul>
        <li><a href="#key-features">Key Features</a></li>
        <li><a href="#built-with">Built With</a></li>
      </ul>
    </li>
    <li>
      <a href="#hardware-architecture--pinout">Hardware Architecture & Pinout</a>
      <ul>
        <li><a href="#bill-of-materials-bom">Bill of Materials (BOM)</a></li>
        <li><a href="#wiring--pin-mapping">Wiring & Pin Mapping</a></li>
      </ul>
    </li>
    <li>
      <a href="#getting-started">Getting Started</a>
      <ul>
        <li><a href="#required-libraries">Required Libraries</a></li>
        <li><a href="#running-in-wokwi-simulator">Running in Wokwi Simulator</a></li>
      </ul>
    </li>
    <li>
      <a href="#usage--joystick-controls">Usage & Joystick Controls</a>
      <ul>
        <li><a href="#hardware-state-machine">Hardware State Machine</a></li>
      </ul>
    </li>
    <li><a href="#roadmap">Roadmap</a></li>
    <li><a href="#license">License</a></li>
    <li><a href="#acknowledgments">Acknowledgments</a></li>
  </ol>
</details>

---

## About The Project

Standard software Pomodoro apps get buried under browser tabs and fail to stop coworkers, roommates, or family members from interrupting deep-work sessions. The **Pomodoro Focus Station** solves both problems by pairing a dedicated **128×64 SSD1306 OLED countdown dashboard** with a **physical Servo Motor flag** that automatically raises to **90° ("DO NOT DISTURB")** the moment a focus session begins.

Built strictly under a constrained embedded component budget—using **zero external RTC modules or extra buttons**—the entire station is driven by the Arduino Uno's internal `millis()` timer and a single **Analog Joystick**.

### Key Features

* **Physical "Do Not Disturb" Servo Flag:** Automatically rotates to `90° (UP)` during active Focus sessions and lowers to `0° (DOWN)` during Break or Pause states.
* **Single-Joystick 3-Axis Control:** Uses horizontal deflection (`HORZ`) to configure session length, vertical deflection (`VERT`) to toggle simulation speed, and the center push-switch (`SEL`) to start or pause.
* **60× Turbo Demo Mode:** Includes an on-the-fly time-scaling toggle (`1 real second = 1 Pomodoro minute`) so full work/break cycles can be demonstrated during live evaluations in seconds.
* **Tri-Color LED Status Bar:** Provides instant ambient visibility across a room (**Red** = Focus/Busy, **Yellow** = Paused/Standby, **Green** = Break/Available).
* **Non-Blocking Architecture:** Uses `millis()` state-machine timing rather than blocking `delay()` loops, ensuring responsive joystick polling at ~33 FPS.

### Built With

* ![Arduino][Arduino-badge]
* ![C++][Cpp-badge]
* ![Wokwi][Wokwi-badge]
* **Adafruit SSD1306 & GFX** (I2C Monochrome OLED Driver)
* **Arduino Servo Library** (Timer1 Hardware PWM Control)

<p align="right">(<a href="#readme-top">back to top</a>)</p>

---

## Hardware Architecture & Pinout

### Bill of Materials (BOM)

This project strictly uses only the 7 permitted component types with zero external sensors:

| # | Component | Wokwi Part ID | Qty | Purpose |
| :- | :--- | :--- | :-- | :--- |
| 1 | **Arduino Uno (ATmega328P)** | `wokwi-arduino-uno` | 1 | Main microcontroller & `millis()` timekeeper |
| 2 | **128×64 OLED Display (SSD1306)** | `board-ssd1306` | 1 | I2C dashboard (`0x3C`) for clock, progress bar, and HUD |
| 3 | **Analog Joystick** | `wokwi-analog-joystick` | 1 | 2-axis ADC menu input + center `SEL` push button |
| 4 | **Micro Servo Motor** | `wokwi-servo` | 1 | Physical "Do Not Disturb" flag actuator (`0°` / `90°`) |
| 5 | **LEDs (Red, Yellow, Green)** | `wokwi-led` | 3 | Ambient room status indicators (Focus / Paused / Break) |
| 6 | **Resistors (220 Ω)** | `wokwi-resistor` | 3 | Current limiting (~13.6 mA) for the 3 status LEDs |
| 7 | **Piezo Buzzer** | `wokwi-buzzer` | 1 | Tactile UI clicks and phase-transition audio chimes |

### Wiring & Pin Mapping

| Component | Component Pin | Arduino Uno Pin | Protocol / Signal Type |
| :--- | :--- | :--- | :--- |
| **SSD1306 OLED** | `VCC` / `GND` | `5V` / `GND` | Power |
| | `SCL` | `A5` | Hardware I2C Clock |
| | `SDA` | `A4` | Hardware I2C Data |
| **Analog Joystick** | `VCC` / `GND` | `5V` / `GND` | Power |
| | `HORZ` | `A0` | 10-bit ADC (`0–1023`) |
| | `VERT` | `A1` | 10-bit ADC (`0–1023`) |
| | `SEL` | `Pin 2` | Digital Input (`INPUT_PULLUP`, Active-Low) |
| **Servo Motor** | `V+` / `GND` | `5V` / `GND` | Power |
| | `PWM` | `Pin 9` | 50 Hz PWM Signal |
| **Piezo Buzzer** | `+` / `-` | `Pin 8` / `GND` | Square-Wave `tone()` Output |
| **Green LED** | `Anode (+)` | `Pin 4` *(via 220 Ω)* | Digital Output (Break Mode) |
| **Yellow LED** | `Anode (+)` | `Pin 5` *(via 220 Ω)* | Digital Output (Paused Mode) |
| **Red LED** | `Anode (+)` | `Pin 6` *(via 220 Ω)* | Digital Output (Focus Mode) |

<p align="right">(<a href="#readme-top">back to top</a>)</p>

---

## Getting Started

You can run this project directly in your browser using the **Wokwi Arduino Simulator** or flash it onto physical hardware.

### Required Libraries

Install the following libraries via the **Wokwi Library Manager** (or Arduino IDE Library Manager):
```text
Adafruit SSD1306
Adafruit GFX Library
Servo
```

### Running in Wokwi Simulator

1. Go to [Wokwi.com](https://wokwi.com) and create a new **Arduino Uno** project.
2. Copy the project code into the **`sketch.ino`** tab.
3. Open the **`diagram.json`** tab and paste the project's hardware layout JSON to auto-wire all components.
4. Open the **Library Manager** tab, click the purple **`+`** button, and add `Adafruit SSD1306`, `Adafruit GFX Library`, and `Servo`.
5. Click the green **Start Simulation** play button.

<p align="right">(<a href="#readme-top">back to top</a>)</p>

---

## Usage & Joystick Controls

All user interactions are handled through the single **Analog Joystick**:

| Input Action | Condition | Function |
| :--- | :--- | :--- |
| **Click Center (`SEL`)** | Any Time | **Starts** or **Pauses** the active countdown timer |
| **Push Left / Right (`HORZ`)** | While Paused | Decreases / Increases Focus duration (`5 min` to `45 min` in 5-min steps) |
| **Push Up (`VERT > 700`)** | Any Time | Enables **60× Turbo Demo Mode** (`1 real sec = 1 Pomodoro min`) |
| **Pull Down (`VERT < 300`)** | Any Time | Enables **1× Real-Time Mode** (`1 real sec = 1 Pomodoro sec`) |

### Hardware State Machine

The system synchronizes all visual, mechanical, and audio outputs across three primary states:

| System State | OLED Header | Active LED | Servo Flag Angle | Buzzer Feedback |
| :--- | :--- | :--- | :--- | :--- |
| **PAUSED / STANDBY** | `POMODORO : PAUSED` | 🟨 **Yellow (`Pin 5`)** | `0°` (Flag Down) | `600 Hz` pause tone |
| **FOCUS MODE** | `FOCUS MODE (DND ON)` | 🟥 **Red (`Pin 6`)** | `90°` (Flag Raised) | `1200 Hz` start tone |
| **BREAK MODE** | `BREAK MODE (FREE)` | 🟩 **Green (`Pin 4`)** | `0°` (Flag Down) | Two-tone `880 Hz -> 1320 Hz` chime |

<p align="right">(<a href="#readme-top">back to top</a>)</p>

---

## Roadmap

- [x] Core Pomodoro state machine (`Focus`, `Break`, `Paused`)
- [x] Physical Servo DND flag synchronization (`0°` to `90°`)
- [x] 60× Turbo Demo speed toggle for rapid classroom/viva grading
- [ ] Add `EEPROM` storage to persist completed session counts across power cycles
- [ ] Automatically trigger an extended 15-minute Long Break after every 4 completed Pomodoros
- [ ] Add adjustable Break duration in the Pause menu

<p align="right">(<a href="#readme-top">back to top</a>)</p>

---

## License

Distributed under the MIT License. See `LICENSE.txt` for more information.

<p align="right">(<a href="#readme-top">back to top</a>)</p>

---

## Acknowledgments

* [Wokwi Embedded Systems Simulator](https://wokwi.com/)
* [Adafruit SSD1306 & GFX Display Libraries](https://github.com/adafruit/Adafruit_SSD1306)
* [Best-README-Template by othneildrew](https://github.com/othneildrew/Best-README-Template)

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- MARKDOWN LINKS & IMAGES -->
[Arduino-shield]: https://img.shields.io/badge/Platform-Arduino_Uno-00979D?style=for-the-badge&logo=arduino&logoColor=white
[Arduino-url]: https://www.arduino.cc/
[Cpp-shield]: https://img.shields.io/badge/Language-C%2B%2B-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white
[Cpp-url]: https://isocpp.org/
[Wokwi-shield]: https://img.shields.io/badge/Simulator-Wokwi-00C853?style=for-the-badge
[Wokwi-url]: https://wokwi.com/
[OLED-shield]: https://img.shields.io/badge/Display-SSD1306_128x64_I2C-blue?style=for-the-badge
[OLED-url]: https://github.com/adafruit/Adafruit_SSD1306
[License-shield]: https://img.shields.io/badge/License-MIT-yellow.svg?style=for-the-badge
[License-url]: https://opensource.org/licenses/MIT
[Arduino-badge]: https://img.shields.io/badge/Arduino_Uno-00979D?style=flat-square&logo=arduino&logoColor=white
[Cpp-badge]: https://img.shields.io/badge/C++-00599C?style=flat-square&logo=c%2B%2B&logoColor=white
[Wokwi-badge]: https://img.shields.io/badge/Wokwi_Simulator-00C853?style=flat-square