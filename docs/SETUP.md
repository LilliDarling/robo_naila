# Comprehensive Setup Guide

This guide provides detailed, step-by-step instructions to set up both the ESP32-S3 Sense robot firmware and the local AI server. 

## Table of Contents

1. [Overview](#1-overview)
2. [Prerequisites](#2-prerequisites)
   - [Hardware Requirements](#hardware-requirements)
   - [Software Requirements](#software-requirements)
3. [Server Setup (Local AI Brain)](#3-server-setup-local-ai-brain)
   - [3.1. Clone the Repository](#31-clone-the-repository)
   - [3.2. Install `uv` (Python Package Manager)](#32-install-uv-python-package-manager)
   - [3.3. Set up Python Virtual Environment](#33-set-up-python-virtual-environment)
   - [3.4. Install Python Dependencies](#34-install-python-dependencies)
   - [3.5. Install MQTT Broker (Mosquitto)](#35-install-mqtt-broker-mosquitto)
   - [3.6. Download AI Models](#36-download-ai-models)
   - [3.7. Configure the Server (`config.py`)](#37-configure-the-server-configpy)
4. [Firmware Setup (Robot Embedded Code)](#4-firmware-setup-robot-embedded-code)
   - [4.1. Install PlatformIO](#41-install-platformio)
   - [4.2. Open Project in PlatformIO](#42-open-project-in-platformio)
   - [4.3. Configure Firmware (`config.h`)](#43-configure-firmware-configh)
   - [4.4. Build and Upload Firmware](#44-build-and-upload-firmware)
   - [4.5. Monitor Serial Output](#45-monitor-serial-output)
5. [Running the Complete System](#5-running-the-complete-system)
   - [5.1. Start MQTT Broker](#51-start-mqtt-broker)
   - [5.2. Start Local AI Server](#52-start-local-ai-server)
   - [5.3. Start OTA Update Server (Optional)](#53-start-ota-update-server-optional)
   - [5.4. Power on the Robot](#54-power-on-the-robot)
6. [Troubleshooting Common Issues](#6-troubleshooting-common-issues)

---

## 1. Overview

This project builds an interactive robot companion with local AI processing. The system is split into two main components:

- **Firmware:** C++/PlatformIO code running on an ESP32-S3 Sense board, handling hardware interactions and network communication.
- **Server:** Python-based AI server running on a more powerful local machine, performing heavy AI computations (**STT**, **LLM**, **TTS**, **Vision**).

Communication between the robot and the server is primarily via **MQTT**. Over-The-Air (OTA) firmware updates are also supported via a separate HTTP server component.

## 2. Prerequisites

### Hardware Requirements

- **ESP32-S3 Sense Development Board:** This is the brains of your physical robot. Make sure you have a **USB-C data cable** for flashing.
- **Host Machine for AI Server:** A computer (Windows, macOS, or Linux) to run the AI models.
  - **CPU:** Modern multi-core CPU (Intel i5/Ryzen 5 or better recommended).
  - **RAM:** 8GB minimum, **16GB+ highly recommended**, especially for larger LLMs.
  - **GPU (Optional but Recommended):** An NVIDIA GPU with CUDA support can significantly speed up `llama-cpp-python` and `faster-whisper` inference. Ensure you have the correct drivers installed.
  - *Note:* A Raspberry Pi 5 can run smaller models (e.g., tiny Whisper, smaller LLMs) but performance will be limited.
- **Reliable WiFi Network:** Both your ESP32-S3 robot and the AI server machine must be connected to the same local network.
- **Robot Physical Components:** (Specific to your build, but generally includes):
  - Small display (e.g., SPI/I2C LCD or OLED) for expressive eyes.
  - Small servo motors for head/body articulation.
  - Integrated microphone and speaker (typically on the ESP32-S3 Sense board or connected).

### Software Requirements

- **Git:** For cloning this repository. ([Download Git](https://git-scm.com/downloads))
- **Python 3.10 or higher:** For the local AI server. ([Download Python](https://www.python.org/downloads/))
- **`uv`:** A fast Python package installer and virtual environment manager (highly recommended).
- **PlatformIO Core / VS Code Extension:** For ESP32 firmware development and flashing. ([PlatformIO Installation](https://platformio.org/install))
- **MQTT Broker:** A message broker like Mosquitto.

---

## 3. Server Setup (Local AI Brain)

This section guides you through setting up the Python-based AI server on your host machine.

### 3.1. Clone the Repository

Open your terminal or command prompt and clone the entire robot project repository. The `--recursive` flag is crucial as some dependencies might be included as Git submodules.

```bash
git clone --recursive https://github.com/LilliDarling/robo_naila
cd robo-naila/server
```

### 3.2. Install `uv` (Python Package Manager)

`uv` is an extremely fast and efficient Python package installer and virtual environment creator from Astral. If you don't have it installed globally, follow these instructions:

**On macOS and Linux:**

```bash
curl -LsSf https://astral.sh/uv/install.sh | sh
```
or if you have homebrew:
```bash
brew install uv
```

**On Windows (PowerShell):**

```powershell
powershell -ExecutionPolicy ByPass -c "irm https://astral.sh/uv/install.ps1 | iex"
```

After installation, it's a good practice to close and reopen your terminal or ensure uv's installation directory is added to your system's PATH.

### 3.3. Set up Python Virtual Environment

Navigate to the `server/` directory (if you're not already there) and create a dedicated Python virtual environment. This isolates your project's Python dependencies from your system-wide Python installation.

```bash
uv venv
```

Now, activate the virtual environment:

**On Linux/macOS:**

```bash
source .venv/bin/activate
```

**On Windows (Command Prompt):**

```cmd
.\.venv\Scripts\activate
```

**On Windows (PowerShell):**

```powershell
.\.venv\Scripts\Activate.ps1
```

Your terminal prompt should change (e.g., `(.venv) user@host:~/robo_naila/server$`) indicating the virtual environment is active. Always activate this environment before running server commands.

### 3.4. Install Python Dependencies

With your virtual environment activated, install all required Python packages using `uv`. This will read the `requirements.txt` file and efficiently install all dependencies, also generating/updating a `uv.lock` file for reproducible builds.

```bash
uv pip install -r requirements.txt
```

### 3.5. Install MQTT Broker (Mosquitto)

The robot and server communicate over MQTT. You need an MQTT broker running on your local network. It's highly recommended to run it on the same machine as your AI server. Mosquitto is a popular, lightweight, and open-source choice.

**On Linux (Debian/Ubuntu/Raspberry Pi OS):**

```bash
sudo apt-get update
sudo apt-get install mosquitto mosquitto-clients
sudo systemctl enable mosquitto # Enable auto-start on boot
sudo systemctl start mosquitto  # Start the service now
```

**On macOS (using Homebrew):**

```bash
brew install mosquitto
brew services start mosquitto # Start the service now and enable auto-start
```

**On Windows:**
Download the installer from the [Mosquitto Downloads page](https://mosquitto.org/download/). Follow the installation wizard. Mosquitto usually runs as a Windows service after installation.

**Verification (Optional):**
To confirm Mosquitto is running and accessible, open two separate terminal windows:

**Terminal 1 (Subscriber):** (Requires Mosquitto clients installed)

```bash
mosquitto_sub -h localhost -t "test/topic"
```

**Terminal 2 (Publisher):**

```bash
mosquitto_pub -h localhost -t "test/topic" -m "Hello MQTT from my server!"
```

You should see "Hello MQTT from my server!" appear in Terminal 1. If not, troubleshoot your Mosquitto installation and firewall settings.

### 3.6. Download AI Models

The AI server requires several large pre-trained models. You'll need to manually download these into the `models/` directory within your `server/` folder. Create the specified subdirectories if they don't already exist.

Your `models/` directory structure should look like this:

```
robot-project/server/models/
├── stt/
├── llm/
├── tts/
└── vision/ (optional)
```

#### a. Speech-to-Text (STT) Model: faster-whisper (GGUF format)

- **Model:** OpenAI Whisper (optimized for llama.cpp format, used by faster-whisper).
- **Recommended Size:** Start with `tiny.en` for quicker downloads and lower resource usage. `base.en` offers better accuracy.
- **Download Location:** Hugging Face. Search for `ggerganov/whisper.cpp` or `Systran/faster-whisper` models. Look specifically for `.gguf` files (e.g., `ggml-tiny.en.bin`).
- **Example Link:** [ggerganov/whisper.cpp](https://huggingface.co/ggerganov/whisper.cpp) - `ggml-tiny.en.bin` (If downloaded as `.bin`, you can rename it to `.gguf` for clarity).
- **Place in:** `robo_naila/server/models/stt/`
- **Example File:** `models/stt/whisper-tiny.en.gguf`

#### b. Large Language Model (LLM): Llama 3 8B Instruct (GGUF format)

- **Model:** Meta-Llama-3-8B-Instruct.
- **Recommended Quantization:** `Q4_K_M` or `Q5_K_M` provide a good balance between performance, accuracy, and file size for local inference on consumer hardware.
- **Download Location:** Hugging Face. Look for `.gguf` quantizations provided by communities like TheBloke or bartowski.
- **Example Link:** [bartowski/Meta-Llama-3-8B-Instruct-GGUF](https://huggingface.co/bartowski/Meta-Llama-3-8B-Instruct-GGUF) (Choose a `Q4_K_M` or `Q5_K_M` variant).
- **Place in:** `robo_naila/server/models/llm/`
- **Example File:** `models/llm/Meta-Llama-3-8B-Instruct-Q4_K_M.gguf`

#### c. Text-to-Speech (TTS) Model: Llama-OuteTTS-1.0-1B (GGUF format)

- **Model:** OuteAI/Llama-OuteTTS-1.0-1B. This model also uses the GGUF format and runs via llama-cpp-python.
- **Download Location:** Hugging Face.
- **Model Link:** [OuteAI/Llama-OuteTTS-1.0-1B-GGUF](https://huggingface.co/OuteAI/Llama-OuteTTS-1.0-1B-GGUF) (Download the `.gguf` file, e.g., `llama-outetts-1b-q4_k_m.gguf`).
- **Place in:** `robo_naila/server/models/tts/`
- **Example File:** `models/tts/llama-outetts-1b-q4_k_m.gguf`

**Speaker Reference Audio (.wav):** This model requires a short (e.g., 5-10 seconds) `.wav` file of the desired speaker's voice to perform voice cloning. This allows the robot to speak in a custom voice.

- Create or record a `.wav` file (e.g., using Audacity or your phone). Ensure it's clear and relatively noise-free.
- **Place in:** `robo_naila/server/models/tts/`
- **Example File:** `models/tts/my_custom_speaker_voice.wav`

#### d. Vision Model (Optional): Ultralytics YOLOv8 Nano (.pt format)

- **Model:** YOLOv8 Nano (yolov8n). This is a small, fast object detection model.
- **Download Location:** Ultralytics GitHub releases.
- **Example Link:** [Ultralytics YOLOv8 Releases](https://github.com/ultralytics/ultralytics/releases) (Look for `yolov8n.pt`).
- **Place in:** `robo_naila/server/models/vision/`
- **Example File:** `models/vision/yolov8n.pt`

### 3.7. Configure the Server (`config.py`)

Open the `config.py` file located in `robo_naila/server/` using a text editor (like VS Code). You must update the following settings to match your specific setup and downloaded model paths.

```python
# robot-project/server/config.py (example snippet)

# --- Network Settings ---
# MQTT Broker Settings: IP address of the machine running the MQTT broker.
# Use "localhost" or "127.0.0.1" if running on the same machine as the server.
# Otherwise, use the server's actual local IP address (e.g., "192.168.1.100").
MQTT_BROKER_HOST = "YOUR_SERVER_IP_ADDRESS_HERE" # e.g., "192.168.1.100" or "localhost"
MQTT_BROKER_PORT = 1883 # Default MQTT port

# OTA Update Server Settings: Port for the local HTTP server to serve firmware updates
OTA_SERVER_PORT = 8071 # Choose an available port, ensure no conflicts

# --- AI Model Paths ---
# IMPORTANT: Update these paths to point to your downloaded model files.
# Paths are relative to the 'server/' directory.

STT_MODEL_PATH = "models/stt/whisper-tiny.en.gguf" # Path to your Whisper GGUF model
LLM_MODEL_PATH = "models/llm/Meta-Llama-3-8B-Instruct-Q4_K_M.gguf" # Path to your LLM GGUF model

# TTS Model settings
TTS_MODEL_PATH = "models/tts/llama-outetts-1b-q4_k_m.gguf" # Path to your Llama-OuteTTS GGUF model
TTS_SPEAKER_REFERENCE_PATH = "models/tts/my_custom_speaker_voice.wav" # Path to your speaker reference WAV file

# Vision Model settings (Optional)
# Set to None if you are not using the Vision service.
VISION_MODEL_PATH = "models/vision/yolov8n.pt" # Path to your YOLOv8 model

# --- Other Settings (Optional, adjust as needed) ---
# ... other configurations for logging, thresholds, etc.
```

**Finding Your Server's IP Address:**

- **Windows:** Open Command Prompt and type `ipconfig`. Look for "IPv4 Address" under your active network adapter (e.g., "Wireless LAN adapter Wi-Fi").
- **macOS/Linux:** Open Terminal and type `ifconfig` or `ip a`. Look for `inet` address under your active network interface (e.g., `en0`, `wlan0`).

## 4. Firmware Setup (Robot Embedded Code)

This section details how to set up the development environment and flash the firmware onto your ESP32-S3 Sense board using PlatformIO.

### 4.1. Install PlatformIO

If you haven't already, install PlatformIO. The easiest way is via the VS Code Extension:

1. **Install Visual Studio Code:** If you don't have it, download and install VS Code from [code.visualstudio.com](https://code.visualstudio.com).

2. **Install PlatformIO IDE Extension:** Open VS Code, go to the Extensions view (Ctrl+Shift+X or Cmd+Shift+X), search for "PlatformIO IDE", and click "Install".

3. PlatformIO will prompt you to install its core components, which might take a few minutes.

### 4.2. Open Project in PlatformIO

1. Open VS Code.

2. Go to **File > Open Folder...** and select the `robo_naila/firmware/` directory.

3. PlatformIO will detect the project structure (`platformio.ini`) and set up the necessary build environment. This may involve downloading toolchains and frameworks for ESP32, which can take some time on the first load.

### 4.3. Configure Firmware (`config.h`)

Open the `config.h` file located in `robo_naila/firmware/`. You must update your WiFi credentials and the MQTT broker's IP address to allow the robot to connect to your network and the server.

```cpp
// robo_naila/firmware/config.h (example snippet)

#pragma once

// --- WiFi Settings ---
#define WIFI_SSID "YOUR_WIFI_NETWORK_NAME"     // REPLACE with your WiFi SSID
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD_HERE" // REPLACE with your WiFi password

// --- MQTT Settings ---
// IP address of the machine running the MQTT broker (your AI server).
// This MUST be the actual IP address, not "localhost".
#define MQTT_BROKER_IP "YOUR_SERVER_IP_ADDRESS" // e.g., "192.168.1.100" (same as in server/config.py)
#define MQTT_BROKER_PORT 1883               // Default MQTT port (same as in server/config.py)

// --- Robot Identification ---
#define ROBOT_ID "my_robot_001"             // A unique ID for your robot (used in MQTT topics)

// --- Other Hardware/Software Configurations (adjust as needed) ---
// Example: Display pins, motor pins, wake word parameters, etc.
// #define LCD_MOSI_PIN 10
// #define LCD_SCLK_PIN 11
// #define LCD_CS_PIN   12
// #define LCD_DC_PIN   13
// #define LCD_RST_PIN  14
// #define LCD_BL_PIN   15
// #define SERVO_PIN_X 21
// #define SERVO_PIN_Y 22
// #define WAKE_WORD_THRESHOLD 0.7
```

**Important:** The `MQTT_BROKER_IP` here must be the actual local IP address of your AI server machine, not `localhost`.

### 4.4. Build and Upload Firmware

1. **Connect ESP32-S3:** Connect your ESP32-S3 Sense board to your computer using a USB-C data cable. Ensure your computer recognizes it (you might need specific ESP-IDF drivers if on Windows, but PlatformIO usually handles this).

2. **Select Serial Port:** In the VS Code bottom toolbar, locate and select the correct serial port that corresponds to your ESP32-S3 board.

3. **Clean Build (Optional but Recommended):** Click the "Clean" (broom icon) button in the PlatformIO toolbar. This ensures a fresh build.

4. **Build:** Click the "Build" (check mark icon) button in the PlatformIO toolbar to compile the firmware. Monitor the "TERMINAL" tab for any errors.

5. **Upload:** Once the build is successful, click the "Upload" (right arrow icon) button in the PlatformIO toolbar to flash the compiled firmware onto your ESP32-S3 Sense board.

**Troubleshooting Uploads:** If the upload fails, try:

- Ensuring the correct serial port is selected.
- Pressing the "BOOT" button (if available) on your ESP32-S3 board immediately before or during the upload process (then release).
- Disconnecting and reconnecting the USB cable.
- Closing any other applications that might be using the serial port.

### 4.5. Monitor Serial Output

After uploading, it's crucial to monitor the robot's serial output for debug information and connectivity status.

Click the "Serial Monitor" (plug icon) button in the PlatformIO toolbar within VS Code. This will open a console displaying real-time messages from your ESP32-S3. You should see it:

- Connecting to your WiFi network.
- Attempting to connect to the MQTT broker at the specified IP address.
- Reporting its status and any errors.

This output is invaluable for diagnosing connectivity problems.

## 5. Running the Complete System

With both the server and firmware configured and ready, you can now bring NAILA to life!

### 5.1. Start MQTT Broker

Ensure your MQTT broker (Mosquitto) is actively running on your server machine. If you installed it as a system service, it should already be running. Otherwise, start it manually (refer to Step 3.5).

### 5.2. Start Local AI Server

1. Open a new terminal or command prompt window.

2. Navigate to your `robo_naila/server/` directory.

3. Activate your Python virtual environment:
   - **Linux/macOS:** `source .venv/bin/activate`
   - **Windows (Cmd):** `.\.venv\Scripts\activate`
   - **Windows (PowerShell):** `.\.venv\Scripts\Activate.ps1`

4. Run the main server application:

```bash
python main.py
```

The server will start initializing all the AI services (STT, LLM, TTS, optional Vision) by loading their respective models. This step can take a significant amount of time, especially if you have large models or no GPU acceleration. You should see log messages indicating successful model loading and MQTT client connection.

### 5.3. Start OTA Update Server (Optional, in a separate terminal)

If you plan on performing Over-The-Air (OTA) firmware updates for your robot, you need to run the dedicated HTTP server that hosts the firmware binaries. This should be done in a separate terminal window, concurrently with the main AI server.

1. Open another new terminal window.

2. Navigate to your `robo_naila/server/` directory.

3. Activate the same Python virtual environment:
   - **Linux/macOS:** `source .venv/bin/activate`
   - **Windows (Cmd):** `.\.venv\Scripts\activate`
   - **Windows (PowerShell):** `.\.venv\Scripts\Activate.ps1`

4. Run the OTA server script:

```bash
python update_server/ota_http_server.py
```

This server will typically listen on the port you configured in `config.py` (e.g., 8071). Keep this terminal window open for as long as you want OTA updates to be available.

### 5.4. Power on the Robot

With the MQTT broker and the AI server (and optionally the OTA server) running, now power on your ESP32-S3 Sense robot.

- The robot will attempt to connect to your configured WiFi network.
- Upon successful WiFi connection, it will try to connect to the MQTT broker at the IP address specified in its `config.h`.
- If all connections are successful, you should see communication flowing between the robot's serial monitor and the AI server's terminal. The robot is now ready to interact!

## 6. Troubleshooting Common Issues

### ESP32 "Could not connect to MQTT broker" / "WiFi connection failed" in Serial Monitor:

- **WiFi:** Double-check `WIFI_SSID` and `WIFI_PASSWORD` in `firmware/config.h` for typos.
- **MQTT IP:** Ensure `MQTT_BROKER_IP` in `firmware/config.h` is the actual local IP address of your AI server machine, not `localhost`.
- **MQTT Broker:** Verify Mosquitto is running on your server (see Step 3.5). Use `mosquitto_sub` to confirm it's active.
- **Firewall:** Check your host machine's firewall settings. Ensure that the MQTT port (default 1883) is open for incoming connections.
- **Network:** Confirm both the robot and server are on the same local network (e.g., connected to the same router).

### AI Server "Model not found" or "File not found" errors:

- Verify that `STT_MODEL_PATH`, `LLM_MODEL_PATH`, `TTS_MODEL_PATH`, `TTS_SPEAKER_REFERENCE_PATH`, and `VISION_MODEL_PATH` in `server/config.py` correctly point to the downloaded model files.
- Ensure the models are in the specified `models/` subdirectories (`stt/`, `llm/`, `tts/`, `vision/`).
- Check for any typos in the file paths or filenames. File names are case-sensitive on Linux/macOS.

### Python dependency errors on server:

- Always ensure your Python virtual environment is activated before running server scripts (see Step 3.3).
- If you added new dependencies, re-run `uv pip install -r requirements.txt` (see Step 3.4).

### Firmware upload fails (PlatformIO):

- Confirm your ESP32-S3 is connected with a data-capable USB-C cable.
- Select the correct serial port in the VS Code bottom toolbar.
- Try holding the "BOOT" button on the ESP32-S3 while clicking "Upload" in PlatformIO, then release it once the upload starts.
- Close any other applications that might be using the serial port (e.g., another serial monitor).

### Robot not responding or behaving unexpectedly:

- **Serial Monitor (ESP32):** Keep the PlatformIO serial monitor open. Look for communication errors, unexpected reboots, or task failures.
- **Server Terminal:** Check the AI server's terminal output for any errors during STT, LLM, or TTS processing that might prevent a response.
- **MQTT Topics:** Verify that the MQTT topics and message payloads used by the firmware and server code are consistent.

### Llama-OuteTTS output quality is poor or silent:

- Ensure `TTS_SPEAKER_REFERENCE_PATH` in `server/config.py` points to a valid, clear, and relatively short (5-10 seconds) `.wav` file of a human voice. The quality of this reference audio significantly impacts the output voice.
- The model may be sensitive to the quality/format of the reference audio. Try different `.wav` files if possible.
