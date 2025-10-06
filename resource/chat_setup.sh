#!/bin/bash

set -e

CONFIG_DIR="${XDG_CONFIG_HOME:-$HOME/.config}/dappa"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

setup() {
    mkdir -p "$CONFIG_DIR"
    sqlite3 "$CONFIG_DIR/storage.db" < "$SCRIPT_DIR/schema.sql"
    sqlite3 "$CONFIG_DIR/server.db" < "$SCRIPT_DIR/schema.sql"
    touch "$CONFIG_DIR/psm"
    echo "Setup complete: $CONFIG_DIR"
}

wipe() {
    case $1 in
        1)
            rm -f "$CONFIG_DIR/storage.db"
            echo "Wiped storage.db"
            ;;
        0)
            rm -f "$CONFIG_DIR/server.db"
            echo "Wiped server.db"
            ;;
        *)
            echo "Usage: wipe [0|1]" >&2
            exit 1
            ;;
    esac
}

case $1 in
    setup)
        setup
        ;;
    wipe)
        wipe "$2"
        ;;
    *)
        echo "Usage: $0 {setup|wipe [0|1]}" >&2
        exit 1
        ;;
esac