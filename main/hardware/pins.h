//Contains ALL pin numbers of used peripherals

//The led strip data line, mux to RMT channel, cannot be changed
#define LED_STRIP_DIN 22

//Display pins
#define BUSY_PIN 26 
#define RST_PIN 27 
#define DC_PIN 25

//Default VSPI pins, cannot be changed
#define CS_PIN 15
#define MOSI_PIN 23
#define SCK_PIN 18

//Presence sensor interrupt pin, should be connected with resistor to GND.
#define SENSOR_INT 4

//Reset button, should be connected with resistor to GND.
#define RESET_BUTTON 2

//ADC pin
#define BATTERY_ADC 32

//Pin for driving the battery warning LED
#define BATTERY_WARNING_LED 19
#define BATTERY_OK_LED 12

//Pin for driving the network warning LED
#define NETWORK_WARNING_LED 13
