- - - - 
# Principals of Embedded Software Final Project #
#### Code for Final Project for PES, ECEN-5813, Fall 2020 ####
#### Author Jake Michael ####

- - - -
### Introduction ###
This repository contains code for my Final Project for ECEN 5813 (Principles of Embedded Software) at CU, Boulder for Fall 2020. The ultimate goal for the project was to use several of the topics discussed throughout the course to create something interesting for the FRDM-KL25Z development platform. My idea for the project was to do music visualization using a microphone, the ADC and some LEDs. 

- - - -
### Recommended Prerequisites ###
This code should be run on the NXP FRDM-KL25Z development board and built with NXP's MCUxpresso IDE.

- - - -
### Compiling and Running the Code: ###
The user should run the code after importing the project into NXP's MCUxpresso IDE. After cloning the git repository do the following to compile and run the code:
1. Place the cloned directory in the active workspace for your MCUxpresso configuration. 
2. In MCUxpresso go to `File->Import` then select `General->Existing Projects into Workspace`. 
3. Browse for the file directory of the cloned git repository, select the project and finish the import wizard.
4. Now you should be able to see the project in the IDE workspace. The code is now ready to be built and run / debugged from the IDE. 

- - - - 
### Hardware ### 
This project utilized various hardware components, listed below:
* FRDM-KL25Z development board.
* [Sparkfun Sound Detector](https://www.sparkfun.com/products/12642) microphone module.
* [WS2812B's](https://www.digikey.com/en/datasheets/parallaxinc/parallax-inc-28085-ws2812b-rgb-led-datasheet), popularly known as Neopixels.
* [74HCT245 IC](https://www.digikey.com/en/products/detail/texas-instruments/CD74HCT245E/38454) for 3.3 V to 5 V logic level conversion for Neopixels.
* 5V, 2A DC power supply.

The Sound Detector Module output was wired to the analog input pin of the KL25Z (Port C, Pin 0). Since Neopixels are typically driven with a 5 V power source at 5 V logic, the 74HCT245 was used to convert the logic level. This is a high quality (fast response time) CMOS Logic Octal-Bus Transceiver. The KL25Z output pin that drives the Neopixels is Port A, Pin 12. The schematic below shows the circuit design for logic level conversion:

<p align="center">
  <img src="74HCT245_sch.jpg" width="450" title="74HCT245">
</p>

Finally, I 3D printed a neopixel ring mount and soldered together 8 neopixels. Here are a few photos of the final breadboard and Neopixel ring:
<p align="center">
  <img src="Ring_Back.jpg" width="350" title="neopixel ring back">
</p>
<p align="center">
<img src="Project_Breadboard.jpg" width="350" title="project breadboard">
</p>

- - - -
### Software ###

#### Testing ####

- - - -
### Acknowledgements ###
Big thanks to the instructor Howdy Pierce and the TA's Saket Penurkar and Rakesh Kumar for a great semester! I truly enjoyed the course and want to thank them for making it all possible. 
- - - -

- - - - 
### Project Link: [https://github.com/jmichael16/PES-Final-Project](https://github.com/jmichael16/PES-Final-Project) ###
- - - -
