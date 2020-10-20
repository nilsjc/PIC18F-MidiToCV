# PIC18F-MidiToCV

A midi to control voltage converter made with Microchip pic 18F4620.
It should be no problem to compile this code to other 18F-models.
This is in a experimental state. For now it takes the note number and put it on PORTD, while RB0 puts the gate out. However in future i will use a separate DAC instead, connected with SPI.
It works in monophonic state.
Internal clock is used.
Connect MIDI thru a optocoupler to the RC7 pin.
Easiest way to compile this is just to create a new project in MPLABX for your selected MCU and copypaste this code into the main file. Note my configuration settings in the beginning though, it is made for 18F4620. Replace them if other controller is used.

Have fun!
