# ESP32 CSI Tool

[ESP32 CSI Tool Website](https://stevenmhernandez.github.io/ESP32-CSI-Tool/)

The purpose of this project is to allow for the collection of Channel State Information (CSI) from the ESP32 Wi-Fi enabled microcontroller. 
By collecting this data rich signal source, we can use this information for tasks such as Wi-Fi Sensing and Device-free Localization directly from the small, self contained ESP32 microcontroller.  

The following projects can be found in this repository:

* `./active_sta` - *Active CSI collection (Station)* - Connects to some Access Point (AP) (Router or another ESP32) and sends packet requests (thus receiving CSI packet responses). 
* `./active_ap` - *Active CSI collection (AP)* - AP which can be connected to by devices (ESP32, see previous).
* `./passive` - *Passive CSI collection* - Passively listens for CSI frames on a given channel (default: channel 3).

Each project automatically sends the collected CSI data to both serial port and SD card (if present). 
These settings can be configured as described below. 

In addition to these ESP32 specific projects, we also consider methods for analyzing CSI in Python and MATLAB (See **Analysing CSI Data** below). 

## Installation

First, Install Espressif IoT Development Framework (ESP-IDF) by following their [step by step installation guide](https://docs.espressif.com/projects/esp-idf/en/release-v4.3/esp32/get-started/index.html).
Notice, this project requires **version (v4.3) of ESP-IDF**.  

**Important:** It is important that you are able to successfully build and flash the example project from the esp-idf guide onto your own esp32.
If you have issues building the example project on your hardware, **do not create an issue in this github repo**.
We will not be able to assist with general ESP32 issues (those issues that are unrelated to this project).  

Next, clone this repository:

```
git clone https://github.com/StevenMHernandez/esp32-csi-tool
```

Finally, decide which sub-project you would like to flash to your ESP32(s). 
The simplest case would be to use two ESP32s. 
One using the *Active CSI collection (Station)* codebase and the other using the *Active CSI collection (AP)* codebase. 
To begin working with a given codebase, open a terminal and change into the respective directory.

```
cd ./active_sta # For Active Station
# OR
cd ./active_ap # For Active Access Point
# OR
cd ./passive # For Passive CSI collection
```

We can now begin configuring and flashing your ESP32.

## Configuration (ESP-IDF)

The ESP-IDF provides great control over project configuration. 
This configuration can be updated by running the following command from your terminal.

```
idf.py menuconfig
```

It is important to notice that these configurations are project specific and will not automatically be copied between sub-projects. 
So for example, make sure when you change the Wi-Fi password in *Active CSI collection (AP)*, you also update this configuration in the *Active CSI collection (Station)* project as well.

The following configurations are important for this project:

1. `Serial flasher config > 'idf.py monitor' baud rate > Custom Baud Rate`
2. `Serial flasher config > Custom baud rate value > 1552000` This allows more data to be transmitted on the Serial port
3. `Component config > Common ESP32-related > Channel for console output > Custom UART`
4. `Component config > Common ESP32-related > UART console baud rate > 1552000`
5. `Component config > Wi-Fi > WiFi CSI(Channel State Information)` (Press space to select)
6. `Component config > FreeRTOS > Tick rate (Hz) > 1000`
7. `ESP32 CSI Tool Config > ****` all options in this menu can be specified per your experiment requirements.

**NOTE:** For some systems, baud rate `1552000` does not work. Good alternatives to try are `921600`, `1000000`, `1152000`, and `1500000`.  
**The higher baud rate the better! Baud rate is extremely important to achieve high sampling rates without lag!**  
If you have a problem, please leave any relevant information such as operating system, esp-idf version, list of all baud rates work and baud rates that do not work etc in [issue #5](https://github.com/StevenMHernandez/ESP32-CSI-Tool/issues/5). 

## Flash ESP32

Run the following command from within one of the sub-project's directories.

```
idf.py flash monitor
```

This will flash the ESP32 and once completed, will print incoming data from the freshly programmed ESP32's serial port. 
To exit monitoring, use `ctrl+]`

## Collecting CSI Data

There are two methods to collect CSI data. 
If your ESP32 has an SD card on board (such as the TTGO T8 V1.7 ESP32), the ESP32 will automatically detect the SD card and automatically output CSI data to a simple csv file.

If the device does not have an SD card or you wish to collect the data directly from the Serial port on your computer, you can run the following command:

```
# macOS or Linux
idf.py monitor | grep "CSI_DATA" > my-experiment-file.csv

# Windows
idf.py monitor | findstr "CSI_DATA" > my-experiment-file.csv 
```

Because the clocks on the ESP32 are not synchronized with any real world time, it can be difficult to sync this data with other external data sources or sensors. 
To help with this, we can pass output first through a python script which appends a timestamp from your computer.

```
idf.py monitor | python ./python_utils/serial_append_time.py > my-experiment-file.csv
```

## Analysing CSI Data

Once data has been collected, we now wish to run analysis and (most likely) apply deep learning algorithms on the collected data. 
Luckily, the output from the esp32 is a simple CSV file, thus we can pass the contents to any available CSV parser in our language of choice (Python, MATLAB, R, etc.). 
The use of CSV was selected for its simplicity and small size when compared with the likes of XML or JSON.

## Advanced:

### Setting Local Time

Because the ESP32 is not connected to the internet as a whole, it is not possible to automatically set the clock time locally.
To handle this, we offer the ability to set the time in a couple of different ways.

First, while running `idf.py monitor` we can type the following `SETTIME: 123123123123` then `ENTER` where the number 123123123123 indicates the current UNIX time in seconds.

Additionally, the access point code in `./active_ap` will automatically send its current timestamp to any connected station running the `./active_sta` sub-project.
This means that you only need to set the time for the access point and all other nodes will synchronize automatically.

Finally, the simplest method is to simply run the output of `idf.py monitor` through a utility function which appends the correct timestamp to the output when received on your computer as described in the **Collecting CSI Data* section above.

### Misc.

[ESP32 CSI Tool](https://stevenmhernandez.github.io/ESP32-CSI-Tool/) developed by [Steven M. Hernandez](https://github.com/StevenMHernandez)

[Cite this Tool with BibTeX](https://raw.githubusercontent.com/StevenMHernandez/ESP32-CSI-Tool/master/docs/bibtex/esp32_csi_tool_wowmom.bib)