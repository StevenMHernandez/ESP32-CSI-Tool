# Active CSI collection (AP)

Allows connection from some Station (STA) (ESP32 or potentially phone) and accepts packets (thus receiving CSI packet responses).

To use run `make flash monitor` from a terminal.

This sub-project most commonly pairs with the project in `./active_sta`. Flash these two sub-projects to two different ESP32s to quickly begin collecting Channel State Information.