#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
ROOT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
PID_FILE="$ROOT_DIR/.run/arch_demo.pids"

if [[ ! -f "$PID_FILE" ]]; then
    echo "[status] Not running. PID file not found."
    exit 0
fi

while read -r name pid; do
    if [[ -n "$pid" ]] && kill -0 "$pid" 2>/dev/null; then
        echo "[status] $name running pid=$pid"
    else
        echo "[status] $name stopped pid=$pid"
    fi
done < "$PID_FILE"
