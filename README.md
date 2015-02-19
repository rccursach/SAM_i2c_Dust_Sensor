#ATTiny85 I2C Firmware for interfacing Sharp gp2y1010 Dust Sensor#

This firmware interfaces the Sharp GP2Y1010AUF Dust Sensor adding I2C bus
comunication as a slave device, allowing its use on non-real-time and/or lacking
of ADC devices like the Raspberry Pi computer or similar.

##Building the firmware##

This are the steps and considerations for building and uploading the firmware
into the ATTiny85 microcontroller.

As a summary what's needed for building and uploading a firmware is:

* An ATTiny85 microcontroller
* AVR Programmer (Or another arduino as ISP programmer)
* arduino-tiny Core (to code with arduino instead of bare AVR C code)
* TinyWireS Library (Provides I2C Slave implementation on top of Atmel's USI)
* Arduino IDE


###ATTiny Arduino core###

This firmware has been built with the arduino-tiny core, there are other cores
arround but they might or might not work as those aren't been testet yet.

This core can be found on: https://code.google.com/p/arduino-tiny

The content of this repository in its own directory must be located on a
directory named "hardware" inside your arduino sketchbook.
(you must avoid symbols on the directory names).

Ej: "/home/rccursach/Sketchbook/hardware/arduinotiny/"


###TinyWireS Library ####

ATTiny85 uControllers doesn't have built-in support for I2C and SPI protocols,
Instead they have (Universal Serial Interface) USI, which is a multi-purpose
serial hardware implementation. SPI and I2C protocols are supposed to be
suported as software implementations using the USI hardware capabilities.

The I2C Implementation used is the one founded on Arduino's site: 
http://playground.arduino.cc/Code/USIi2c

This library must be located on a directory named "libraries" inside your
Sketchbook directory.

Ej: "/home/rccursach/Sketchbook/libraries/TinyWireS/"


##Compiling and upload##


###ATTiny85 @ 16 MHz (Internal PLL)###

The first step after opening the firmware's sketch is selecting what board
model, clock speed, and programmer use.

The correct/tested model is "ATTiny85 @ 16 MHz (Internal PLL; 4.3 V BOD)" which
means that the microcontroller will run at 16MHz without an external crystal or
resonator, leaving all pins free for use.

The minimum speed migth be 8 MHz for TinyWireS, but i found it not suitable for
use with the Raspberry Pi computer, showing corruption/erratic behaviour at
least 60% of the time, while it worked without any trouble at 8MHz communicating
with an Arduino UNO as I2C Master


###Select Programmer, Burn Bootloader and Upload###

At this point you should just connect your programmer with the ATTiny attached
to it, select what programmer are you using in Arduino IDE's Menu
(Tools > Programmer).

Important: You MUST now go to (Tools > Burn Bootloader) which will not upload a
bootloader but set up the ATTiny85 Fuses to make it run at 16 MHz (or whatever
speed you choose avove 8MHz)

You can now press the Verify/Compile to check for errors if any, and if
everything goes well, press Upload button and load the firmware into your
ATTiny85 microcontroller.



##Troubleshooting##


If you found errors while compiling make shure:

* You selected the correct model (ATTiny85)
* The TinyWireS library is on your Sketchbook/libraries directory. (It will appear as "TinyWireS" on the "Sketch > Import Library.." menu's list)
* You are using the same Arduino Core listed above, i had some trouble using some of the other cores available.


If there are errors uploading the firmware check if you:

* Choose the right programmer model on the list.
* Burned the bootloader (do it again if not sure).
* have loose/wrong connections between the IC and the AVR programmer.

