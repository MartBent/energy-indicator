# Main

## Program flow
Since the device should consume the least energy that is possible it utilizes the deep sleep mode of the ESP to save power when it is not required to do anything. Because of this sleep mode the RAM memory is wiped everytime the device goes to sleep, this means it fully restarts at the app_main() function whenever it wakes up from deepsleep.

The ESP determine what is should do on wakeup by looking at the reason the device woke up. A overview of how the device handles different types of wakeup reasons is given below:

![wakeupflow drawio](https://user-images.githubusercontent.com/45065264/214599025-34540cca-b4a2-4747-bf44-62e8ad422f69.png)

### No user settings
Whenever the device has no user settings it will be turned into a Wi-Fi access point (or hotspot). This hotspot is used to connect to the device and set user settings. More information can be found in the network folder.

### Wakeup reasons

#### Sensor
To preserve energy for the device is LED strip is only turned on when someone walks past it. This is done using an passive infrared sensor. More information can be found in the hardware folder.

#### Reset button
To reset the user's Wi-Fi settings a reset button can be used. This button should be held down for 5 seconds for it to work. If it is held down an animation on the LED strip will be shown to indicate that the settings have been reset. The device will turn on into a Wi-Fi access point.

#### Timer / Startup
When the device is startup for the first time is will immediately try to retrieve weather data from the internet. This data is saved in retaining memory. This data will be refreshed every 30 minutes. This is done so that the device does not have to retrieve data everytime someone triggers the presence sensor.


