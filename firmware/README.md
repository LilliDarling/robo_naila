| Supported Target | ESP32-S3 |
| ---------------- | -------- |

# NAILA Robot Firmware

This directory contains the firmware source code for the robot's embedded system, designed to run on the Seeed Studio XIAO ESP32-S3 Sense board. It is built using the Espressif IDF (IoT Development Framework) and manages the robot's hardware interactions, on-device processing, and communication with the distributed AI orchestration system.

## ⚠️ Important Note: Environment Setup

Before you can build this firmware, you *must* have the ESP-IDF development environment properly set up on your system (Linux, macOS, or Windows). This includes:

* Installing necessary system-level dependencies (e.g., `git`, `cmake`, `ninja`, `dfu-util`, `libusb` and development files).
* Cloning the ESP-IDF repository.
* Running the `install.sh` (or equivalent) script to download and configure the Xtensa toolchain and Python virtual environment.
* Sourcing the `export.sh` script to set up your environment variables.

Refer to the official ESP-IDF "Get Started" guide for detailed instructions specific to your operating system:
[Espressif Docs](https://docs.espressif.com/projects/esp-idf/en/stable/esp32s3/index.html)  


## Project Structure

This is a standard ESP-IDF project structure aligned with the new multi-agent architecture. Key directories:

* **`main/`**: Contains the core application logic of the robot.
    * `robot_main.c`: The main entry point and overall control flow.
    * `communications/`: Handles Wi-Fi connectivity, MQTT client implementation for communicating with the AI orchestration layer, and OTA update client logic.
    * `peripherals/`: Drivers and abstraction layers for the board's hardware:
        * `audio_processing.c`: Real-time audio capture and preprocessing
        * `sensor_manager.c`: Unified interface for touch, IR, and environmental sensors
        * `vision_module.c`: On-device computer vision processing and camera management
        * `motor_control.c`: Servo coordination, movement patterns, and gesture execution
        * `display_driver.c`: Screen control for expressive animations
    * `ai_inference/`: Contains on-device AI capabilities:
        * Wake word detection models and inference
        * Basic vision preprocessing
        * Local safety and emergency response logic
    * `security/`: Device-level security implementations
    * ... (other robot-specific logic as it evolves)
* **`components/`**: Custom components or Git submodules for external C/C++ libraries not part of the ESP-IDF components registry.
* **`idf_component.yml`**: Defines dependencies on components from the Espressif Components Registry. The ESP-IDF build system automatically downloads and manages these.
* **`partitions.csv`**: Defines the flash memory layout of the ESP32-S3. **Crucial for Over-The-Air (OTA) updates**, as it defines multiple application partitions.
* **`CMakeLists.txt`**: The build system configuration file for this project. It defines how source files are compiled and linked.
* **`sdkconfig.defaults`**: Default configuration options for `menuconfig`. Useful for maintaining consistent settings.

## Building and Flashing

Once your ESP-IDF environment is set up and active, follow these steps to build and flash the firmware onto your ESP32-S3 Sense board.

1.  **Navigate to this project directory:**
    ```bash
    cd robo_naila/firmware/
    ```

2.  **Set the target chip:**
    This specifies that you are building for the ESP32-S3.
    ```bash
    idf.py set-target esp32s3
    ```

3.  **Connect your ESP32-S3 board:**
    Connect the board to your computer via a USB cable.

5.  **Identify the serial port:**
    Determine the serial port your board is connected to.
    * **Linux:** Typically `/dev/ttyUSB0`, `/dev/ttyACM0`. Use `ls /dev/tty*` before and after connecting the board.
    * **macOS:** Typically `/dev/cu.usbserial-XXXX` or `/dev/cu.usbmodemXXXX`. Use `ls /dev/cu.*`.
    * **Windows:** Typically `COMx` (e.g., `COM3`). Check Device Manager.

    *Permissions Note for Linux:* If you encounter permission errors when flashing, you might need to add your user to the `dialout` group:
    `sudo usermod -a -G dialout $USER` (then log out and log back in).

6.  **Build, Flash, and Monitor:**
    This command will build the firmware, flash it to your ESP32-S3, and then open a serial monitor to display output from the board. **Replace `/dev/ttyUSB0` with your actual serial port.**
    ```bash
    idf.py -p /dev/ttyUSB0 flash monitor
    ```
    To exit the serial monitor, press `Ctrl+]`.

## Over-The-Air (OTA) Updates

This firmware is designed to support OTA updates from the local AI server. The `partitions.csv` file is configured for two application partitions (`app0` and `app1`).

* The `communications/ota_updater.c` module handles the client-side logic for checking for updates, downloading new firmware from the AI orchestration server (via HTTP), and rebooting into the new image.
* The AI orchestration system (in the `../ai-server/` directory) manages OTA updates through its device management component. Refer to the server's documentation for triggering OTA updates.