#!/usr/bin/env bash
# Create and bring up a virtual CAN interface.
# Usage: sudo scripts/setup_vcan.sh [iface]   (default: vcan0)
# Teardown: sudo ip link delete vcan0
set -euo pipefail

iface="${1:-vcan0}"

if [[ $EUID -ne 0 ]]; then
    echo "error: must be run as root (try: sudo $0 ${iface})" >&2
    exit 1
fi

modprobe vcan

if ! ip link show "${iface}" &>/dev/null; then
    ip link add dev "${iface}" type vcan
fi

ip link set up "${iface}"

ip -details link show "${iface}"
