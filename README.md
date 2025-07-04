# NAILA Desk Robot

Small custom built desk AI robot prototype.

## Table of Contents

1.  [About the Project](#about-the-project)
2.  [Features & Capabilities](#features--capabilities)
3.  [System Architecture](#system-architecture)
4.  [Hardware Overview](#hardware-overview)
5.  [Software Components](#software-components)
6.  [Future Enhancements](#future-enhancements)
7.  [Copyright & Licensing](#7-copyright--licensing)

---

## 1. About the Project

This project aims to build a personalized AI companion robot inspired by devices like EMO or Vector. The goal is to create an interactive and intelligent robot that lives on your desk, providing companionship, entertainment, and utility, all powered by local, privacy-focused AI.

By leveraging powerful local AI models and efficient communication protocols, NAILA aims to offer a low-latency, responsive experience without constant reliance on cloud services for the time being.

## 2. Features & Capabilities

NAILA as a robot is designed to offer a range of intelligent and interactive features:

* **Natural Language Understanding (NLU) & Conversation:**
    * **Wake Word Detection:** Responds to a custom wake word to initiate interaction.
    * **Speech-to-Text (STT):** Transcribes spoken commands and queries into text using **`faster-whisper`** (running a `tiny.en` or `base.en` Whisper model).
    * **Large Language Model (LLM) Processing:** Engages in dynamic, intelligent conversations and answers questions using a local LLM (**Llama 3 8B Instruct** via `llama-cpp-python`).
    * **Text-to-Speech (TTS):** Generates natural-sounding voice responses using the **`Llama-OuteTTS-1.0-1B-GGUF`** model (also powered by `llama-cpp-python`), allowing for unique voice characteristics via speaker referencing.

* **Interactive Presence:**
    * **Expressive Display:** Utilizes an integrated screen for animated eyes, expressions, and on-screen information.
    * **Motorized Movements:** Enables head tilts, body movements, and other gestures to convey emotion and attention.
    * **Responsive Audio:** Plays sounds and speaks directly to the user.

* **Environmental Awareness:**
    * **Basic Vision Processing:** Utilizes a camera for basic object detection and recognition using **`Ultralytics YOLOv8 Nano`**. This can enable the robot to identify people, pets, or specific objects in its vicinity.

* **Maintainability & Updates:**
    * **Over-The-Air (OTA) Firmware Updates:** Allows for easy wireless updates to the robot's embedded firmware directly from the local server.

## 3. System Architecture

The robot's intelligence is split between a low-power embedded system on the robot itself and a more powerful local AI server, communicating over a local network.

**Key Interactions:**
1. Robot (ESP32-S3 Sense): Handles hardware control (motors, display, LEDs), basic audio capture, wake word detection (on-device or via server), and WiFi communication.
2. MQTT Broker: Acts as the central message hub. Both the robot and the server connect to it, enabling asynchronous, event-driven communication.
3. Local AI Server: Runs on a more powerful machine (e.g., PC, Raspberry Pi 5). It hosts the large AI models and processes the heavy computational tasks.

## 4. Hardware Overview

* **Main Microcontroller:** ESP32-S3 Sense (chosen for its AI capabilities, camera, and voice processing peripherals).
* **Camera:** Integrated with ESP32-S3 Sense.
* **Display:** Small, expressive display (e.g., SPI or I2C LCD/OLED) for facial animations and information.
* **Audio Input:** Microphone array (e.g., PDM mic on ESP32-S3 Sense board).
* **Audio Output:** Speaker with amplifier.
* **Motors:** Small servo motors for head/body articulation.
* **Power:** Battery-powered with charging circuit (details to be defined).

## 5. Software Components

The project is divided into two main software domains:

**`firmware/` (Robot Embedded Code)**
This directory contains the C++/PlatformIO code that runs directly on the ESP32-S3 Sense board.

* **Responsibilities:**  
  * Low-level hardware control (motors, display, LEDs).
  * Audio capture and pre-processing.
  * WiFi connectivity and MQTT client for communication with the server.
  * Potentially on-device wake word detection (if resources allow).
  * Receiving and playing TTS audio.
  * Receiving and executing action commands from the server.
  * Handling OTA firmware updates.

* **Further Details:** See [firmware/README.md](firmware/README.md)

**`server/` (Local AI Server)**
This directory contains the Python-based server that runs on a more powerful machine on your local network.

* **Responsibilities:**
  * Hosts and runs large AI models for STT, NLU/LLM, TTS, and (optional) Vision.
  * Manages MQTT communication with the robot.
  * Orchestrates the robot's conversational flow and decision-making.
  * Hosts a simple HTTP server for OTA firmware updates.
  * Key AI Frameworks/Models Used:
  * Speech-to-Text (STT): faster-whisper (e.g., tiny.en or base.en Whisper GGUF model)
  * Natural Language Understanding (NLU) / Large Language Model (LLM): llama-cpp-python (e.g., Llama 3 8B Instruct GGUF model)
  * Text-to-Speech (TTS): llama-cpp-python (with Llama-OuteTTS-1.0-1B-GGUF model for voice generation)
  * Vision (Optional): Ultralytics YOLOv8 (e.g., yolov8n.pt model)
  * MQTT Client: paho-mqtt or similar.

* **Further Details:** See [server/README.md](server/README.md)

## 6. Future Enhancements 

* **Advanced Embodiment:** More complex motor control for nuanced expressions and body language.
* **Improved Vision:** Facial recognition, emotion detection, spatial awareness.
* **Personalization:** Learning user preferences, remembering past conversations.
* **Integrated Skills:** Connecting to external APIs for weather, news, smart home control, etc.
* **Mobile App Integration:** Companion app for remote control, settings, and advanced logging.
* **On-device Wake Word:** Further optimize on-device processing to reduce server load for wake word.

## 7. Copyright & Licensing

©2025 Valkyrie Remedy LLC. All rights reserved.

This code is provided for informational and educational purposes only. You are welcome to view the code.

**Unless explicitly granted by the copyright holder, no one is permitted to:**
* Use, copy, modify, distribute, or create derivative works from this code for any purpose, commercial or non-commercial.
* Offer contributions to this repository. All development is managed internally.

If you wish to discuss potential collaborations or licensing for specific use cases, please contact Lillith@valkyrieremedy.com.