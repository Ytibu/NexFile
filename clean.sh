#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")" && pwd)"

echo "Cleaning *.out and *.o under: $ROOT_DIR"
find "$ROOT_DIR" -type f \( -name "*.out" -o -name "*.o" \) -print -delete
echo "Done."
