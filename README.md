- - - - 
# Principals of Embedded Software Assignment 7 #
#### Code for Assignment 7 for PES, ECEN-5813, Fall 2020 ####
#### Author Jake Michael ####
#### CU identikey: jami1063 ####
#### CU ID: 101920267 ####
#### Revision: 1.2 ####

- - - -
### Introduction ###
This repository is a private repository which contains the IDE project files for ECEN-5813, Principles of Embedded Software (PES), Assignment 7. This code should not be used by any third parties without the author's express written consent (which will not be given to other PES students). This project explores direct memory access (DMA), digital-to-analog conversion (DAC), and analog-to-digital conversion (ADC) with the KL25Z development board. 
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
### Analog Interfacing ### 
This assignment primarily explores analog interfacing. The assignment consists of two primary modules that interface together: analog_input.[c/h] and analog_output.[c/h]. The analog input module primarily deals with the ADC and TPM1, while the analog output module deals with the DAC, DMA and TPM0. The fixed_sine.[c/h] module presents a lookup table based fixed-point sine approximation. The `fp_sin` function returns values between -2037 and 2037 with fixed point integer inputs between INT_MIN and INT_MAX. The premise of the code is that we fill a buffer of sine wave output values to play a tone at a prescribed frequency. We then use DMA to consistently load the sine wave values into the 12-bit DAC with an output rate of 48 kHz (set by TPM0). While the DAC is looping, we use the ADC to sample the DAC output at 96 kHz (via TPM1) and run some statistical analysis on the samples. We output the results of the statistical analysis to UART including: minimum, maximum, average, period (calculated via Howdy's autocorrelate), and frequency (based on said period).         

- - - -
### Extra Credit ###
The following waveforms were captured with the Siglent SDS 1104X-E oscilloscope. They are generally inline with the UART statistical output. The oscilloscope results give me even more confidence than the fixed point autocorrelate output. The scopeshots show the mean frequency measurement of the DAC output waveforms are all within 0.1% of the coded value. 


![440 Hz DAC Output](scopeshots/440Hz.png)
![587 Hz DAC Output](scopeshots/587Hz.png)
![659 Hz DAC Output](scopeshots/659Hz.png)
![880 Hz DAC Output](scopeshots/880Hz.png)

- - - - 
### Project Link: [https://github.com/jmichael16/PES-Assignment-7](https://github.com/jmichael16/PES-Assignment-7) ###
- - - -
