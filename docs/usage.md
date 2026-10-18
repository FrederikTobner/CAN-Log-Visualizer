# Usage

## Set up a virtual CAN interface

```
sudo scripts/setup_vcan.sh          # creates and brings up vcan0
sudo scripts/setup_vcan.sh vcan1    # or choose another name
```

The script is safe to run repeatedly. Remove an interface with `sudo ip link delete vcan0`. The interface does not survive a reboot.

## Run the application

```
build/debug/app/CanLogVisualizer
build/debug/app/CanLogVisualizer --interface vcan1
```

The interface defaults to `vcan0`. Run with `--help` for all options. Press **Connect** to start receiving frames.

## Generate traffic

With `can-utils` installed, start `candump` in one terminal before sending
frames; start the application and connect to the same interface to view them.
Quote each `cansend` frame argument: shells such as zsh treat `#` specially
when it is unquoted.

```
cangen vcan0 -g 10          # random frames every 10 ms
cansend vcan0 '123#DEADBEEF'  # standard frame (11-bit ID)
cansend vcan0 '12345678#11'   # extended frame (8 hex digits = 29-bit ID)
cansend vcan0 '123#R'         # remote frame
candump vcan0               # compare with what the application shows
```
