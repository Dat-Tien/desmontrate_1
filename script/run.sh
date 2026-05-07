#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
ROOT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
BUILD_DIR="$ROOT_DIR/build"
RUN_DIR="$ROOT_DIR/.run"
LOG_DIR="$ROOT_DIR/logs"
PID_FILE="$RUN_DIR/arch_demo.pids"

mkdir -p "$RUN_DIR" "$LOG_DIR"

is_running() {
    local pid="$1"
    [[ -n "$pid" ]] && kill -0 "$pid" 2>/dev/null
}

if [[ -f "$PID_FILE" ]]; then
    while read -r name pid; do
        if is_running "$pid"; then
            echo "[run] $name is already running with PID $pid"
            echo "[run] Run ./stop.sh first if you want to restart."
            exit 1
        fi
    done < "$PID_FILE"
fi

if [[ ! -x "$BUILD_DIR/Application/_arch_demo" ]]; then
    echo "[run] Build output not found. Run ./build.sh first."
    exit 1
fi

rm -f "$PID_FILE"

start_process() {
    local name="$1"
    local exe="$2"
    local log="$LOG_DIR/${name}.log"

    if [[ ! -x "$exe" ]]; then
        echo "[run] Missing executable for $name: $exe"
        exit 1
    fi

    # setsid starts a new process group. stop.sh can later terminate the full group.
    setsid "$exe" > "$log" 2>&1 &
    local pid=$!
    echo "$name $pid" >> "$PID_FILE"
    echo "[run] started $name pid=$pid log=$log"
}

start_process audio_service "$BUILD_DIR/Services/AudioService/audio_service"
start_process hmi_service "$BUILD_DIR/Services/HmiService/hmi_service"
start_process power_service "$BUILD_DIR/Services/PowerService/power_service"
start_process region_service "$BUILD_DIR/Services/RegionService/region_service"

# Give services time to bind their Unix domain sockets before the application connects.
sleep 0.5

start_process arch_demo "$BUILD_DIR/Application/_arch_demo"

echo "[run] All processes started."
echo "[run] PID file: $PID_FILE"
echo "[run] Logs: $LOG_DIR"
echo "[run] Try: ./build/slld/slld audio AUDIO_BEEP"
echo "[run] Stop: ./stop.sh"
