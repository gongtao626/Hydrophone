
********************************************************************************
    Release notes for low power demo for 50MHz kinetis family
		  Freescale Semiconductor February 2012   
********************************************************************************
TWRK20D50M Demo Software Rev 1.4 

Demo code runs on the TWR-K20D50M module.

The objective of this demo is to get familiar with TWR-K20D50M, 

Note For details on this software please refere to TWRK20D50MQSLAB document

==============
What you need:
==============
1)	IAR Embedded Workbench for ARM v6.30 
2)	P&E OSBDM OSJTAG Virtual Serial Toolkit

===========
Basic Setup
===========

TWR-K20D50M setup
----------------
Default configration as specified in TWR-K20D50M_UM


================
Demo explanation
================

1. Download the code to MCU. Make sure to disconnect the debugger by stopping debugging or with a power on reset.

2. Open the P&E Micro Terminal Utility and connect the USB COM at 115200 bps with no parity and 8 bit size.

3. Reset the board. A prompt will appear showing reset information and all operation modes.

type "disp"  for available functions




functions demonstrated  with this software are

Module	Function	Details
ADC	Reads 2 signals	Potentiometer	Microphone
PDB	Creates a sampling frequency of 8 kHz for the ADC converter 8 Khz Triggers ADC0, 
DMA	Store microphone readings in a double buffer of 128 samples each
	Interrupt  when 128 samples are sampled and perform calculation of FFT, and filters.
DSP	Calculates FFT of 128 samples and magnitude of each harmonic,Uses CMSIS integrated library in IAR 6.3
I2C	Reads accelerometer, acceleration in each axes x, y, and z	K20 calculates tilt angle and fall detection          based on axes acceleration.
PIT	CH0 Generates a 1 msec periodic interrupt
        CH1 Generates a 125 msec periodic interrupt	
GPIO	4 LED, 2 Push button	
HSCMP	Reads signal for IR receiver, and output is connected to UART0 receiver 	DAC6 used as HSCMP minus         input, IR receiver connected to HSCMP plus input,
MCG+OSC	Oscillator of 8MHz crystal, MCG creates a 50 MHz signal from 8MHz crystal with the PLL	
CMT	Create a 40KHz carrier frequency, and 600 Baud modulation signal for the IR transmitter. 	
UART0	Receives signal from HSCMP(IR receiver) 600,8,n,1	Inverted reception
TSI	Configured to read 2 Electrodes	
RTC	A real time clock  (seconds only) based on 32Khz Crystal	
JTAG	Interface of debugging and programming of the parts.
Programmed through the OSJTAG	
UART1	User communication interface  38400,8,n,1	
FTM0	Generate buzzer frequency	

