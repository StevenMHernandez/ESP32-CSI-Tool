# Active CSI collection (Station)

Connects to some Access Point (AP) (Router or another ESP32) and sends packet requests (thus receiving CSI packet responses).

To use run `make flash monitor` from a terminal.

This sub-project most commonly pairs with the project in `./active_ap`. Flash these two sub-projects to two different ESP32s to quickly begin collecting Channel State Information.