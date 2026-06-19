# Conway's Game of Life for ESP32 (Cheap Yellow Display)

A highly optimized implementation of Conway's Game of Life for the popular **ESP32-2432S028R** development board, affectionately known in the maker community as the **"Cheap Yellow Display" (CYD)**. 

This project utilizes the fantastic `LovyanGFX` library to achieve high-framerate, flicker-free rendering of the classic cellular automaton.

🔗 **Repository:** [https://github.com/rmruiz/life_game_esp32](https://github.com/rmruiz/life_game_esp32)

![ESP32 CYD](https://img.shields.io/badge/Hardware-ESP32_CYD-yellow?style=for-the-badge&logo=espressif)
![LovyanGFX](https://img.shields.io/badge/Library-LovyanGFX-blue?style=for-the-badge)

## ✨ Features

* **Flicker-Free Rendering:** Uses in-memory sprites to draw the entire grid instantly without screen tearing.
* **Memory Optimized (No PSRAM Required):** A full 320x240 standard 16-bit sprite would crash a standard ESP32 due to RAM exhaustion. This project elegantly solves this by dropping the sprite's color depth to **8-bit**, halving the memory footprint so it fits comfortably in the ESP32's internal SRAM.
* **Interactive Touch:** Fully utilizes the CYD's onboard XPT2046 resistive touch controller. Simply **tap anywhere on the screen** to instantly randomize the grid and start a new simulation.

## 🛠️ Hardware Requirements

* **Board:** ESP32-2432S028R (2.8" LCD TFT Module with Touch)
* **Screen:** 240x320 ST7789 TFT
* **Touch Controller:** XPT2046 Resistive Touch

## 📦 Dependencies

This project relies on a single library. You can install it directly through the Arduino IDE Library Manager:

* **LovyanGFX** by lovyan03

## 🚀 Quick Start

1. Open the `.ino` file in the Arduino IDE.
2. Ensure you have installed the `LovyanGFX` library.
3. Select your board: **ESP32 Dev Module** (or any generic ESP32 WROOM option).
4. **Important Upload Settings:**
   * **Baud Rate:** `115200` (The CYD board sometimes struggles with higher upload speeds).
   * **Flash Size:** `4MB`
5. Compile and upload! 

## 🧠 Technical Notes for CYD Users

The Cheap Yellow Display features a unique hardware layout that can be tricky to configure. This code includes a custom `LGFX` class configuration specifically tailored for this board:
* **Separated SPI Buses:** Unlike standard breadboard setups that share the SPI bus, the CYD hardwires the display to the `HSPI` bus and the touch controller to the `VSPI` bus. This code configures both independently to prevent collisions.
* **Touch Interrupts:** The touch controller uses GPIO 36 for hardware interrupts, and custom pins (25, 32, 39, 33) for its SPI communication.

## 📜 The Rules of Conway's Game of Life
The universe of the Game of Life is a two-dimensional orthogonal grid of square cells, each of which is in one of two possible states, *alive* or *dead*. Every cell interacts with its eight neighbors:

1. Any live cell with fewer than two live neighbors dies (underpopulation).
2. Any live cell with two or three live neighbors lives on to the next generation.
3. Any live cell with more than three live neighbors dies (overpopulation).
4. Any dead cell with exactly three live neighbors becomes a live cell (reproduction).

## 📄 License
This project is open-source. Feel free to modify, fork, and use it in your own CYD projects!
