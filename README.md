# Energy Indicator

The energy indicator will be able to indicate information about energy consumtion to the user. This will be done using a internet connection. 

## Getting started
This project makes use of Espressif's own embedded framework called "ESP IDF". This framework is used to write, build, flash, and debug embedded applications on ESP SoC's and it is required in order to use this project. A tutorial on how to install and use it can be found at [Esp Idf: Getting started](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/index.html)


## Overview
![image](https://user-images.githubusercontent.com/45065264/214901735-14d080a3-8aa6-45b9-8559-48c41f92fb3d.png)

This is a high level overview of the energy indicator system. More detailed explainations can be found in the corresponding folders found in the main folder.

## Components
The components shown in the overview are described.

### Power source / USB Supply
Used to charge the internet battery of the system.

### MCU
Holds the program that will control the various indicators in the system. The indications are determined by the status of the battery, status of the internet connected and the data retrieved from the internet.

### Battery
Holds the energy used by the system. Can be recharged using the power source.

### WiFi module
The WiFi module is responsible for providing a internet connected to the system which will access data to determine the information shown in the Dishwasher indicator.

### Battery indicator
Indicates the current status of the battery, and wether is needs to be charged or not.

### WiFi indicator
This will indicate the current network status, which can either be, connected, API offline or disconnected.

### Dishwasher indicator
This will indicate all the information about the power consumtion of the user. This data will include: power efficiency in the next 12 hours, power efficiency of this current moment, and the power efficiency of the whole community.
