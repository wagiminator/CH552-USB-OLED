// ===================================================================================
// Project:   USB-HID to I2C Bridge for CH551, CH552, CH554
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
// This code implements a simple USB HID to I2C bridge. Data coming in via USB will be
// directly send via I2C to the slave device. Each HID packet must begin with the
// I2C address of the slave device.
//
// References:
// -----------
// - Blinkinlabs: https://github.com/Blinkinlabs/ch554_sdcc
// - Deqing Sun: https://github.com/DeqingSun/ch55xduino
// - Ralph Doncaster: https://github.com/nerdralph/ch554_sdcc
// - WCH Nanjing Qinheng Microelectronics: http://wch.cn
// - TinyTerminal: https://github.com/wagiminator/ATtiny85-TinyTerminal
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
// - Connect the board via USB to your PC. It should be detected as a HID device.
// - Run 'python3 hid-bridge-demo.py' or 'python3 hid-bridge-conway.py'.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================

// Libraries
#include <system.h>                       // system functions
#include <delay.h>                        // for delays
#include <i2c.h>                          // for I²C
#include <usb_hid.h>                      // for USB HID data

// Prototypes for used interrupts
void USB_interrupt(void);
void USB_ISR(void) __interrupt(INT_NO_USB) {
  USB_interrupt();
}

// ===================================================================================
// Main Function
// ===================================================================================

void main(void) {
  // Variables
  uint8_t len;

  // Setup
  CLK_config();                           // configure system clock
  DLY_ms(5);                              // wait for clock to stabilize
  HID_init();                             // init USB HID
  I2C_init();                             // init I2C

  // Loop
  while(1) {
    if(HID_available()) {                 // received data packet?
      len = HID_available();              // get number of bytes in packet
      I2C_start();                        // start I2C transmission
      while(len--) I2C_write(HID_read()); // pass all bytes in packet to I2C
      I2C_stop();                         // stop I2C transmission
    }
  }
}
