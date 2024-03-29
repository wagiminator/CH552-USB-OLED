// ===================================================================================
// Project:   USB-CDC to I2C Bridge for CH551, CH552, CH554
// Version:   v1.1
// Year:      2022
// Author:    Stefan Wagner
// Github:    https://github.com/wagiminator
// EasyEDA:   https://easyeda.com/wagiminator
// License:   http://creativecommons.org/licenses/by-sa/3.0/
// ===================================================================================
//
// Description:
// ------------
// This code implements a simple USB to I2C bridge. Data coming in via USB will be
// directly send via I2C to the slave device. Each data stream must begin with the
// I2C address of the slave device. During the transmission the RTS flag must be set.
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
// - Adjust the firmware parameters in src/config.h if necessary.
// - Make sure SDCC toolchain and Python3 with PyUSB is installed.
// - Press BOOT button on the board and keep it pressed while connecting it via USB
//   with your PC.
// - Run 'make flash' immediatly afterwards.
// - To compile the firmware using the Arduino IDE, follow the instructions in the 
//   .ino file.
//
// Operating Instructions:
// -----------------------
// - Connect the board via USB to your PC. It should be detected as a CDC device.
// - Run 'python3 bridge-demo.py' or 'python3 bridge-conway.py'.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================

// Libraries
#include "src/system.h"                   // system functions
#include "src/delay.h"                    // for delays
#include "src/i2c.h"                      // for I²C
#include "src/usb_cdc.h"                  // for USB-CDC serial

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
  CLK_config();                           // configure system clock
  DLY_ms(5);                              // wait for clock to stabilize
  CDC_init();                             // init USB CDC
  I2C_init();                             // init I2C

  // Loop
  while(1) {
    if(CDC_getRTS()) {                    // incoming CDC data stream?
      I2C_start();                        // start I2C transmission
      while(CDC_getRTS()) {               // repeat for all incoming bytes
        while(CDC_available())            // incoming CDC data byte?
          I2C_write(CDC_read());          // write received data byte via I2C
      }
      I2C_stop();                         // stop I2C transmission
    }
  }
}
