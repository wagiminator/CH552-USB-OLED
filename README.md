# USB-controlled I²C OLED based on CH552E
USB-OLED is a simple USB-controlled 128x64 pixels I2C OLED display. The CH552E (or CH554E) microcontroller builds a [USB Communication Device Class (CDC)](https://en.wikipedia.org/wiki/USB_communications_device_class) for serial communication over USB, which can be used to transfer data from the PC to the OLED. An integrated buzzer offers the possibility of outputting acoustic signals.

![USB_OLED_pic4.jpg](https://raw.githubusercontent.com/wagiminator/CH552-USB-OLED/main/documentation/USB_OLED_pic4.jpg)

# Hardware
## Schematic
![USB_OLED_wiring.png](https://raw.githubusercontent.com/wagiminator/CH552-USB-OLED/main/documentation/USB_OLED_wiring.png)

## CH552E 8-bit USB Device Microcontroller
The CH552E is a low-cost, enhanced E8051 core MCU compatible with the MCS51 instruction set. It has an integrated USB controller with USB2.0 data transfer at full speed (12 Mbit/s) and supports up to 64 byte data packets with integrated FIFO and direct memory access (DMA). The CH552E has a factory built-in bootloader so firmware can be uploaded directly via USB without the need for an additional programming device.

![USB_OLED_pic2.jpg](https://raw.githubusercontent.com/wagiminator/CH552-USB-OLED/main/documentation/USB_OLED_pic2.jpg)

# Software
## CDC OLED Terminal
This firmware implements a simple terminal for displaying text messages on the OLED. It can be use with any serial monitor on your PC. The integrated buzzer gives an acoustic signal for every message received.

![USB_OLED_pic3.jpg](https://raw.githubusercontent.com/wagiminator/CH552-USB-OLED/main/documentation/USB_OLED_pic3.jpg)

Operating Instructions:
- Connect the board via USB to your PC. It should be detected as a CDC device.
- Open a serial monitor and select the correct serial port (BAUD rate doesn't matter).
- Send a text message, it should be displayed on the OLED.

On Linux you can also send text messages via a terminal:

```
echo "Hello World!\n" > /dev/ttyACM0
```

## USB to I²C Bridge
This firmware implements a simple USB to I2C bridge. Any data transmission must begin with the PC software setting the RTS flag. This causes the firmware on the microcontroller to set the start condition on the I2C bus. Then all data bytes sent via USB CDC are passed directly to the I2C bus. This also means that each data stream must start with the I2C write address of the I2C slave device (in this case the OLED). When all data bytes have been transferred, the PC software must clear the RTS flag again, which causes the microcontroller to set the stop condition on the I2C bus. This mode of operation enables full control of the OLED via the PC. In principle, the firmware also allows the control of other I2C devices.

Two attached Python scripts show the PC-side implementation of the I2C bridge as an example. "bridge-demo.py" shows and scrolls an image, "bridge-conway.py" plays [Conway's Game of Life](https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life) on the OLED.

![USB_OLED_anim.gif](https://raw.githubusercontent.com/wagiminator/CH552-USB-OLED/main/documentation/USB_OLED_anim.gif)

Operating Instructions:
- Connect the board via USB to your PC. It should be detected as a CDC device.
- Run ```python3 bridge-demo.py``` or ```python3 bridge-conway.py```.

## Compiling and Installing Firmware
### Installing Toolchain for CH55x
Install the [CH55x Software Development Kit for the SDCC compiler](https://github.com/Blinkinlabs/ch554_sdcc). Follow the instructions on the website. In order for the programming tool and the I²C-Bridge software to work, Python3 must be installed on your system. To do this, follow these [instructions](https://www.pythontutorial.net/getting-started/install-python/). In addition [PyUSB](https://github.com/pyusb/pyusb) and [PySerial](https://github.com/pyserial/pyserial/) must be installed. On Linux (Debian-based), all of this can be done with the following commands:

```
sudo apt install sdcc python3 python3-pip
sudo pip install pyusb
sudo pip install pyserial
```

### Installing Drivers for the CH55x Bootloader
On Linux you do not need to install a driver. However, by default Linux will not expose enough permission to upload your code with the USB bootloader. In order to fix this, open a terminal and run the following commands:

```
echo 'SUBSYSTEM=="usb", ATTR{idVendor}=="4348", ATTR{idProduct}=="55e0", MODE="666"' | sudo tee /etc/udev/rules.d/99-ch55x.rules
sudo service udev restart
```

On Windows you will need the [Zadig tool](https://zadig.akeo.ie/) to install the correct driver for both, the CH55x bootloader and the CDC device. Click "Options" and "List All Devices" to select the USB module, then install the libusb-win32 driver.

### Entering CH55x Bootloader Mode
A brand new chip starts automatically in bootloader mode as soon as it is connected to the PC via USB. Once firmware has been uploaded, the bootloader must be started manually for new uploads. To do this, the board must first be disconnected from the USB port and all voltage sources. Now press the BOOT button and keep it pressed while reconnecting the board to the USB port of your PC. The chip now starts again in bootloader mode, the BOOT button can be released and new firmware can be uploaded within the next couple of seconds.

### Compiling and Uploading Firmware
Open a terminal and navigate to the folder with the makefile. Run ```make flash``` to compile and upload the firmware. If you don't want to compile the firmware yourself, you can also upload the precompiled binary. To do this, just run ```python3 ./tools/chprog.py firmware.bin```.

# References, Links and Notes
1. [EasyEDA Design Files](https://oshwlab.com/wagiminator/ch552-usb2oled)
2. [CH551/552 Datasheet](http://www.wch-ic.com/downloads/CH552DS1_PDF.html)
3. [SSD1306 Datasheet](https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf)
4. [SDCC Compiler](https://sdcc.sourceforge.net/)
5. [CH55x SDK for SDCC](https://github.com/Blinkinlabs/ch554_sdcc)
6. [ATtiny85 TinyTerminal](https://github.com/wagiminator/ATtiny85-TinyTerminal)
7. [128x64 OLED on Aliexpress](http://aliexpress.com/wholesale?SearchText=128+64+0.96+oled+new+4pin)

# License
![license.png](https://i.creativecommons.org/l/by-sa/3.0/88x31.png)

This work is licensed under Creative Commons Attribution-ShareAlike 3.0 Unported License. 
(http://creativecommons.org/licenses/by-sa/3.0/)
