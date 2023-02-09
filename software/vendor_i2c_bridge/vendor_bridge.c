// ===================================================================================
// Project:   USB Vendor Class to I2C Bridge for CH551, CH552, CH554
// Version:   v1.0
// Year:      2022
// Author:    Stefan Wagner
// Github:    https://github.com/wagiminator
// EasyEDA:   https://easyeda.com/wagiminator
// License:   http://creativecommons.org/licenses/by-sa/3.0/
// ===================================================================================
//
// Description:
// ------------
// This code implements a simple USB vendor class to I2C bridge. The start and 
// stop condition on the I2C bus is set according to an appropriate vendor class 
// control request. Data received via USB bulk transfer is passed directly to the 
// slave device via I2C.
// Vendor control requests can also be used to control the buzzer or put the 
// microcontroller into boot mode.
// This firmware also includes an experimental implementation of a Windows 
// Compatible ID (WCID). This allows to use the device without manual driver 
// installation on Windows system. However, since I (un)fortunately do not have a 
// Windows system, this function is untested. This feature can be switched on or 
// off in the configuration file (config.h). If not used, a manual installation of 
// the libusb-win32 driver via the Zadig tool (https://zadig.akeo.ie/) is required 
// on Windows systems.
//
// References:
// -----------
// - Blinkinlabs: https://github.com/Blinkinlabs/ch554_sdcc
// - Deqing Sun: https://github.com/DeqingSun/ch55xduino
// - Ralph Doncaster: https://github.com/nerdralph/ch554_sdcc
// - WCH Nanjing Qinheng Microelectronics: http://wch.cn
// - TinyTerminal: https://github.com/wagiminator/ATtiny85-TinyTerminal
// - Pete Batard: https://github.com/pbatard/libwdi/wiki/WCID-Devices
//
// Wiring:
// -------
//                                       +--\/--+
//                --------- SCS A1 P14  1|°     |10 V33
//         BUZZER --- PWM1 MOSI A2 P15  2|      |9  Vcc
//     OLED (SDA) ------ MISO RXD1 P16  3|      |8  GND
//     OLED (SCL) ------- SCK TXD1 P17  4|      |7  P37 UDM --- USB D-
//                ---------------- RST  6|      |6  P36 UDP --- USB D+
//                                       +------+
//
// Compilation Instructions:
// -------------------------
// - Chip:  CH551, CH552 or CH554
// - Clock: 16 MHz internal
// - Adjust the firmware parameters in include/config.h if necessary.
// - Make sure SDCC toolchain and Python3 with PyUSB is installed.
// - Press BOOT button on the board and keep it pressed while connecting it via USB
//   with your PC.
// - Run 'make flash'.
//
// Operating Instructions:
// -----------------------
// - Connect the board via USB to your PC. It should be detected as a vendor class 
//   device.
// - Run 'python3 vendor-bridge-demo.py' or 'python3 vendor-bridge-conway.py'.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================

// Libraries
#include <config.h>                       // user configurations
#include <gpio.h>                         // GPIO functions
#include <system.h>                       // system functions
#include <delay.h>                        // for delays
#include <i2c.h>                          // for I²C
#include <usb_vendor.h>                   // for USB vendor-specific functions

// Prototypes for used interrupts
void USB_interrupt(void);
void USB_ISR(void) __interrupt(INT_NO_USB) {
  USB_interrupt();
}

// ===================================================================================
// Main Function
// ===================================================================================

void main(void) {
  // Setup
  CLK_config();                                 // configure system clock
  DLY_ms(5);                                    // wait for clock to stabilize
  VEN_init();                                   // init USB vendor-specific device
  I2C_init();                                   // init I2C
  PWM_set_freq(2000);                           // set buzzer tone frequency
  PWM_write(PIN_BUZZER, 127);                   // set buzzer duty cycle 50%

  // Loop
  while(1) {
    if(VEN_BOOT_flag)   BOOT_now();             // enter bootloader?
    if(VEN_BUZZER_flag) PWM_start(PIN_BUZZER);  // buzzer start?
    else {                                      // buzzer stop?
      PWM_stop(PIN_BUZZER);
      PIN_high(PIN_BUZZER);
    }

    if(VEN_I2C_flag) {                          // I2C start?
      I2C_start();                              // set I2C start condition
      while(VEN_I2C_flag) {                     // repeat for all incoming bytes
        while(VEN_available())                  // incoming bulk data?
          I2C_write(VEN_read());                // write received data byte via I2C
      }
      I2C_stop();                               // set I2C stop condition
    }
  }
}
