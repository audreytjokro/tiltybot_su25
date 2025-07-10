# TiltyBot

TiltyBot is a tabletop robot built using an ESP32 (TinyPICO), Dynamixel XL330 motors, and an Anker power bank. This project enables simple two-motor driving and tilting behavior, with optional extensions of camera and voice command features.

## Overview
This repository provides a step-by-step guide for running TiltyBot: a compact tabletop robot designed for human-robot interaction (HRI) research, prototyping, and creative exploration. Whether you're a researcher, student, or hobbyist, this repo is designed to be reproducible and easy to build on.

## Table of Contents

- [Hardware Requirements](#hardware-requirements)
- [Software Requirements](#software-requirements)
- [Getting Started](#getting-started)
- [Flashing the Code](#flashing-the-code)
- [Running the Robot](#running-the-robot)
- [Wi-Fi Instructions](#wi-fi-setup)
- [Powering the Robot](#powering-the-robot)
- [Optional Extensions](#optional-extensions)

---

## Hardware Requirements

- ESP32 TinyPICO
- 2× Dynamixel XL330 servo motors
- USB breakout cable
- Anker USB power bank
- Breadboard + jumper wires
- Laptop for development

## Software Requirements

- [PlatformIO](https://platformio.org/install) (installed via VSCode extension)
- Arduino framework for ESP32
- Serial monitor (built into PlatformIO)
- Git

## Getting Started

1. **Clone the repository**

   ```bash
   git clone https://github.com/your-username/tiltybot.git
   cd tiltybot
   ```

2. **Open in VSCode**
- Launch Visual Studio Code
- Open the tiltybot folder

3. **Install PlatformIO**
- Install the PlatformIO extension for VSCode

5. **Connect Your TinyPICO**
- Use a USB-C to connect your TinyPICO board to your computer

---

## Flashing the Code

1. **Select the .cpp File to Run** 

In platformio.ini, locate the build_src_filter section.
- Uncomment the file you want to run by removing the ; at the beginning of the line
- Comment out the others by adding a ; at the beginning
 <img width="351" alt="Screenshot 2025-07-08 at 1 17 45 PM" src="https://github.com/user-attachments/assets/e5ec724c-2561-41ec-aed4-4aca14e01f3e" />

Only one .cpp test file should be active at a time.

2. **Build the LittleFS Filesystem**
- In the PlatformIO sidebar:
- Click “PlatformIO: Build Filesystem Image”

4. **Upload Filesystem**
- Click “PlatformIO: Upload Filesystem Image”

6. **Upload Code to the Board**
- Click “PlatformIO: Upload” to flash the code

8. **Open Serial Monitor**
- Use the built-in PlatformIO serial monitor
- Set the baud rate to 115200

## Running the Robot

Each .cpp file demonstrates different functionality:
- 2motor.cpp: Controls either wheel using a button-based web interface.
- tilty.cpp: Controls the robot in a tilting motion using your phone's GPS.
- drive.cpp: Controls motion control with button-based web interface.

⸻

## Wi-Fi Setup

1. **Edit network.h**

Select the "YOUGROUPNAME" wifi on your phone.

2. **Upload and Monitor**
- After uploading, check the Serial Monitor for:
- The SSID name
- The IP address (usually something like 192.168.4.1)

4. **Connect via Phone**
- On your phone, connect to the robot’s Wi-Fi SSID
- Enter the IP address in your mobile browser to open the control interface
- A warning will show up asking if you would like to proceed, click on Advanced options and proceed anyway.

⸻

## Powering the Robot

Once you’ve tested via USB:
	1.	Disconnect the TinyPICO from your laptop
	2.	Plug it into a USB power bank (such as an Anker portable charger)
	3.	The robot will boot and run on power, and you can still control it from your phone.


Notes
- **Only one source file (*.cpp) should be active at a time**
- If you plan to extend this with camera and microphone functionality (e.g. Whisper / Browser Speech Recognition for speech), integrate it after the base mobility works!


----

## Optional Extensions

TiltyBot is designed to be modular and extensible. Once the core mobility is working, you can build on top of it with additional features.

### Voice Command Control

Control the robot using spoken commands directly from a web browser.

This feature uses the Web Speech API for browser-based voice recognition. It’s lightweight, compatible with most devices, and does not require a backend or cloud setup.

For full setup instructions and implementation details, see the [Voice Control Extension](/voice/README.md).
