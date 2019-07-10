# ESP32 CSI Tool

The purpose of this project is to allow for the collection of Channel State Information (CSI) from the ESP32 Wi-Fi enabled microcontroller. 
By collecting this data rich signal source, we can use this information for tasks such as Wi-Fi Sensing and Device-free Localization directly from the small, self contained ESP32 microcontroller.  

The following projects can be found in this repository:

* `./active_sta` - *Active CSI collection (Station)* - Connects to some Access Point (AP) (Router or another ESP32) and sends packet requests (thus receiving CSI packet responses). 
* `./active_ap` - *Active CSI collection (AP)* - AP which can be connected to by devices (ESP32, see previous).
* `TODO` - *Passive CSI collection* - Collects ambient CSI packets (promiscuous mode) without formally requesting.

Each project automatically sends the collected CSI data to both serial port and SD card (if present). 
These settings can be configured as described below. 

In addition to these ESP32 specific projects, we also consider methods for analyzing CSI in Python and MATLAB (See **Analysing CSI Data** below). 

## Installation

First, Install Espressif IoT Development Framework (ESP-IDF) by following their [https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html#installation-step-by-step](step by step installation guide). 

Next, clone this repository:

```
git clone https://github.com/StevenMHernandez/esp32-csi
```

Finally, decide which sub-project you would like to flash to your ESP32(s). 
The simplest case would be to use two ESP32s. 
One using the *Active CSI collection (Station)* codebase and the other using the *Active CSI collection (AP)* codebase. 
To begin working with a given codebase, open a terminal and change into the respective directory.

```
cd DIR/TO/CODEBASE
```

We can now begin configuring and flashing your ESP32.

## Configuration (ESP-IDF)

The ESP-IDF provides great control over project configuration. 
This configuration can be running the following command from a terminal.

```
make menuconfig
```

It is important to notice that these configurations are project specific and will not automatically be copied between sub-projects. 
So for example, make sure when you change the Wi-Fi password in *Active CSI collection (AP)*, you also update this configuration in the *Active CSI collection (Station)* project as well.

The following configurations are important for this project:

1. `Serial flasher config > Default serial port` This port may not be correct if you have multiple ESP32s connected to your computer. Find you port by running the following comment: `ls /dev/cu.*` while the ESP32 is connected. The port will look something like the following: **/dev/cu.SLAB_USBtoUART**
2. `Serial flasher config > Default baud rate > 921600 baud` This allows more data to be transmitted on the Serial port (this needs to be set in 3 places total)
3. `Serial flasher config > 'make monitor' baud rate > 921600 baud` This allows more data to be transmitted on the Serial port (this needs to be set in 3 places total)
4. `Component config > Common ESP-related > UART console baud rate > 921600` This allows more data to be transmitted on the Serial port (this needs to be set in 3 places total)
5. `Component config > Wi-Fi > WiFi CSI(Channel State Information)` (Press space to select)
6. `ESP32 CSI Tool Config > ****` all options in this menu can be specified per your experiment requirements.

## Flash ESP32

Run the following command from within one of the sub-project's directories.

```
make flash monitor
```

This will flash the ESP32 and once completed, will print the data transmitted from the ESP32 through the serial port. 
To exit monitoring, use `ctrl+]`

## Collecting CSI Data

There are two methods to collect CSI data. 
If your ESP32 has an SD card on board (such as the TTGO T8 V1.7 ESP32), the program will automatically detect this SD card and output CSI data to a simple csv file.

If the device does not have an SD card or you wish to collect the data directly from the Serial port, you can run the following command:

```
make monitor | grep "CSI_DATA" > my-experiment-file.csv 
```

Because the clocks on the ESP32 are not synchronized with any real world time, it can be difficult to sync this data with other external data sources or sensors. 
To help us with this, we can pass output first through a python script which appends a timestamp from your computer.

```
make monitor | python ./_utils/serial_append_time.py > my-experiment-file.csv
```

## Analysing CSI Data

Once data has been collected, we now wish to run analysis and most likely apply deep learning algorithms on the collected data. 
Luckily, the output from the esp32 is a simple CSV file, thus we can pass the contents to any available CSV parser in our language of choice (Python, MATLAB, R, etc.). 
The use of CSV was selected for its simplicity and small size when compared with the likes of XML or JSON.

## Additional information:

### Setting Local Time

Because the ESP32 is not connected to the internet as a whole, it is not possible to automatically set the clock time locally.
To handle this, we offer the ability to set the time in a couple of different ways.

First, while running `make monitor` we can type the following `SETTIME: 123123123123` then `ENTER` where the number 123123123123 indicates the current time in seconds.

Additionally, the access point code in `./active_ap` will automatically send its current timestamp to any connected station running the `./active_sta` sub-project.
This means that you only need to set the time for the access point and all other nodes will synchronize automatically.

Finally, the simplest method is to simply run the output of `make monitor` through a utility function which appends the correct timestamp to the output when received on your computer as described in the **Collecting CSI Data* section above.

## Previous Works

Jonathan Muller has begun some useful investigation on collecting CSI on the ESP32 [https://github.com/jonathanmuller/ESP32-gather-channel-state-information-CSI-](https://github.com/jonathanmuller/ESP32-gather-channel-state-information-CSI-). 
Muller's work appears to primarily be concerned with the collection of CSI packets in a general sense for analysis in softwares such as Wireshark. 
This work on the other hand hopes to create a simple system for researchers to collect CSI packet data for use in tasks such as Wi-Fi Sensing and Device-free Localization. 

[Linux 802.11n CSI Tool](https://dhalperi.github.io/linux-80211n-csitool/) provides a method for researchers to collect CSI packet data from the Intel 5300 Network Interface Card (NIC). 
[Atheros CSI Tool](https://wands.sg/research/wifi/AtherosCSI/) is very similar targeting Atheros NICs. 
These tools are useful in collecting CSI packet data with hardware available on hand, but are limited in their capacity by their reliance on keeping the NICs to a computer or laptop. 
This limitation results in much larger size for a CSI-collecting unit as well as increases in the cost per unit. 

