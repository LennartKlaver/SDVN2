# SDVN2
This repository contains the project files for the updated version of our visible light communication platform. It is work in progress so come back later to see our results.

# Transmitter testing.
In the folder tools you can find two arduino projects for a basic VLC transmitter using the pulse position modulation scheme. It will transmit each byte it receives at the serial (USB) port.
The VLC_TX_PWM_FREE version runs without interrupts and has faster IO results than the interrupt version.

NOTE:
Connect the LED driving circuit to arduino pin 6, but be careful as we use direct PORTB control instead of digitalwrite() so you should check if pin 6 is also PORTB6 for your Arduino/board.
