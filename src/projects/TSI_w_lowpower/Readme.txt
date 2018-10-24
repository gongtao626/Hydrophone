
********************************************************************************
    Release notes for Touch sensing + low power demo for 50MHz kinetis family
		  Freescale Semiconductor February 2012   
********************************************************************************

Demo code runs on the TWR-K20D50M module.

TSI_w_lowpower runs a simple touch sensing algorithm on the two touch electrodes found in the board. The LEDs in the electrode center will toggle on and off each time the electrode is touched. The touch sensing algorithm is very simple:

1. Init part and TSI module.
2. Run a single measurement of the electrodes. Measured values will be established as baseline values. Precalibrated touch thresholds based on a delta value above the baseline are also stored at this point for active mode as well as for low power wake-up.
3. Touch algorithm starts. Touch algorithm is completely interrupt-driven.
4. Users can go into sleep modes LLS by sending any character through a terminal utility, wake-up with touch and then send another character to go into VLLS1 mode.

===========
Basic Setup
===========

TWR-K20D50M setup
----------------
Use default jumper setting for board.

================
Demo explanation
================

1. Download the code to MCU. Make sure to disconnect the debugger by stopping debugging.

2. Open the P&E Micro Terminal Utility and connect the USB COM at 115200 bps with no parity and 8 bit size.

3. Reset the board. A prompt will appear showing reset information and requesting a character to go into LLS mode.

4. LEDs can be toggled by touching the electrodes at this time.

5. When any character is sent through the terminal utility the MCU will go into low power LLS mode. 

6. Toggle on electrode 1 will stop working. Touching electrode 0 will wake-up the MCU.

7. Upon wake-up user can enter into VLLS1 by sending another character through the terminal utility.

8. The default VLLSx mode that will be entered is VLLS1, but users can also test VLLS2 or 3 by defining the symbol VLLS2 or VLLS3 in the main file.

9. User can wake up the device from VLLS1 by touching electrode 0 again, this wake-up will be a reset.