graph TD
    subgraph Robot
        A[Microphone] --> B(Audio Capture & Pre-processing)
        C[Camera] --> D(Image Capture & Basic Vision)
        E[Motors & Actuators] <-- F(Action Executor)
        G[Display] --> H(UI render)
        I(Sensors: IMU, Touch, etc.) --> J(Sensor Data Processing)
        K(Wake Word Detection - TinyML) --> L(Command Orchestrator)
        M(Network Client: Wi-Fi) --> N(MQTT Client / WebSocket Client / HTTP Client)
        B --> K
        D --> K
        K --> L
        J --> L
        L --> N
        N <--> P(MQTT Broker / WebSocket Server / HTTP Server)
        P --> Q(STT Service - Local)
        P --> R(NLU/LLM Service - Local)
        P --> S(TTS Service - Local)
        P --> T(Advanced Vision Service - Local)
        Q --> R
        R --> S
        S --> N
        T --> N
        R --> F
        R --> H
        N <--> U(OTA Update Server - Local)
        U --> V{Firmware Storage}
    end

    subgraph Local AI Server
        P
        Q
        R
        S
        T
        U
        V
    end

    subgraph Communication Protocols
        W[MQTT, WebSockets, HTTP]
        X[File Transfer for OTA]
    end