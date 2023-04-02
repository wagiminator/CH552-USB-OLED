// ===================================================================================
// USB HID Data Transfer Functions for CH551, CH552 and CH554
// ===================================================================================

#include "ch554.h"
#include "usb.h"
#include "usb_hid.h"
#include "usb_descr.h"
#include "usb_handler.h"

// ===================================================================================
// Variables and Defines
// ===================================================================================

volatile __xdata uint8_t HID_EP1_readByteCount = 0; // number of data bytes in IN buffer
volatile __xdata uint8_t HID_EP1_readPointer   = 0; // data pointer for fetching
volatile __bit HID_EP1_writeBusyFlag = 0;           // flag of whether upload pointer is busy
__xdata uint8_t HID_EP1_writePointer = 0;           // data pointer for writing

// ===================================================================================
// Front End Functions
// ===================================================================================

// Setup USB HID
void HID_init(void) {
  USB_init();
  UEP1_T_LEN  = 0;
}

// Flush the OUT buffer
void HID_flush(void) {
  if(!HID_EP1_writeBusyFlag && HID_EP1_writePointer > 0) {  // not busy and buffer not empty?
    UEP1_T_LEN = HID_EP1_writePointer;                      // number of bytes in OUT buffer
    UEP1_CTRL  = UEP1_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_ACK; // respond ACK
    HID_EP1_writeBusyFlag = 1;                              // busy for now
    HID_EP1_writePointer  = 0;                              // reset write pointer
  }
}

// Write single byte to OUT buffer
void HID_write(uint8_t b) {
  while(!HID_ready());                                      // wait for ready to write
  EP1_buffer[MAX_PACKET_SIZE + HID_EP1_writePointer++] = b; // write data byte
  if(HID_EP1_writePointer == EP1_SIZE) HID_flush();         // flush if buffer full
}

// Read single byte from IN buffer
uint8_t HID_read(void) {
  uint8_t b;
  while(!HID_available());                                  // wait for data
  b = EP1_buffer[HID_EP1_readPointer++];                    // get data byte
  if(--HID_EP1_readByteCount == 0)                          // dec number of bytes in buffer
    UEP1_CTRL = UEP1_CTRL & ~MASK_UEP_R_RES | UEP_R_RES_ACK;// request new data if empty
  return b;
}

// ===================================================================================
// HID-Specific USB Handler Functions
// ===================================================================================

// Setup HID endpoints
void HID_setup(void) {
  UEP1_DMA    = EP1_ADDR;                   // EP1 data transfer address
  UEP1_CTRL   = bUEP_AUTO_TOG               // EP1 Auto flip sync flag
              | UEP_T_RES_NAK               // EP1 IN transaction returns NAK
              | UEP_R_RES_ACK;              // EP1 OUT transaction returns ACK
  UEP4_1_MOD  = bUEP1_RX_EN | bUEP1_TX_EN;  // EP1 double buffer
}

// Reset HID parameters
void HID_reset(void) {
  UEP1_CTRL = bUEP_AUTO_TOG | UEP_T_RES_NAK | UEP_R_RES_ACK;
  HID_EP1_readByteCount = 0;                // reset received bytes counter
  HID_EP1_writeBusyFlag = 0;                // reset write busy flag
}

// Endpoint 1 IN handler (HID data transfer to host)
void HID_EP1_IN(void) {
  UEP1_T_LEN = 0;                                           // no data to send anymore
  UEP1_CTRL = UEP1_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_NAK;  // default NAK
  HID_EP1_writeBusyFlag = 0;                                // clear busy flag
}

// Endpoint 1 OUT handler (HID data transfer from host)
void HID_EP1_OUT(void) {
  if(U_TOG_OK) {                            // discard unsynchronized packets
    HID_EP1_readByteCount = USB_RX_LEN;     // set number of received data bytes
    HID_EP1_readPointer = 0;                // reset read pointer for fetching
    if(HID_EP1_readByteCount) 
      UEP1_CTRL = UEP1_CTRL & ~MASK_UEP_R_RES | UEP_R_RES_NAK; // respond NAK after a packet. Let main code change response after handling.
  }
}
