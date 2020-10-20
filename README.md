# PIC18F-MidiToCV

A midi to control voltage converter made with Microchip pic 18F4620.
It should be no problem to compile this code to other 18F-models.
Experimental state. For now it takes the note number and put it on PORTD, while PORTB pin 0 puts the gate out. However in future i will use a separate DAC instead, connected with SPI.
It works in monophonic state.
Internal clock is used.

Easiest way to compile this is just to copypaste this code into MPLABX. Note the configuration settings in the beginning though, it is made for 18F4620. 

Have fun!