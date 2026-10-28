# CAN Core Library

Provides the reusable CAN functionality shared by the application and tests: classic CAN frame types and formatting, raw SocketCAN reception, `candump` log file reading and writing, ID/mask filtering, the trace table model, and bus statistics (frame rate, bus load, per-ID count and cycle time). Public headers are under `include/`; implementations and tests are kept separately.
