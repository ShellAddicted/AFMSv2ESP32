# AFMSv2ESP32

This is the library for the Adafruit Motor Shield V2 for ESP32 (esp-idf).    
It supports DC motors & Stepper motors with microstepping as well as stacking-support.  
It is *not* compatible with the V1 library!  

It will only work with https://www.adafruit.com/products/1438  

## Compatibility

Tested on ESP32D0WDQ6 (DevKitC)  
 - DC Motors: Tested (OK)  
 - Steppers: <b>NOT</b> Tested (?)  

## Disclaimer

Forked & Ported to ESP32 (esp-idf) by ShellAddicted  
<b>I AM NOT</b> affiliated in any way with Adafruit.

><b>Based on</b>: https://github.com/adafruit/Adafruit_Motor_Shield_V2_Library  
>Adafruit invests time and resources providing this open  
>source code, please support Adafruit and open-source hardware  
>by purchasing products from Adafruit!  
>Written by Limor Fried/Ladyada for Adafruit Industries.  
>BSD license, check license.txt for more information.  
>All text above must be included in any redistribution.  

# Getting Started
<b>NOTE: this code is not (yet) Production Ready.</b>  
You can use this library as a component for your project:  
```
mkdir -p <YOUR_PROJECT_ROOT>/components/
cd <YOUR_PROJECT_ROOT>/components/
git clone https://github.com/ShellAddicted/AFMSv2ESP32
```
