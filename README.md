# Embedded Architecture Demo

This project demonstrates a simplified Android/Linux embedded-style application architecture inspired by an automotive software system.

The purpose of this repository is to show how a main application can communicate with independent service processes through IPC, normalize service messages through wrapper/adaptor layers, process events asynchronously through a queue, dispatch logic by region, and manage behavior using a state machine.

---

## 1. Architecture Overview

The project is organized around the following flow:

```text
User / SLLD
    ↓
Service Processes
    ↓ IPC Unix Domain Socket
Application Wrappers
    ↓
EventHandler
    ↓
MessageQueue<AppMessage>
    ↓
Application Thread
    ↓
ProcessorManager
    ↓
Region Processor
    ↓
StateMachine / TimerManager / Actions
```

### Main Components

| Component | Responsibility |
|---|---|
| `Application` | Owns lifecycle, dependency startup, callbacks, queue, app thread, and graceful shutdown |
| `Wrappers` | IPC client adaptors that receive service messages and convert them to `AppMessage` |
| `EventHandler` | Centralized event posting point before pushing events into the queue |
| `MessageQueue` | Thread-safe bounded queue for asynchronous event processing |
| `ProcessorManager` | Dispatches messages to the active region processor |
| `Processor` | Handles region-specific business logic and state transitions |
| `StateMachine` | Executes deterministic transitions based on current state and event |
| `TimerManager` | Manages timers, pause/resume/cancel, and posts timeout events |
| `SLLD` | Command-line signal injection tool used to emulate service signals |

---

## 2. Project Structure

```text
.
├── Application
│   ├── app
│   │   ├── include
│   │   │   ├── application.hpp
│   │   │   ├── event_handler.hpp
│   │   │   ├── processor_base.hpp
│   │   │   ├── processor_manager.hpp
│   │   │   ├── state_machine.hpp
│   │   │   └── timer_manager.hpp
│   │   └── src
│   │       ├── application.cpp
│   │       ├── event_handler.cpp
│   │       ├── main.cpp
│   │       ├── processor_manager.cpp
│   │       ├── state_machine.cpp
│   │       └── timer_manager.cpp
│   ├── services
│   │   ├── include
│   │   │   ├── audio_wrapper.hpp
│   │   │   ├── hmi_wrapper.hpp
│   │   │   ├── power_wrapper.hpp
│   │   │   └── region_wrapper.hpp
│   │   └── src
│   │       ├── audio_wrapper.cpp
│   │       ├── hmi_wrapper.cpp
│   │       ├── power_wrapper.cpp
│   │       └── region_wrapper.cpp
│   └── CMakeLists.txt
├── Common
│   ├── include
│   │   ├── logger.hpp
│   │   └── types.hpp
│   └── src
│       ├── logger.cpp
│       └── types.cpp
├── Services
│   ├── AudioService
│   ├── HmiService
│   ├── PowerService
│   └── RegionService
├── slld
│   ├── include
│   └── src
├── script
│   ├── build.sh
│   ├── run.sh
│   ├── stop.sh
│   └── status.sh
├── README.md
└── CMakeLists.txt
```

---

## 3. Build Requirements

The project requires:

- Linux or WSL
- CMake
- C++17 compiler
- pthread support

On Ubuntu or WSL:

```bash
sudo apt update
sudo apt install -y build-essential cmake
```

Verify:

```bash
g++ --version
cmake --version
```

---

## 4. Build

From the project root:

```bash
./script/build.sh
```

This configures and builds all targets into the `build/` directory.

---

## 5. Run

Start all demo processes:

```bash
./script/run.sh
```

This starts:

- `_arch_demo`
- `audio_service`
- `hmi_service`
- `power_service`
- `region_service`

Logs are written to:

```text
logs/
```

PID files are stored in:

```text
.run/
```

Check running status:

```bash
./script/status.sh
```

Stop all processes:

```bash
./script/stop.sh
```

---

## 6. SLLD Command Tool

`slld` is a small command-line tool used to emulate service-originated signals.

Command format:

```bash
./build/slld/slld <service> <payload1> [payload2] [payload3]
```

The first argument selects the service handle.  
The remaining arguments are sent as payload to the corresponding service command socket.

Examples:

```bash
./build/slld/slld power IG_ON
./build/slld/slld hmi START_REQUEST
./build/slld/slld hmi STOP_REQUEST
./build/slld/slld audio AUDIO_PLAY_COMPLETED
./build/slld/slld power IG_OFF
```

Supported services:

```text
power
hmi
audio
region
```

---

## 7. Demo Scenario

The main demo flow shows a complete state transition sequence.

### Scenario

```text
1. User sends IG_ON through PowerService
2. Application receives PowerIgnitionOn
3. State changes Idle -> IgnitionOn

4. User sends START_REQUEST through HMI
5. Application receives ApplicationStartRequest
6. State changes IgnitionOn -> Running

7. After 10 seconds, user sends STOP_REQUEST through HMI
8. State changes Running -> Stopping
9. Processor starts a 5-second stop-completion timer

10. Timer expires
11. State changes Stopping -> WaitingAudioComplete
12. Processor requests AudioService to play a stop-completed sound

13. AudioService finishes playing sound
14. AudioService sends AUDIO_PLAY_COMPLETED callback
15. State changes WaitingAudioComplete -> Idle
```

### Commands

Run the system:

```bash
./script/run.sh
```

Send ignition on:

```bash
./build/slld/slld power IG_ON
```

Send start request:

```bash
./build/slld/slld hmi START_REQUEST
```

Wait around 10 seconds, then send stop request:

```bash
./build/slld/slld hmi STOP_REQUEST
```

Expected final state:

```text
Idle
```

Stop the demo:

```bash
./script/stop.sh
```