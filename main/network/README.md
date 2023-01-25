# Network

## HTTP
The HTTP files contain an implementation to retrieve data from the internet using HTTP GET requests. It is currently only being used to retrieve data from [Forecast.solar](https://forecast.solar/).

The HTTP files also contain instructions on how to process an HTTP GET request. This GET request is retrieved by the webpage hosted on the esp, in case it is currently being used as Wi-Fi hotspot.

## Page
Whenever the device is waiting for the user to set settings a webpage is hosted. The contents of this webpage can be found in the page.c and page.h files.

## Time
To retrieve the current time the device uses the SNTP (Simple network time protocol). This protocol uses a free to use time server to get its localtime. This time is used to display the time on the ePaper display.

