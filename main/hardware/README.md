# Hardware

## Overview
The ESP is connected to several hardware components, how these components are wired up is described below:

![image jpeg](https://user-images.githubusercontent.com/45065264/214882115-4a8cf49a-1bd1-4b1d-9f39-1ae300eaf52d.jpg)

### Pin numbering

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
| 15  | Network warning led | OUT |

## Battery
Since the device is battery powered it needs to be able to tell how much battery capacity is left. This is done using a SAR ADC which are built into the ESP. The ADC has an range from 150 to 2350 mV, this means that it does not have enough range to measure the whole battery, which can be up to 4.2 volts. This voltage would also damage the GPIO since these can only handle voltages of up to 3.3 volts.
To overcome this issue an voltage divider is used. This voltage cuts the voltage in half.

This means that a full battery would result in an ADC measuring met 2.1v (4.2v / 2).

## Display


## LED Strip

## LEDs
