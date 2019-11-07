# Passive CSI collection

Listens passively for packets on channel 3 (same channel as both active_ap and active_sta). This channel can be changed in `main/main.c` depending on the channel of the device you wish to passively listen for.

The easiest way to evaluate this sub-project is to flash three ESP32s. One with active_ap, one with active_sta and finally one with this passive sub-project.

To use run `make flash monitor` from a terminal.
