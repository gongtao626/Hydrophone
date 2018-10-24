   ********************************************************************************
		Release notes for USB HOST demo for Kinetis family
 
			Freescale Semiconductor November 2010
   ********************************************************************************

Host code demo waits for an HID mouse and basically displays the movement and the button
status of the mouse in the serial console.


===========
Basic Setup
===========

TWR-K20 jumper setup
----------------
J26	
1-2	ON


================
Demo explanation
================
Download the code and run it. After the terminal shows the message "waiting for mouse" use
an USB adapter to connect a standard HID mouse th to USB port in the TWR-serial board.
Kinetis will recognize the mouse and will show some information about it and user can
start moving te mouse and pressing the buttons. 

Note:
Host stacks needs more heap, than other applications therefore a separate linker file is need it. 

IAR -> 128KB_Pflash_usb.icf
CW -> 128KB_usb_host_Pflash.lcf