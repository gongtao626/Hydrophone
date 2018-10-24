
********************************************************************************
    Release notes for HSCMP demo for 50MHz kinetis family
		  Freescale Semiconductor February 2012   
********************************************************************************

Demo code runs on the TWR-K20D50M module.

This example uses IR transmitter on PTD7 to drive the
IR receiver. They are angled so that the IR transmitter
can drive the IR receiver on the tower board. This
requires shunting J7 and J9, and remove J4.

P input of the comparator has a value of threshold, and the M input is the IR receiver signal,  
when the output voltage of the IR is less than the threshold value the comparator is active and 
it send messages to the terminal each time the output toggle, that’s because the IR is supplied 
with a square voltage signal.

To enable IR transmit put the board on a white paper and in order to stop 
transmiting lift the board.
Alternatively you can point a remote to the IR
receiver and toggle the HSCMP that way.



===========
Basic Setup
===========

TWR-K20D50M setup
----------------
J4  LED header
1-2  OFF

J7 IR OUTPUT header
1-2	ON

J9 IR INPUT header
1-2	ON

================
Demo explanation
================

1. Download the code to MCU. Make sure to disconnect the debugger by stopping debugging.

2. Disconnect and connect the USB cable of the Tower board.

2. Open the P&E Micro Terminal Utility and connect the USB COM at 115200 bps with no parity and 8 bit size.

3. Reset the board. A prompt will appear showing reset information. 

4. Put the board on a white paper to enable IR transmit, lift the board to stop transmiting.

