**ncs-arducam-mega-driver**

Overview
********
This project includes a Zephyr driver for the Arducam Mega camera sensor, and a couple of samples showing how to use it. 

The following samples are included:
 - take_picture: A simple example showing how to enable the camera, take a picture, and transfer the image data into memory. Nothing is done with the data other than log the number of bytes received. 
 - mass_img_capture: A modification of the mass sample in Zephyr, configured to use the external flash chip on the development kit to implement a simple USB memory stick. Upon reset the application will take a number of pictures and store it to the flash, and then enable the USB interface to allow the images to be read over USB. 

Requirements
************

- nRF Connect SDK v2.2.0
- Arducam Mega camera sensor
- One of the following supported boards:
	- nrf52840dk_nrf52840
	- nrf5340dk_nrf5340_cpuapp
