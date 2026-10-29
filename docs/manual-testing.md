# Manual testing

Automated tests cover the models and the window. Use this checklist for anything that needs a real interface or a real screen. The in-app **Help > Quick Reference** (F1) describes the features themselves.

## Set up a virtual CAN interface

```
sudo scripts/setup_vcan.sh          # creates and brings up vcan0
sudo scripts/setup_vcan.sh vcan1    # or choose another name
```

The script is safe to run repeatedly. Remove an interface with `sudo ip link delete vcan0`. The interface does not survive a reboot. Tests that need `vcan0` fail (window test) or skip (socket test) when it is missing.

## Run the application

```
build/debug/app/CanLogVisualizer                    # default interface vcan0
build/debug/app/CanLogVisualizer --interface vcan1  # another interface
build/debug/app/CanLogVisualizer capture.log        # open a candump log file
```

Run with `--help` for all options.

## Generate traffic

Requires `can-utils`.

```
cangen vcan0 -g 10 -I 123            # ID 123 every 10 ms
cangen vcan0 -g 100 -I 456 -L 4      # ID 456 every 100 ms, 4 bytes
cansend vcan0 12345678#DEADBEEF      # one extended frame
candump -L vcan0 > capture.log       # record a log to open later
```
