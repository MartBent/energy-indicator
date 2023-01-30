# Hardware

## Overview
The ESP is connected to several hardware components, how these components are wired up is described below:

![image jpeg](https://user-images.githubusercontent.com/45065264/214882115-4a8cf49a-1bd1-4b1d-9f39-1ae300eaf52d.jpg)

### Pin numbering
Below is a table describing each GPIO pin and its function in this device. mind that these pin numberings are NOT the physical ESP pins, but GPIO pins.

| Gpio n  | Function | Direction in relation to ESP |
|---|---|---|
| 22  | Led strip data in | OUT |
| 26  | Display BUSY | IN |
| 27  | Display RESET | OUT |
| 25  | Display Data Command | OUT |
| 15  | Display Chip select | OUT |
| 23  | Display Master out slave in | OUT |
| 18  | Display Serial clock | OUT |
| 22  | Passive infrared sensor interrupt | IN |
| 26  | Reset button | IN |
| 27  | Battery warning led | OUT |
| 32  | Battery ADC | IN |
| 25  | Battery ok led | OUT |
| 13  | Network warning led | OUT |

## Battery
Since the device is battery powered it needs to be able to tell how much battery capacity is left. This is done using a SAR ADC which are built into the ESP. The ADC has an range from 150 to 2350 mV, this means that it does not have enough range to measure the whole battery, which can be up to 4.2 volts. This voltage would also damage the GPIO since these can only handle voltages of up to 3.3 volts.
To overcome this issue an voltage divider is used. This voltage cuts the voltage in half.

This means that a full battery would result in an ADC measuring met 2.1v (4.2v / 2).

## Display
The display that is used in this model is a 2.9 inch ePaper module from waveshare:

![image](https://user-images.githubusercontent.com/45065264/214900684-8b8c8cce-bc4a-4c4a-80dd-561a21cc0fa7.png)

This ePaper display uses an SPI interface to communicate with the ESP. The ESP is able to set all the pixels on this display. The contents of the display are retained when the power is turned off, making it suitable for low power applications.

## LED Strip
The LED strip that is used to represent a analog clock. This clock is able to show the time by turning on the LEDs on the strip that are on the location of this certain time. 

## LEDs
The device uses 3 seperate LEDs to display any warnings to the user. These warnings are network issues and battery statuses.
