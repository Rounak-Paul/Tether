#!/usr/bin/env bash
# Simple helper: typecheck and parse a .tx file and show output
set -euo pipefail
if [ "$#" -lt 1 ]; then
  echo "usage: run_tx.sh <file.tx>"
  exit 2
fi
FILE="$1"
# Run parser and typechecker
./parser-runner "$FILE"
./typecheck-runner "$FILE"
