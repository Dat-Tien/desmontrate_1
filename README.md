#  Architecture Demo with IPC

This project is a simplified C++ automotive architecture demo based on an event-driven embedded application pattern.

## Workspace structure

- **Application/**: main application that owns the app thread, queue, wrappers, processor manager, and region-specific processors
- **AudioService/**: separate audio service application that sends events over IPC
- **run_demo.sh**: helper script to build both applications and run the IPC demo

## Architecture overview

### Main application
- **services/**: wrapper layer for external callbacks such as HMI and region, plus `ServiceOneWrapper` as the IPC adapter
- **main.cpp**: owns the application thread and receives normalized messages from wrappers
- **processor_manager.cpp**: central dispatcher that routes messages by current region
- **eu_processor.cpp / jp_processor.cpp**: region-specific processors
- **state_machine.hpp**: table-driven state machine used inside each processor

### Audio service
- **audio_service.cpp**: standalone application exposing a Unix domain socket at `/tmp/_audio_service.sock`

## Runtime flow

1. `AudioService` runs as a separate process.
2. `ServiceOneWrapper` connects to `AudioService` over Unix domain socket IPC.
3. When `AudioService` publishes an event like `AUDIO_BEEP`, `ServiceOneWrapper` receives it.
4. `ServiceOneWrapper` converts the IPC payload into an internal `AppMessage` and pushes it to the application queue.
5. `main.cpp` consumes messages on the app thread and forwards them to `ProcessorManager`.
6. `ProcessorManager` checks the current region.
7. The message is dispatched to `DestThreeProcessor` or `DestOneProcessor`.
8. The processor handles the event through a state machine transition.

## Build all targets

```bash
cmake -S . -B build
cmake --build build
```

## Run the full IPC demo

```bash
./run_demo.sh
```

## Run manually in two terminals

Terminal 1:
```bash
./build/AudioService/audio_service
```

Terminal 2:
```bash
./build/Application/_arch_demo
```

## Suggested Git initialization

```bash
git init
git add .
git commit -m "Add  architecture demo with IPC-based audio service"
```
