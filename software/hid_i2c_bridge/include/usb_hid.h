// ===================================================================================
// USB HID Data Transfer Functions for CH551, CH552 and CH554
// ===================================================================================

#pragma once
#include <stdint.h>

#define HID_available()   (HID_EP1_readByteCount)   // number of received bytes
#define HID_ready()       (!HID_EP1_writeBusyFlag)  // check if ready to write

void HID_init(void);                                // setup USB-HID
uint8_t HID_read(void);                             // read byte from IN buffer
void HID_write(uint8_t b);                          // write byte to OUT buffer
void HID_flush(void);                               // flush the OUT buffer

extern volatile __xdata uint8_t HID_EP1_readByteCount;
extern volatile __bit HID_EP1_writeBusyFlag;
