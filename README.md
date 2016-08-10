# PIC16F684-periodic-plant-watering
Plant watering system using the PIC16F684 microcontroller.
The MCU commands a 12V solenoid valve to open with specific period, and close after a specific interval.
The period and interval can be configured over Bluetooth using Android phone with RoboRemo app.
The settings are stored in the EEPROM memory of the PIC, so they don't get lost at power OFF.

Commands:

"get\n" - get parameter values (p = period, i = open interval)  
"time\n" - get up time (how many seconds passed since system was powered)  
"set p [n]\n" - set period, in seconds. Example: "set p 300\n" - valve wil open every 5 minutes  
"set i [n]\n" - set the open interval, in seconds. Example: "set i 20\n" - valve will stay open for 20 seconds  
"open\n" - manually open valve (for test)  
"close\n" - manually close valve (for test)  

The push button is used to turn ON the Bluetooth module, which is off by default, to reduce the power consumption. The system turns off the Bluetooth module automatically after 30s if not receiving any command.

The Bluetooth module used in this project is the BlueSMiRF, configured to 9600 baud.

The blue LED blinks every 2s to signal that system is running.

Current draw (with Bluetooth OFF and valve OFF) is about 6mA.
That gives a battery life of about 30 days (with 5Ah 12V battery)
