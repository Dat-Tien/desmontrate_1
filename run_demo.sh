#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")" && pwd)"
BUILD_DIR="$ROOT_DIR/build"
LOG_FILE="$ROOT_DIR/log.txt"

cleanup() {
    [[ "$CLEANED_UP" -eq 1 ]] && return
    CLEANED_UP=1

    echo "[run_demo] cleaning up..."

    [[ -n "${APP_PID:-}" ]] && kill -TERM "$APP_PID" 2>/dev/null || true
    [[ -n "${AUDIO_PID:-}" ]] && kill -TERM "$AUDIO_PID" 2>/dev/null || true

    sleep 1

    [[ -n "${APP_PID:-}" ]] && kill -0 "$APP_PID" 2>/dev/null && kill -KILL "$APP_PID" 2>/dev/null || true
    [[ -n "${AUDIO_PID:-}" ]] && kill -0 "$AUDIO_PID" 2>/dev/null && kill -KILL "$AUDIO_PID" 2>/dev/null || true

    wait "${APP_PID:-}" 2>/dev/null || true
    wait "${AUDIO_PID:-}" 2>/dev/null || true
}

trap cleanup EXIT INT TERM

cmake -S "$ROOT_DIR" -B "$BUILD_DIR"
cmake --build "$BUILD_DIR"

"$BUILD_DIR/Services/AudioService/audio_service" &
AUDIO_PID=$!

sleep 0.3

"$BUILD_DIR/Application/_arch_demo" 2>&1 | tee "$LOG_FILE" &
APP_PID=$!

echo "Audio PID: $AUDIO_PID"
echo "App PID: $APP_PID"

wait "$APP_PID" || true
wait "$AUDIO_PID" || true