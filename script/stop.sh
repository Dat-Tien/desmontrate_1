#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
ROOT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
RUN_DIR="$ROOT_DIR/.run"
PID_FILE="$RUN_DIR/arch_demo.pids"

SOCKETS=(
    /tmp/_audio_service.sock
    /tmp/_audio_service_cmd.sock
    /tmp/_hmi_service.sock
    /tmp/_hmi_service_cmd.sock
    /tmp/_power_service.sock
    /tmp/_power_service_cmd.sock
    /tmp/_region_service.sock
    /tmp/_region_service_cmd.sock
)

is_running() {
    local pid="$1"
    [[ -n "$pid" ]] && kill -0 "$pid" 2>/dev/null
}

if [[ ! -f "$PID_FILE" ]]; then
    echo "[stop] No PID file found: $PID_FILE"
    echo "[stop] Cleaning stale sockets only."
    rm -f "${SOCKETS[@]}"
    exit 0
fi

echo "[stop] Sending SIGTERM..."
while read -r name pid; do
    if is_running "$pid"; then
        # Kill the whole process group created by setsid in run.sh.
        kill -TERM -- "-$pid" 2>/dev/null || kill -TERM "$pid" 2>/dev/null || true
        echo "[stop] SIGTERM sent to $name pid=$pid"
    else
        echo "[stop] $name pid=$pid is not running"
    fi
done < "$PID_FILE"

# Wait up to 3 seconds for graceful shutdown.
for _ in 1 2 3 4 5 6; do
    still_running=0
    while read -r _name pid; do
        if is_running "$pid"; then
            still_running=1
            break
        fi
    done < "$PID_FILE"

    if [[ "$still_running" -eq 0 ]]; then
        break
    fi
    sleep 0.5
done

echo "[stop] Checking remaining processes..."
while read -r name pid; do
    if is_running "$pid"; then
        echo "[stop] $name pid=$pid did not exit, sending SIGKILL"
        kill -KILL -- "-$pid" 2>/dev/null || kill -KILL "$pid" 2>/dev/null || true
    fi
done < "$PID_FILE"

rm -f "$PID_FILE"
rm -f "${SOCKETS[@]}"

echo "[stop] Done."
