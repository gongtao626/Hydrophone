
********************************************************************************
    Release notes for low power demo for 50MHz kinetis family
		  Freescale Semiconductor February 2012   
********************************************************************************

Demo code runs on the TWR-K20D50M module.

The objective of this demo is to get familiar with the steps needed to use low power modes available in Kinetis family, and provide considerations to get the lowest consumption.

Note: For a complete guide of this lab read aplication note "AN4470: Using Low Power modes on Kinetis family ".

==============
What you need:
==============
1)	IAR Embedded Workbench for ARM v6.30 or CodeWarrior MCU V10.1 (with MCU 10.1 Kinetis 50MHz Service Pack installed)
2)	P&E OSBDM OSJTAG Virtual Serial Toolkit

===========
Basic Setup
===========

TWR-K20D50M setup
----------------
Use J25 jumper to measure power consumption.
Use J29 Connector on position 3-5 to supply MCU at 3.3V or 5-7 to supply at 1.8V.
J35 VBAT could be removed to get lower consumption. 

================
Demo explanation
================

1. Download the code to MCU. Make sure to disconnect the debugger by stopping debugging or with a power on reset.

2. Open the P&E Micro Terminal Utility and connect the USB COM at 115200 bps with no parity and 8 bit size.

3. Reset the board. A prompt will appear showing reset information and all operation modes.

4. Select one operation mode writing the corresponding number on the menu. 

5. Press SW2 to wake up from LLS and VLLSX modes. SW3 or SW2 to wake up from Wait, Stop, VLPW and VLPS. For VLPR select Exit VLPR option. 

6. After Wake up the menu appears again. 