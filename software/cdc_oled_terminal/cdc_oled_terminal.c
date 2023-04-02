// ===================================================================================
// Project:   USB CDC OLED Terminal for CH551, CH552, CH554
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
// This code implements a simple USB controlled 128x64 pixels I2C OLED display.
// The CH55x builds a USB communication device class (CDC) for serial communication
// via USB. Text messages of all kinds can be sent via the USB interface and shown
// on the OLED display. The integrated buzzer gives an acoustic signal for every
// message received.
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
// - Open a serial monitor and select the correct serial port (BAUD rate doesn't 
//   matter).
// - Send a text message, it should be displayed on the OLED.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================

// Libraries
#include "src/config.h"                   // user configurations
#include "src/system.h"                   // system functions
#include "src/gpio.h"                     // for GPIO
#include "src/delay.h"                    // for delays
#include "src/oled.h"                     // for OLED
#include "src/usb_cdc.h"                  // for USB-CDC serial

// Prototypes for used interrupts
void USB_interrupt(void);
void USB_ISR(void) __interrupt(INT_NO_USB) {
  USB_interrupt();
}

// ===================================================================================
// Buzzer Function
// ===================================================================================

// Create a short beep on the buzzer
void beep(void) {
  uint8_t i;
  for(i=255; i; i--) {
    PIN_low(PIN_BUZZER);
    DLY_us(125);
    PIN_high(PIN_BUZZER);
    DLY_us(125);
  }
}

// ===================================================================================
// Main Function
// ===================================================================================

void main(void) {
  // Setup
  CLK_config();                           // configure system clock
  DLY_ms(5);                              // wait for clock to stabilize
  CDC_init();                             // init USB CDC
  OLED_init();                            // init OLED

  // Print start message
  OLED_print("* CDC OLED TERMINAL *");
  OLED_print("---------------------");
  OLED_print("Ready\n");
  OLED_print("_\r");
  beep();

  // Loop
  while(1) {
    if(CDC_available()) {                 // something coming in?
      char c = CDC_read();                // read the character ...
      OLED_write(c);                      // ... and print it on the OLED
      if((c == 10) || (c == 7)) beep();   // beep on newline command
    }
  }
}
