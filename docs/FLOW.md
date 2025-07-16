# NAILA Robot - Prototype Flow

## Core System Architecture

```mermaid
graph TD
    subgraph "ESP32-S3 Robot"
        A[Microphone] --> B(Audio Capture)
        C[Camera] --> D(Image Capture)
        E[Motors & Actuators] <-- F(Action Executor)
        G[Display] --> H(UI Renderer)
        I[Sensors: IMU, Touch, IR] --> J(Sensor Processing)
        K(Wake Word Detection) --> L(Command Orchestrator)
        M(Wi-Fi) --> N(MQTT Client)
        
        B --> K
        D --> K
        K --> L
        J --> L
        L --> N
    end

    subgraph "MQTT Communication"
        N <--> BROKER[MQTT Broker]
        BROKER <--> AI_CLIENT[AI Server Client]
    end

    subgraph "AI Server - Core Services"
        AI_CLIENT --> STT[STT - Whisper]
        AI_CLIENT --> VISION[Vision - YOLOv8]
        STT --> LLM[LLM - Llama 3]
        VISION --> LLM
        LLM --> TTS[TTS - OuteTTS]
        TTS --> AI_CLIENT
        LLM --> ACTIONS[Action Commands]
        ACTIONS --> AI_CLIENT
    end

    subgraph "Support Services"
        LLM --> OTA[OTA Updates]
        OTA --> FIRMWARE[(Firmware Storage)]
    end
```

## Basic Interaction Flow

```mermaid
sequenceDiagram
    participant User
    participant Robot as ESP32-S3 Robot
    participant MQTT as MQTT Broker
    participant AI as AI Server

    User->>Robot: "Hey Naila, what time is it?"
    Robot->>MQTT: Raw audio data
    MQTT->>AI: Process STT
    AI->>MQTT: Text: "what time is it?"
    MQTT->>AI: Process with LLM
    AI->>MQTT: Response + TTS audio
    MQTT->>Robot: Play audio + display action
    Robot->>User: "It's 3:15 PM"
```

## MQTT Topics (Prototype)

```
naila/
├── robot/
│   ├── audio/raw          # Raw audio from microphone
│   ├── vision/raw         # Raw images from camera
│   ├── sensors/data       # Sensor readings
│   └── status/            # Robot status updates
├── ai/
│   ├── stt/result         # Speech recognition results
│   ├── vision/result      # Vision analysis results
│   ├── tts/audio          # Generated speech audio
│   └── actions/           # Action commands for robot
└── system/
    ├── ota/               # Over-the-air updates
    └── health/            # System health monitoring
```