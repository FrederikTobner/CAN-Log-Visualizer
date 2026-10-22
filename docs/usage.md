# Usage

## Set up a virtual CAN interface

```
sudo scripts/setup_vcan.sh          # creates and brings up vcan0
sudo scripts/setup_vcan.sh vcan1    # or choose another name
```

The script is safe to run repeatedly. Remove an interface with `sudo ip link delete vcan0`. The interface does not survive a reboot.

## Run the application

```
build/debug/app/CanLogVisualizer                    # Run the application using the default interface vcan0
build/debug/app/CanLogVisualizer --interface vcan1  # Run the application using the interface vcan1
build/debug/app/CanLogVisualizer capture.log        # Open a candump log file
```

The interface defaults to `vcan0`. Run with `--help` for all options. Press **Connect** to start receiving frames.

## Generate traffic

With `can-utils` installed, start `candump` in one terminal before sending any frames.
Then start the application and connect to the same interface to view them.

```
cangen vcan0 -g 1000          # random frames every second (1000 ms)
cansend vcan0 '123#DEADBEEF'  # standard frame (11-bit ID)
cansend vcan0 '12345678#11'   # extended frame (8 hex digits = 29-bit ID)
cansend vcan0 '123#R'         # remote frame
candump vcan0                 # compare with what the application shows
```

## Filter the trace

Type a filter into the field next to the Connect button and press Enter. The table then shows only matching frames; clearing the field shows everything again. The frames themselves are kept, and the `#` column still shows the original frame numbers.

| Filter | Shows |
| --- | --- |
| `123` | Standard frames with ID `123` |
| `700:700` | Standard frames where `ID & 700 == 700`, i.e. `700` to `7FF` |
| `12345678` | The extended frame `12345678` |
| `123, 456` | Frames matching any of the listed IDs |
| `!7FF` | Everything except ID `7FF` (an exclude always wins) |

IDs and masks are hexadecimal. Up to 3 digits select standard frames and 8 digits select extended frames, so `123` never matches the extended frame `00000123`. This differs from `candump`, where a mask such as `123:7FF` also matches extended frames. Invalid input is marked red, explained in the tooltip and status bar, and the previous filter stays active.

## Log files

The application reads and writes the log format of `candump -l` and `candump -L`, one frame per line:

```
(1792411168.454309) vcan0 123#DEADBEEF
```

```
candump -l vcan0              # record to candump-<date>_<time>.log
candump -L vcan0 > run.log    # the same format on stdout
canplayer -I run.log          # replay a log onto vcan0
```

- **File > Open Log...** (Ctrl+O) loads a log into the table, as does passing the file on the command line. Opening a file disconnects from the interface and replaces the trace. Connecting again replaces the loaded file with live frames, because their timestamps do not belong together.
- Up to 1,000,000 frames are kept when loading; for larger files only the last ones are shown, and the status bar says so.
- Lines that cannot be read (CAN FD frames, error frames, garbage) are skipped. The status bar reports how many and why the first one failed.
- **File > Save Log As...** (Ctrl+Shift+S) saves the frames currently shown, so an active filter applies. The interface name in the file is taken from the interface field.
